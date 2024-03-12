#ifndef NVDRUNACTION_HH
#define NVDRUNACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Timer;
class G4Run;

class NVDRunAction : public G4UserRunAction {
public:
  NVDRunAction();
  ~NVDRunAction() override;

public:
  void BeginOfRunAction(const G4Run *aRun) override;
  void EndOfRunAction(const G4Run *aRun) override;
};

#endif // NVDRUNACTION_HH
