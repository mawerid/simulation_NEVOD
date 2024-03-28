#include "ActionInitialization.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

namespace NEVOD {

void ActionInitialization::BuildForMaster() const {
  auto runAction = new RunAction;
  SetUserAction(runAction);
}

void ActionInitialization::Build() const {

  auto generator = new PrimaryGeneratorAction;
  SetUserAction(generator);

  auto runAction = new RunAction;
  SetUserAction(runAction);
}

} // namespace NEVOD
