#ifndef NVDSteppingAction_h
#define NVDSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class NVDSteppingAction : public G4UserSteppingAction {
public:
  NVDSteppingAction();
  virtual ~NVDSteppingAction(){};

  virtual void UserSteppingAction(const G4Step *);

private:
public:
};

#endif
