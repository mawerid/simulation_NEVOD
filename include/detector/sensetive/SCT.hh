#ifndef SCT_SENSITIVE_DETECTOR_HH
#define SCT_SENSITIVE_DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "control/Communicator.hh"
#include "control/EventData.hh"

class G4Step;
namespace nevod {

class SCTSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator;

 public:
  SCTSensetiveDetector(G4String name, Communicator* communicator);
  ~SCTSensetiveDetector();

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};
}  // namespace nevod

#endif  // SCT_SENSITIVE_DETECTOR_HH