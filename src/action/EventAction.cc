#include "action/EventAction.hh"

namespace nevod {

double normalRandom(double a, double sigma) {
  G4double g1, g2, norm;

  g1 = G4UniformRand();
  g2 = G4UniformRand();
  norm = std::sqrt(-2. * log(g1)) * cos(2. * M_PI * g2);
  norm = a + norm * sigma;

  return norm;
}

EventAction::EventAction(RunAction* run_action, Communicator* communicator): G4UserEventAction(), communicator_(communicator) {
  G4int thread_id = G4Threading::G4GetThreadId();

  std::string output_file_name = communicator_->GetSimulationParams().output_dir_path + "/output_thread_" + std::to_string(thread_id) + ".root";
  output_file_ = new TFile(output_file_name.c_str(), "RECREATE");

  event_data_ = communicator_->GetEventData();

  run_header_tree_ = new TTree("RunHeaderTree", "run level data");
  event_data_->ConnectHeaderTree(run_header_tree_);

  event_tree_ = new TTree("EventTree", "event level data");
  event_tree_->Branch("Epoch", &current_epoch_, "Epoch/I");
  event_data_->ConnectEventTree(event_tree_);
}

EventAction::~EventAction() {
  output_file_->Write();
  output_file_->Close();
  delete output_file_;
}

void EventAction::BeginOfEventAction(const G4Event*) {}

void EventAction::EndOfEventAction(const G4Event*) {
  auto current_time = std::chrono::steady_clock::now();

  if (!header_processed_) {
    run_header_tree_->Fill();
    header_processed_ = true;
  }

  event_data_->duration = (current_time - event_data_->start_time).count();

  if (event_data_->muon_nevod.first.detected_copy_num >= 0 && event_data_->muon_nevod.second.detected_copy_num >= 0) {
    event_data_->muon_count = 1;
    event_data_->track_length = std::hypot(
                                    (event_data_->muon_nevod.second.coordinate.x() - event_data_->muon_nevod.first.coordinate.x()),
                                    (event_data_->muon_nevod.second.coordinate.y() - event_data_->muon_nevod.first.coordinate.y()),
                                    (event_data_->muon_nevod.second.coordinate.z() - event_data_->muon_nevod.first.coordinate.z())) /
                                m;
    event_data_->energy_start = event_data_->muon_nevod.first.energy;
    event_data_->energy_end = event_data_->muon_nevod.second.energy;
  }

  G4double amplitude, q;

  for (size_t i = 0; i < event_data_->photoelectron_num.size(); ++i) {
    auto id = communicator_->GetQSMId(i);

    if (event_data_->photoelectron_num[i] > 0) {
      amplitude = 0;
      for (G4int i = 0; i < event_data_->photoelectron_num[i]; i++) {
        q = -4. * log(1. - G4UniformRand());                                  // G4UniformRand != 1.
        if (event_data_->photoelectron_num[i] > 1) q = normalRandom(q, 2.8);  // delta_1e*A_1e = 0.7*4.0 = 2.8
        amplitude += q;
      }
      if (amplitude < 1.0) amplitude = 1.0;

      event_data_->amplitude_qsm[id.plane][id.stripe][id.module][id.tube] = amplitude;
    }
  }

  current_epoch_ = communicator_->GetCurrentEpoch();

  event_tree_->Fill();
  output_file_->Flush();

  G4cout << event_data_ << G4endl;

  event_data_->Clear();
}

}  // namespace nevod