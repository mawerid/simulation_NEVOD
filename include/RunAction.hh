#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Timer;
class G4Run;

namespace NEVOD {
class RunAction : public G4UserRunAction {
public:
  RunAction();
  ~RunAction() override;

public:
  void BeginOfRunAction(const G4Run *aRun) override;
  void EndOfRunAction(const G4Run *aRun) override;
};

} // namespace NEVOD

#endif // RUNACTION_HH
