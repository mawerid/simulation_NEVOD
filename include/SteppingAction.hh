#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "globals.hh"

namespace NEVOD {

class SteppingAction : public G4UserSteppingAction {
public:
  SteppingAction();
  ~SteppingAction() override = default;

  void UserSteppingAction(const G4Step *) override;
};

} // namespace NEVOD

#endif // STEPPINGACTION_HH
