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

  if (thread_num == -1) thread_num = G4Threading::G4GetNumberOfCores() - 1;
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

void Communicator::PrintStartMessage() const {
  // TODO Implement start message
  G4cout << "Here will be really cool start message" << G4endl;
}

void Communicator::PrintEndMessage() const {
  // TODO Implement end message
  G4cout << "Here will be really cool end message " << G4endl;
}

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

void Communicator::MergeOutputFiles() const {
  // TODO Implement merging of output files
}

void Communicator::SetTotalEventCount(const G4int total_event_count) {
  G4AutoLock lock(&mutex_);
  total_event_count_ = total_event_count;
}

void Communicator::SetCountPMT(const G4int count_pmt) {
  G4AutoLock lock(&mutex_);
  count_pmt_ = count_pmt;
  ResetEventData();
}
void Communicator::SetCountSCT(const G4int count_sct) {
  G4AutoLock lock(&mutex_);
  count_sct_ = count_sct;
  ResetEventData();
}
void Communicator::SetQSMId(const std::vector<PMTId>& id_qsm) {
  G4AutoLock lock(&mutex_);
  id_qsm_ = id_qsm;
  ResetEventData();
}

void Communicator::SetCounterId(const std::vector<CounterId>& id_sct) {
  G4AutoLock lock(&mutex_);
  id_sct_ = id_sct;
  ResetEventData();
}

void Communicator::SetCurrentEpoch(const G4int current_epoch) {
  G4int thread_num = G4Threading::G4GetThreadId();
  G4AutoLock lock(&mutex_);
  current_epoch_[thread_num] = current_epoch;
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

CounterId Communicator::GetCounterId(const G4int copy_number) {
  G4AutoLock lock(&mutex_);
  return id_sct_[copy_number];
}

std::chrono::steady_clock::time_point Communicator::GetEventStartTime() {
  G4AutoLock lock(&mutex_);
  return event_data_[G4Threading::G4GetThreadId()]->start_time;
}

G4int Communicator::GetMaxStepCount() {
  G4AutoLock lock(&mutex_);
  return simulation_params_.batch_size;
}
G4int Communicator::GetTotalEpochNum() {
  G4AutoLock lock(&mutex_);
  return simulation_params_.epoch_num;
}
G4int Communicator::GetCurrentEpoch() {
  G4int thread_num = G4Threading::G4GetThreadId();
  G4AutoLock lock(&mutex_);
  return current_epoch_[thread_num];
}

void Communicator::ResetEventData() {
  if (count_pmt_ == 0 || count_sct_ == 0 || id_qsm_.empty() || id_sct_.empty()) return;

  PMTId max_qsm = id_qsm_.back();
  max_qsm.plane++;
  max_qsm.stripe++;
  max_qsm.module++;
  max_qsm.tube++;

  CounterId max_sct = id_sct_.back();
  max_sct.side++;
  max_sct.plane++;
  max_sct.row++;

  for (auto& data: event_data_) {
    data->Clear();

    // SCT
    data->edep_count_sct.resize(max_sct.side);
    for (G4int i = 0; i < max_sct.side; ++i) {
      data->edep_count_sct[i].resize(max_sct.plane);
      for (G4int j = 0; j < max_sct.plane; ++j) {
        data->edep_count_sct[i][j].resize(max_sct.row);
        for (G4int k = 0; k < max_sct.row; ++k) {
          data->edep_count_sct[i][j][k] = 0;
        }
      }
    }

    // DECOR
    data->muon_decor.resize(8);
    for (G4int i = 0; i < 8; ++i) {
      data->muon_decor[i].resize(8);
      for (G4int j = 0; j < 8; ++j) {
        data->muon_decor[i][j].resize(2);
        for (G4int k = 0; k < 2; ++k) {
          data->muon_decor[i][j][k] = 0;
        }
      }
    }

    // CherenkovWD NEVOD
    data->photoelectron_num.resize(count_pmt_);
    for (G4int i = 0; i < count_pmt_; ++i) {
      data->photoelectron_num[i] = 0;
    }

    data->amplitude_qsm.resize(max_qsm.plane);
    for (G4int i = 0; i < max_qsm.plane; ++i) {
      data->amplitude_qsm[i].resize(max_qsm.stripe);
      for (G4int j = 0; j < max_qsm.stripe; ++j) {
        data->amplitude_qsm[i][j].resize(max_qsm.module);
        for (G4int k = 0; k < max_qsm.module; ++k) {
          data->amplitude_qsm[i][j][k].resize(max_qsm.tube);
          for (G4int l = 0; l < max_qsm.tube; ++l) {
            data->amplitude_qsm[i][j][k][l] = 0;
          }
        }
      }
    }
  }
}

}  // namespace nevod