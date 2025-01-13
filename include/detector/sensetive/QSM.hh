#ifndef QSM_SENSITIVE_DETECTOR_HH
#define QSM_SENSITIVE_DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "control/Communicator.hh"
#include "control/EventData.hh"

class G4Step;
namespace nevod {

class AirtubeSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator;

 public:
  AirtubeSensetiveDetector(G4String name, Communicator* communicator);
  ~AirtubeSensetiveDetector();

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};

class PhotocathodeSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator;

 public:
  PhotocathodeSensetiveDetector(G4String name, Communicator* communicator);
  ~PhotocathodeSensetiveDetector();

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};
}  // namespace nevod

#endif  // SCT_SENSITIVE_DETECTOR_HH