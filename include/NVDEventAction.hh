#ifndef NVDEventAction_h
#define NVDEventAction_h 1

#include "G4UserEventAction.hh"

class G4Event;

class NVDEventAction : public G4UserEventAction {
public:
  NVDEventAction();
  ~NVDEventAction();

public:
  void BeginOfEventAction(const G4Event *);
  void EndOfEventAction(const G4Event *);
};

#endif
