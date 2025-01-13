#ifndef COMMUNICATOR_HH
#define COMMUNICATOR_HH

#include "control/EventData.hh"
#include "globals.hh"

namespace nevod {

enum ConfigQSM {
  OLD_CONFIGURATION,
  NEW_CONFIGURATION,
  RANDOM_CONFIGURATION
};

struct SimulationParams {
  G4int thread_num{0};
  G4int epoch_num{1000};
  G4int batch_size{};
  G4int initial_offset{0};
  G4String input_path{};
  G4String output_dir_path{};
  G4bool verbose{true};
  ConfigQSM config_qsm{NEW_CONFIGURATION};
  G4bool use_ui{false};
  G4int seed{47212};
  G4bool save_logs{true};
  G4String log_save_dir_path{};

  SimulationParams() = default;

  SimulationParams(G4String config_path);

  ~SimulationParams() = default;
};

class Communicator {
 public:
  Communicator(G4String config_path);

  ~Communicator();

  void PrintStartMessage() const;
  void PrintEndMessage() const;
  void UpdateProgress(G4double increment);
  void SetTotalEventCount(G4int total_event_count);
  void MergeOutputFiles() const;

  SimulationParams GetSimulationParams() const;

 private:
  // progress bar
  G4double current_progress_{0.0};
  G4int total_event_count_{0};

  // simulation parameters
  SimulationParams simulation_params_;
  G4int count_pmt_;
  std::vector<std::vector<G4int>> id_qsm_;

  // current event data (?)
  EventData* event_data_;
};
}  // namespace nevod

#endif  // COMMUNICATOR_HH