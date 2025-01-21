#ifndef INPUTMANAGER_HH
#define INPUTMANAGER_HH

#include <fstream>
#include <vector>

#include "G4AutoLock.hh"
#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"
#include "TTree.h"
#include "control/Communicator.hh"
#include "globals.hh"

namespace nevod {

struct DataFile {
  G4String data_dir_name;
  G4String dataset_name;
  G4int event_num;

  DataFile(G4String data_dir_name, G4String dataset_name, G4int event_num);
  // : dir_num(dir_num), data_num(data_num), event_num(event_num) {}

  ~DataFile() = default;

  G4String GetFileName();
};

class InputManager {
  G4int file_num_;
  G4int thread_num_;

  G4String path_;
  size_t offset_;

  Communicator* communicator_;
  std::vector<DataFile> files_;

  G4int current_file_id;

 public:
  InputManager(Communicator* communicator, size_t offset = 0);

  ~InputManager();

  void DetectFiles(G4String path);

  G4int GetFilesNumber();

  std::vector<DataFile>& GetNextFiles(G4int files_num);

  DataFile& GetNextFile();
};

}  // namespace nevod

#endif  // INPUTMANAGER_HH