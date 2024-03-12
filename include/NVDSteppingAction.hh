#ifndef NVDSTEPPINGACTION_HH
#define NVDSTEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class NVDSteppingAction : public G4UserSteppingAction {
public:
  NVDSteppingAction();
  ~NVDSteppingAction() override = default;

  void UserSteppingAction(const G4Step *) override;
};

#endif // NVDSTEPPINGACTION_HH
