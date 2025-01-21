#include "action/RunAction.hh"

namespace nevod {

RunAction::RunAction(Communicator* communicator): G4UserRunAction(), communicator_(communicator) {}

RunAction::~RunAction() = default;

void RunAction::BeginOfRunAction(const G4Run* run) { G4RunManager::GetRunManager()->SetRandomNumberStore(false); }

void RunAction::EndOfRunAction(const G4Run* run) {
  G4int events_num = run->GetNumberOfEvent();
  if (events_num == 0) return;
  if (IsMaster()) {
    G4cout << "====================== END OF RUN ======================" << G4endl << G4endl;
  } else {
    G4cout << "--------------- End of thread-local run ---------------" << G4endl;
  }
}

}  // namespace nevod