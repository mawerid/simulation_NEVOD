#ifndef DECOR_SENSITIVE_DETECTOR_HH
#define DECOR_SENSITIVE_DETECTOR_HH

#include "G4ParticleTypes.hh"
#include "G4VSensitiveDetector.hh"
#include "control/Communicator.hh"
#include "control/EventData.hh"

class G4Step;
namespace nevod {

class DECORSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator_;

 public:
  DECORSensetiveDetector(G4String name, Communicator* communicator);
  ~DECORSensetiveDetector() = default;

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};
}  // namespace nevod

#endif  // DECOR_SENSITIVE_DETECTOR_HH