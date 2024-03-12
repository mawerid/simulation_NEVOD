#ifndef NVDEVENTACTION_HH
#define NVDEVENTACTION_HH

#include "G4UserEventAction.hh"

class G4Event;

class NVDEventAction : public G4UserEventAction {
public:
  NVDEventAction();
  ~NVDEventAction() override;

public:
  void BeginOfEventAction(const G4Event *) override;
  void EndOfEventAction(const G4Event *) override;
};

#endif // NVDEVENTACTION_HH
