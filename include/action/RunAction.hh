#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "action/PrimaryGeneratorAction.hh"
#include "control/Communicator.hh"
#include "globals.hh"

class G4Run;

namespace nevod {
class RunAction : public G4UserRunAction {
  PrimaryGeneratorAction* primary_generator_action_;
  Communicator* communicator_;

 public:
  RunAction(Communicator* communicator);
  ~RunAction() override;

  void BeginOfRunAction(const G4Run* run) override;
  void EndOfRunAction(const G4Run* run) override;
};

}  // namespace nevod

#endif  // RUNACTION_HH
