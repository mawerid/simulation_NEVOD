#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "control/Communicator.hh"
#include "globals.hh"

namespace nevod {

class EventAction;

class SteppingAction : public G4UserSteppingAction {
  Communicator* communicator_;
  G4int step_count_;

 public:
  SteppingAction(EventAction* event_action, Communicator* communicator);
  ~SteppingAction() override = default;

  void UserSteppingAction(const G4Step* step) override;
};

}  // namespace nevod

#endif  // STEPPINGACTION_HH
