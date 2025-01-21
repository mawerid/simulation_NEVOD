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

      if (copy_number >= 0 && copy_number < 8)
        for (size_t i = 0; i < 8; ++i) {
          if (volume_name == "SuperModule" + std::to_string(i) + "x") {
            event_data->muon_decor[i][copy_number][0] = 1;
            break;
          } else if (volume_name == "SuperModule" + std::to_string(i) + "y") {
            event_data->muon_decor[i][copy_number][1] = 1;
            break;
          }
        }
    }
  }

  return true;
}

void DECORSensetiveDetector::EndOfEvent(G4HCofThisEvent*) {}
}  // namespace nevod
