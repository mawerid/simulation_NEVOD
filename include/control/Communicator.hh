#ifndef COMMUNICATOR_HH
#define COMMUNICATOR_HH

#include <sys/ioctl.h>
#include <unistd.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "G4AutoLock.hh"
#include "control/EventData.hh"
#include "globals.hh"

namespace fs = std::filesystem;

namespace nevod {

struct ConstructionFlags {
  G4bool build_nevod_only = false;
  G4bool build_cwd = true;
  G4bool build_sct = true;
  G4bool build_decor = true;
  G4bool build_eas = true;
  G4bool build_prisma = true;
  G4bool build_uran = true;
};

enum struct CherenkovConfig {
  OLD_CONFIGURATION,
  NEW_CONFIGURATION
};

struct PMTId {
  G4int plane;
  G4int stripe;
  G4int module;
  G4int tube;
};

enum struct SCTConfig {
  OLD_CONFIGURATION,
  NEW_CONFIGURATION
};

struct CounterId {
  G4int side;
  G4int plane;
  G4int row;
};

struct SimulationParams {
  G4int thread_num = 0;
  G4int epoch_num = 1000;
  G4int batch_size{};
  G4int initial_offset = 0;
  std::string input_path{};
  std::string output_dir_path{};
  G4bool verbose = true;
  CherenkovConfig config_qsm{CherenkovConfig::NEW_CONFIGURATION};
  SCTConfig config_sct{SCTConfig::NEW_CONFIGURATION};
  ConstructionFlags construction_flags;
  G4bool use_ui = false;
  G4int seed = 47212;
  G4bool save_logs = true;
  std::string log_save_dir_path{};

  SimulationParams() = default;

  SimulationParams(const G4String& config_path);

  ~SimulationParams() = default;
};

class Communicator {
 public:
  Communicator(const G4String& config_path);

  ~Communicator();

  void PrintStartMessage() const;
  void PrintEndMessage() const;
  void UpdateProgress(const G4double increment);
  void MergeOutputFiles() const;

  void SetTotalEventCount(const G4int total_event_count);
  void SetCountPMT(const G4int count_pmt);
  void SetCountSCT(const G4int count_sct);
  void SetQSMId(const std::vector<PMTId>& id_qsm);
  void SetCounterId(const std::vector<CounterId>& id_sct);
  void SetCurrentEpoch(const G4int current_epoch);

  SimulationParams& GetSimulationParams();
  EventData* GetEventData();
  G4int GetCountPMT();
  G4int GetCountSCT();
  G4int GetMaxStepCount();
  G4int GetTotalEpochNum();
  G4int GetCurrentEpoch();
  PMTId GetQSMId(const G4int copy_number);
  CounterId GetCounterId(const G4int copy_number);
  std::chrono::steady_clock::time_point GetEventStartTime();

 private:
  // progress bar
  struct winsize window_size_;
  G4double current_progress_ = 0.0;
  G4int total_event_count_ = 0;
  // TODO add minimum and maximum energy for PMT

  // simulation parameters
  SimulationParams simulation_params_{};
  G4int count_pmt_ = 0;
  G4int count_sct_ = 0;
  std::vector<PMTId> id_qsm_{};
  std::vector<CounterId> id_sct_{};

  // massive of event data (for each thread)
  std::vector<EventData*> event_data_{nullptr};
  std::vector<G4int> current_epoch_{0};
  G4Mutex mutex_ = G4MUTEX_INITIALIZER;

  void ResetEventData();
};
}  // namespace nevod

#endif  // COMMUNICATOR_HH