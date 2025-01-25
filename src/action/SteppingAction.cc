#include "action/SteppingAction.hh"

namespace nevod {

SteppingAction::SteppingAction(EventAction* event_action, Communicator* communicator)
    : G4UserSteppingAction(), communicator_(communicator), step_count_(0) {
  max_step_count_ = communicator_->GetMaxStepCount();
}

void SteppingAction::UserSteppingAction(const G4Step* step) {
  step_count_++;
  if (step_count_ >= max_step_count_) {
    step_count_ = 0;
    auto end = std::chrono::steady_clock::now();
    auto start = communicator_->GetEventStartTime();
    auto sim_time = std::chrono::duration_cast<std::chrono::minutes>(end - start);
    if (sim_time.count() >= 1.0) {
      G4cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
             << "\n  EXCEEDED TIME LIMIT! ABORTING EVENT SIMULATION  \n"
             << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
             << G4endl;
      G4UImanager* ui_manager = G4UImanager::GetUIpointer();
      ui_manager->ApplyCommand("/event/abort");
    }
  }
}

}  // namespace nevod