#ifndef QSM_SENSITIVE_DETECTOR_HH
#define QSM_SENSITIVE_DETECTOR_HH

#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"
#include "G4VSensitiveDetector.hh"
#include "Randomize.hh"
#include "control/Communicator.hh"

class G4Step;
namespace nevod {

class AirtubeSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator_;

 public:
  AirtubeSensetiveDetector(G4String name, Communicator* communicator);
  ~AirtubeSensetiveDetector() = default;

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};

class PhotocathodeSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator_;
  G4double minimum_energy_ = 0;
  G4double maximum_energy_{};
  G4int total_pmt_count_{};

 public:
  PhotocathodeSensetiveDetector(G4String name, Communicator* communicator);
  ~PhotocathodeSensetiveDetector() = default;

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};
}  // namespace nevod

#endif  // SCT_SENSITIVE_DETECTOR_HH