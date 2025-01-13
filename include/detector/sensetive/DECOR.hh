#ifndef DECOR_SENSITIVE_DETECTOR_HH
#define DECOR_SENSITIVE_DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "control/Communicator.hh"
#include "control/EventData.hh"

class G4Step;
namespace nevod {

class DECORSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator;

 public:
  DECORSensetiveDetector(G4String name, Communicator* communicator);
  ~DECORSensetiveDetector();

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};
}  // namespace nevod

#endif  // DECOR_SENSITIVE_DETECTOR_HH