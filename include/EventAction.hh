#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "FileWriter.hh"
#include "G4Timer.hh"
#include "G4UserEventAction.hh"

class G4Event;

namespace NEVOD {

class EventAction : public G4UserEventAction {
public:
  EventAction();
  ~EventAction() override;

  void BeginOfEventAction(const G4Event *) override;
  void EndOfEventAction(const G4Event *) override;

private:
  G4Timer *fTimer = nullptr;
};

} // namespace NEVOD

#endif // EVENTACTION_HH
