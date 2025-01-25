#ifndef SCT_SENSITIVE_DETECTOR_HH
#define SCT_SENSITIVE_DETECTOR_HH

#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"
#include "G4VSensitiveDetector.hh"
#include "control/Communicator.hh"

class G4Step;
namespace nevod {

class SCTSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator_;
  G4int total_sct_count_;

 public:
  SCTSensetiveDetector(G4String name, Communicator* communicator);
  ~SCTSensetiveDetector();

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};
}  // namespace nevod

#endif  // SCT_SENSITIVE_DETECTOR_HH