#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"

class G4Event;

namespace NEVOD {

class EventAction : public G4UserEventAction {
public:
  EventAction();
  ~EventAction() override;

public:
  void BeginOfEventAction(const G4Event *) override;
  void EndOfEventAction(const G4Event *) override;
};

} // namespace NEVOD

#endif // EVENTACTION_HH
