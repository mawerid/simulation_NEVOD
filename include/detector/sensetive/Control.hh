#ifndef CONTROL_NEVOD_SENSITIVE_DETECTOR_HH
#define CONTROL_NEVOD_SENSITIVE_DETECTOR_HH

#include "G4ParticleTypes.hh"
#include "G4VSensitiveDetector.hh"
#include "control/Communicator.hh"
#include "control/EventData.hh"

class G4Step;
namespace nevod {

class NEVODSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator_;

 public:
  NEVODSensetiveDetector(G4String name, Communicator* communicator);
  ~NEVODSensetiveDetector() = default;

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};
}  // namespace nevod

#endif  // CONTROL_NEVOD_SENSITIVE_DETECTOR_HH