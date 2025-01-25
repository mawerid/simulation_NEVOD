#include "detector/sensetive/DECOR.hh"

namespace nevod {

DECORSensetiveDetector::DECORSensetiveDetector(G4String name, Communicator* communicator): G4VSensitiveDetector(name), communicator_(communicator) {}

G4bool DECORSensetiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
  const auto* track = step->GetTrack();
  auto track_id = track->GetTrackID();
  auto parent_id = track->GetParentID();
  auto particle_type = track->GetDefinition();
  auto volume_name = track->GetVolume()->GetName();

  if (particle_type == G4MuonPlus::MuonPlusDefinition() || particle_type == G4MuonMinus::MuonMinusDefinition()) {
    if (track_id == 1 && parent_id == 0) {
      auto copy_number = track->GetVolume()->GetCopyNo();
      auto event_data = communicator_->GetEventData();

      if (copy_number >= 0 && copy_number < 8) {
        G4int side = -1;
        G4bool set_w = false;
        for (size_t i = 0; i < 8; ++i) {
          if (volume_name == "SuperModule" + std::to_string(i) + "x")
            side = 0;
          else if (volume_name == "SuperModule" + std::to_string(i) + "y")
            side = 1;

          // 195-255 (exp)
          if (event_data->phi >= 15.0 && event_data->phi <= 75.0 && i >= 2) set_w = true;

          // 105-165 (exp)
          if (event_data->phi >= 285.0 && event_data->phi <= 345.0 && i <= 5) set_w = true;

          if (side >= 0) {
            event_data->muon_decor[i][copy_number][side] = 1;
            if (set_w) event_data->muon_decor_w[i][copy_number][side] = 1;
            break;
          }
        }
      }
    }
  }

  return true;
}

void DECORSensetiveDetector::EndOfEvent(G4HCofThisEvent*) {}
}  // namespace nevod
