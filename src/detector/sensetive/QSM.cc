#include "detector/sensetive/QSM.hh"

namespace nevod {

AirtubeSensetiveDetector::AirtubeSensetiveDetector(G4String name, Communicator* communicator)
    : G4VSensitiveDetector(name), communicator_(communicator) {}

G4bool AirtubeSensetiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
  auto particle_type = step->GetTrack()->GetDefinition();

  if (particle_type == G4OpticalPhoton::OpticalPhotonDefinition()) step->GetTrack()->SetTrackStatus(fStopAndKill);
  return true;
}

void AirtubeSensetiveDetector::EndOfEvent(G4HCofThisEvent*) {}

PhotocathodeSensetiveDetector::PhotocathodeSensetiveDetector(G4String name, Communicator* communicator)
    : G4VSensitiveDetector(name), communicator_(communicator) {
  // TODO fix this
  minimum_energy_ = 0;
  maximum_energy_ = 70;
  total_pmt_count_ = communicator_->GetCountPMT();
}

G4bool PhotocathodeSensetiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
  const auto* track = step->GetTrack();
  auto particle_type = track->GetDefinition();

  if (particle_type == G4OpticalPhoton::OpticalPhotonDefinition()) {
    G4double kinetic_energy = step->GetTrack()->GetKineticEnergy();
    step->GetTrack()->SetTrackStatus(fStopAndKill);

    G4int energy = int((kinetic_energy - photon_energy[0]) / energy_step + 0.5);
    // energy range check
    if (energy >= minimum_energy_ && energy < maximum_energy_) {
      // photo effect
      if (quantum_efficiency[energy] > G4UniformRand()) {
        G4int copy_number = track->GetVolume()->GetCopyNo();
        if (copy_number >= 0 && copy_number < total_pmt_count_) {
          auto event_data = communicator_->GetEventData();
          event_data->photoelectron_num[copy_number]++;
          event_data->particle_count++;
        }
      }
    }
  }

  return true;
}

void PhotocathodeSensetiveDetector::EndOfEvent(G4HCofThisEvent*) {}
}  // namespace nevod
