#include "detector/sensetive/Water.hh"

namespace nevod {

WaterSensetiveDetector::WaterSensetiveDetector(G4String name, Communicator* communicator): G4VSensitiveDetector(name), communicator_(communicator) {}

G4bool WaterSensetiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history) {
  auto event_data = communicator_->GetEventData();
  event_data->energy_dep += step->GetTotalEnergyDeposit() / MeV;

  return true;
}

void WaterSensetiveDetector::EndOfEvent(G4HCofThisEvent*) {}
}  // namespace nevod
