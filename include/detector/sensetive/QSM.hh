#ifndef QSM_SENSITIVE_DETECTOR_HH
#define QSM_SENSITIVE_DETECTOR_HH

#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"
#include "G4VSensitiveDetector.hh"
#include "Randomize.hh"
#include "control/Communicator.hh"

class G4Step;
namespace nevod {

// Quantum efficiency of PMT
// TODO check if it's right
// Hamamatsu (interpolation)
G4double quantum_efficiency[70] = {
    0.000543396, 0.00123019, 0.00229057, 0.00419623, 0.00660377, 0.00936604, 0.0127396,  0.0162075,  0.0212226,  0.0266943,  0.0330038, 0.0401057,
    0.0474,      0.0543245,  0.0601962,  0.0655245,  0.0704075,  0.0737321,  0.0770604,  0.0805094,  0.0840453,  0.0873774,  0.0900755, 0.0927698,
    0.0943358,   0.0953509,  0.0961057,  0.0964906,  0.0968792,  0.0971094,  0.0971094,  0.0971094,  0.0971094,  0.0963321,  0.0952491, 0.094166,
    0.093083,    0.0914113,  0.0884377,  0.0854604,  0.0824868,  0.0795132,  0.0764528,  0.0733472,  0.0702377,  0.0671321,  0.0633887, 0.0594792,
    0.055566,    0.0515698,  0.0474906,  0.0434113,  0.0394943,  0.0358075,  0.0324377,  0.0298943,  0.0273547,  0.0248642,  0.0224226, 0.0199849,
    0.0177698,   0.0159019,  0.0144,     0.0131736,  0.0118792,  0.010566,   0.00943396, 0.00846415, 0.00753585, 0.00661132,
};

// Hamamatsu (interpolation, step = 0.05)
G4double photon_energy[70] = {
    1.85 * eV, 1.9 * eV, 1.95 * eV, 2.0 * eV, 2.05 * eV, 2.1 * eV, 2.15 * eV, 2.2 * eV, 2.25 * eV, 2.3 * eV, 2.35 * eV, 2.4 * eV, 2.45 * eV, 2.5 * eV,
    2.55 * eV, 2.6 * eV, 2.65 * eV, 2.7 * eV, 2.75 * eV, 2.8 * eV, 2.85 * eV, 2.9 * eV, 2.95 * eV, 3.0 * eV, 3.05 * eV, 3.1 * eV, 3.15 * eV, 3.2 * eV,
    3.25 * eV, 3.3 * eV, 3.35 * eV, 3.4 * eV, 3.45 * eV, 3.5 * eV, 3.55 * eV, 3.6 * eV, 3.65 * eV, 3.7 * eV, 3.75 * eV, 3.8 * eV, 3.85 * eV, 3.9 * eV,
    3.95 * eV, 4.0 * eV, 4.05 * eV, 4.1 * eV, 4.15 * eV, 4.2 * eV, 4.25 * eV, 4.3 * eV, 4.35 * eV, 4.4 * eV, 4.45 * eV, 4.5 * eV, 4.55 * eV, 4.6 * eV,
    4.65 * eV, 4.7 * eV, 4.75 * eV, 4.8 * eV, 4.85 * eV, 4.9 * eV, 4.95 * eV, 5.0 * eV, 5.05 * eV, 5.1 * eV, 5.15 * eV, 5.2 * eV, 5.25 * eV, 5.3 * eV,
};

G4double energy_step = (photon_energy[69] - photon_energy[0]) / 69.;

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