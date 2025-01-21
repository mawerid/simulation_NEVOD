#ifndef WATER_SENSITIVE_DETECTOR_HH
#define WATER_SENSITIVE_DETECTOR_HH

#include "G4SystemOfUnits.hh"
#include "G4VSensitiveDetector.hh"
#include "control/Communicator.hh"
#include "control/EventData.hh"

class G4Step;
namespace nevod {

class WaterSensetiveDetector : public G4VSensitiveDetector {
  Communicator* communicator_;

 public:
  WaterSensetiveDetector(G4String name, Communicator* communicator);
  ~WaterSensetiveDetector();

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent*);
};
}  // namespace nevod

#endif  // SCT_SENSITIVE_DETECTOR_HH