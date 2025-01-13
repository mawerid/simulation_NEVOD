#include "action/ActionInitialization.hh"

#include "action/EventAction.hh"
#include "action/PrimaryGeneratorAction.hh"
#include "action/RunAction.hh"
#include "action/SteppingAction.hh"

namespace nevod {

ActionInitialization::ActionInitialization(Communicator* communicator, InputManager* input_manager)
    : G4VUserActionInitialization(), communicator_(communicator), input_manager_(input_manager);

void ActionInitialization::BuildForMaster() const {
  auto run_action = new RunAction(communicator_);
  SetUserAction(run_action);
}

void ActionInitialization::Build() const {
  auto generator = new PrimaryGeneratorAction(communicator_, input_manager_);
  SetUserAction(generator);

  auto run_action = new RunAction(communicator_);
  SetUserAction(run_action);

  auto event_action = new EventAction(run_action, communicator_);
  SetUserAction(event_action);

  auto stepping_action = new SteppingAction(event_action, communicator_);
  SetUserAction(stepping_action);
}

}  // namespace nevod
