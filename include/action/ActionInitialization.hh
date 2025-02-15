#ifndef ACTIONINITIALIZATION_HH
#define ACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"
#include "action/EventAction.hh"
#include "action/PrimaryGeneratorAction.hh"
#include "action/RunAction.hh"
#include "action/SteppingAction.hh"
#include "control/Communicator.hh"
#include "control/InputManager.hh"

namespace nevod {

class ActionInitialization : public G4VUserActionInitialization {
  Communicator* communicator_;
  InputManager* input_manager_;

 public:
  ActionInitialization(Communicator* communicator, InputManager* input_manager);
  virtual ~ActionInitialization() = default;

  virtual void BuildForMaster() const;
  virtual void Build() const;
};

}  // namespace nevod

#endif  // ACTIONINITIALIZATION_HH
