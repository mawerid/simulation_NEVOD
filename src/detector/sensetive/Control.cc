#include "detector/sensetive/Control.hh"

namespace nevod {

NEVODSensetiveDetector::NEVODSensetiveDetector(G4String name, Communicator* communicator): G4VSensitiveDetector(name), communicator_(communicator) {}

G4bool NEVODSensetiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
  const auto* track = step->GetTrack();
  auto track_id = track->GetTrackID();
  auto parent_id = track->GetParentID();
  auto particle_type = track->GetDefinition();

  if (particle_type == G4MuonPlus::MuonPlusDefinition() || particle_type == G4MuonMinus::MuonMinusDefinition()) {
    if (track_id == 1 && parent_id == 0) {
      auto copy_number = track->GetVolume()->GetCopyNo();
      auto event_data = communicator_->GetEventData();

      if (copy_number >= 0 && copy_number < 7) {
        if (event_data->muon_nevod.first.detected_copy_num == -1) {
          event_data->muon_nevod.first.coordinate = track->GetPosition();
          event_data->muon_nevod.first.momentum = track->GetMomentumDirection();
          event_data->muon_nevod.first.energy = step->GetPreStepPoint()->GetKineticEnergy();
          event_data->muon_nevod.first.detected_copy_num = copy_number;
        }
        if (event_data->muon_nevod.first.detected_copy_num > -1 && copy_number != event_data->muon_nevod.first.detected_copy_num) {
          event_data->muon_nevod.second.coordinate = track->GetPosition();
          event_data->muon_nevod.second.momentum = track->GetMomentumDirection();
          event_data->muon_nevod.second.energy = step->GetPreStepPoint()->GetKineticEnergy();
          event_data->muon_nevod.second.detected_copy_num = copy_number;
        }
      }
    }
  }

  return true;
}

void NEVODSensetiveDetector::EndOfEvent(G4HCofThisEvent*) {}
}  // namespace nevod
