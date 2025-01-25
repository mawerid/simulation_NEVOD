#include "detector/sensetive/SCT.hh"

namespace nevod {

SCTSensetiveDetector::SCTSensetiveDetector(G4String name, Communicator* communicator): G4VSensitiveDetector(name), communicator_(communicator) {
  total_sct_count_ = communicator_->GetCountSCT();
}

G4bool SCTSensetiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
  const auto* track = step->GetTrack();
  auto track_id = track->GetTrackID();
  auto parent_id = track->GetParentID();
  auto particle_type = track->GetDefinition();
  auto volume_name = track->GetVolume()->GetName();

  if (particle_type == G4MuonPlus::MuonPlusDefinition() || particle_type == G4MuonMinus::MuonMinusDefinition()) {
    if (track_id == 1 && parent_id == 0) {
      auto copy_number = track->GetVolume()->GetCopyNo();
      auto event_data = communicator_->GetEventData();
      CounterId id = communicator_->GetCounterId(copy_number);

      if (copy_number >= 0 && copy_number < total_sct_count_)
        event_data->edep_count_sct[id.side][id.plane][id.row] += step->GetTotalEnergyDeposit() / MeV;
    }
  }

  return true;
}

void SCTSensetiveDetector::EndOfEvent(G4HCofThisEvent*) {}
}  // namespace nevod
