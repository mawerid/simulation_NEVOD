#include "control/Communicator.hh"

namespace nevod {
SimulationParams::SimulationParams(const G4String& config_path) {
  YAML::Node config = YAML::LoadFile(config_path);

  thread_num = config["thread_num"].as<G4int>();
  epoch_num = config["epochs"].as<G4int>();
  batch_size = config["batch_size"].as<G4int>();
  initial_offset = config["initial_offset"].as<G4int>();
  input_path = config["input_file_path"].as<std::string>();
  output_dir_path = config["output_dir_path"].as<std::string>();
  verbose = config["verbose"].as<G4bool>();
  config_qsm = config["use_old_nevod_configs"].as<G4bool>() ? CherenkovConfig::OLD_CONFIGURATION : CherenkovConfig::NEW_CONFIGURATION;
  config_sct = config["use_old_sct_configs"].as<G4bool>() ? SCTConfig::OLD_CONFIGURATION : SCTConfig::NEW_CONFIGURATION;
  use_ui = config["use_ui"].as<G4bool>();
  seed = config["seed"].as<G4int>();
  save_logs = config["save_verbose_output_flag"].as<G4bool>();
  log_save_dir_path = config["save_verbose_output_dir"].as<std::string>();

  if (!fs::exists(input_path)) throw std::runtime_error("Input directory does not exist: " + input_path);

  if (!fs::exists(output_dir_path)) fs::create_directories(output_dir_path);

  if (save_logs && !fs::exists(log_save_dir_path)) fs::create_directories(log_save_dir_path);
}

Communicator::Communicator(const G4String& config_path): current_progress_(0.0) {
  simulation_params_ = SimulationParams(config_path);

  event_data_.resize(simulation_params_.thread_num);

  for (auto& data: event_data_) {
    data = new EventData();
  }

  // TODO Need to initialize the rest of the data

  UpdateProgress(0);
}

Communicator::~Communicator() {
  for (auto& data: event_data_) {
    delete data;
  }
}

void Communicator::PrintStartMessage() const { G4cout << "Here will be really cool start message" << G4endl; }

void Communicator::PrintEndMessage() const { G4cout << "Here will be really cool end message " << G4endl; }

void Communicator::UpdateProgress(const G4double increment) {
  if (increment < 0 || increment >= 1) throw std::runtime_error("Progress increment should be positive and less than 1");

  G4double previous_progress = current_progress_;
  G4double delta = total_event_count_ / 100.0;
  {
    G4AutoLock lock(&mutex_);
    current_progress_ += increment;
    if (current_progress_ > 1) current_progress_ = 1;
  }

  if (current_progress_ - previous_progress > delta) {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size_);

    G4cout << "[";
    G4int position = window_size_.ws_col * current_progress_;

    for (G4int i = 0; i < window_size_.ws_col; ++i) {
      if (i < position) {
        G4cout << "=";
      } else if (i == position) {
        G4cout << ">";
      } else {
        G4cout << " ";
      }
    }

    G4cout << "] " << G4int(current_progress_ * 100.0) << "%\r";
    G4cout.flush();
  }
}

void Communicator::MergeOutputFiles() const {}

void Communicator::SetTotalEventCount(const G4int total_event_count) {
  G4AutoLock lock(&mutex_);
  total_event_count_ = total_event_count;
}

void Communicator::SetCountPMT(const G4int count_pmt) {
  G4AutoLock lock(&mutex_);
  count_pmt_ = count_pmt;
}
void Communicator::SetCountSCT(const G4int count_sct) {
  G4AutoLock lock(&mutex_);
  count_sct_ = count_sct;
}
void Communicator::SetQSMId(const std::vector<PMTId>& id_qsm) {
  G4AutoLock lock(&mutex_);
  id_qsm_ = id_qsm;
}

SimulationParams& Communicator::GetSimulationParams() {
  G4AutoLock lock(&mutex_);
  return simulation_params_;
}

EventData* Communicator::GetEventData() {
  G4int thread_num = G4Threading::G4GetThreadId();
  G4AutoLock lock(&mutex_);
  return event_data_[thread_num];
}

G4int Communicator::GetCountPMT() {
  G4AutoLock lock(&mutex_);
  return count_pmt_;
}

G4int Communicator::GetCountSCT() {
  G4AutoLock lock(&mutex_);
  return count_sct_;
}

PMTId Communicator::GetQSMId(const G4int copy_number) {
  G4AutoLock lock(&mutex_);
  return id_qsm_[copy_number];
}

}  // namespace nevod