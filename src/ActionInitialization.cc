#include "ActionInitialization.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

namespace NEVOD {

extern G4String input_file;
extern size_t input_shift;

void ActionInitialization::BuildForMaster() const {
  auto runAction = new RunAction;
  SetUserAction(runAction);
}

void ActionInitialization::Build() const {

  auto generator = new PrimaryGeneratorAction(input_file, input_shift);
  SetUserAction(generator);

  auto runAction = new RunAction;
  SetUserAction(runAction);

  auto eventAction = new EventAction;
  SetUserAction(eventAction);

  auto steppingAction = new SteppingAction;
  SetUserAction(steppingAction);
}

} // namespace NEVOD
