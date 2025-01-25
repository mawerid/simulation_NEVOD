#include "control/InputManager.hh"

namespace nevod {

DataFile::DataFile(G4String data_dir_name, G4String dataset_name, G4int event_num)
    : data_dir_name(data_dir_name), dataset_name(dataset_name), event_num(event_num) {}

G4String DataFile::GetFileName() { return data_dir_name + "/" + dataset_name + "_" + std::to_string(event_num) + ".root"; }

InputManager::InputManager(Communicator* communicator, size_t offset): communicator_(communicator), offset_(offset) {
  auto params = communicator_->GetSimulationParams();
  path_ = params.input_path;
  thread_num_ = params.thread_num;

  current_file_id = 0;
  DetectFiles(path_);
}

void InputManager::DetectFiles(std::string path) {
  G4cout << "Detecting files in " << path << G4endl;

  for (const auto& entry: fs::recursive_directory_iterator(path)) {
    if (entry.is_regular_file()) {
      std::string file_name = entry.path().filename().string();

      if (file_name.find(".root") != std::string::npos) {
        // check root files
        auto file = TFile::Open(entry.path().string().c_str(), "READ");
        if (!file || file->IsZombie()) {
          G4cerr << "Error opening file " << entry.path().string() << " (zombie file)" << G4endl;
          file->Close();
          delete file;
          continue;
        } else if (file->GetNkeys() == 0) {
          G4cerr << "Error opening file " << entry.path().string() << " (empty file)" << G4endl;
          file->Close();
          delete file;
          continue;
        }
        file->Close();
        delete file;

        // add file to the list
        std::string data_dir_name = entry.path().parent_path().string();
        std::string dataset_name = file_name.substr(0, file_name.find_last_of("_"));
        G4int event_num = std::stoi(file_name.substr(file_name.find_last_of("_") + 1, file_name.find(".root") - file_name.find_last_of("_") - 1));
        files_.push_back(DataFile(data_dir_name, dataset_name, event_num));
      }
    }
  }

  files_num_ = files_.size();

  G4cout << "Found " << files_num_ << " files" << G4endl;
}

G4int InputManager::GetFilesNumber() {
  G4AutoLock lock(&mutex_);
  return files_num_;
}

std::vector<DataFile> InputManager::GetNextFiles(const G4int files_num) {
  G4AutoLock lock(&mutex_);
  G4int start = current_file_id;
  G4int end = (start + files_num >= files_num_) ? files_num_ : start + files_num;

  std::vector<DataFile> files;
  for (G4int i = start; i < end; ++i) {
    files.push_back(GetNextFile());
  }

  current_file_id = (end == files_num_) ? 0 : end;

  return files;
}

DataFile& InputManager::GetNextFile() {
  G4AutoLock lock(&mutex_);
  if (current_file_id >= files_num_) { current_file_id = 0; }
  return files_[current_file_id++];
}

}  // namespace nevod