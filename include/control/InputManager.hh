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

  ~DataFile() = default;

  G4String GetFileName();
};

class InputManager {
  G4int files_num_ = 0;
  G4int thread_num_ = 0;
  Communicator* communicator_ = nullptr;
  size_t offset_ = 0;

  std::string path_;

  std::vector<DataFile> files_;

  G4int current_file_id;

  G4Mutex mutex_ = G4MUTEX_INITIALIZER;

 public:
  InputManager(Communicator* communicator, size_t offset = 0);

  ~InputManager() = default;

  void DetectFiles(std::string path);

  G4int GetFilesNumber();

  std::vector<DataFile> GetNextFiles(const G4int files_num);

  DataFile& GetNextFile();
};

}  // namespace nevod

#endif  // INPUTMANAGER_HH