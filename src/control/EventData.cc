#include "control/EventData.hh"

// Implement ROOT ClassDef macros
ClassImp(nevod::ParticleData);

namespace nevod {

EventData::EventData(): EventData(0, 0, 0, 0, 0) {}
EventData::EventData(ULong_t event_id, ULong_t primary_particle_id, ULong_t particle_amount, Double_t theta, Double_t phi)
    : event_id(event_id), primary_particle_id(primary_particle_id), particle_amount(particle_amount), theta(theta), phi(phi) {
  particles.resize(particle_amount);
  energy_dep = 0;
  particle_count = 0;
  track_length = 0;
  muon_count = 0;
  energy_start = 0;
  energy_end = 0;
  start_time = std::chrono::steady_clock::now();
  duration = 0;

  muon_nevod = std::make_pair(TrackData(), TrackData());
  muon_decor = vector3d<Double_t>();
  muon_decor_w = vector3d<Double_t>();
  edep_count_sct = vector3d<Double_t>();
  amplitude_qsm = vector4d<Double_t>();
}

void EventData::ConnectHeaderTree(TTree* tree) {
  tree->Branch("EventID", &event_id, "eventId/L");
  tree->Branch("PrimaryParticleID", &primary_particle_id, "PrimaryParticleID/L");
  tree->Branch("ParticleAmount", &particle_amount, "ParticleAmount/L");
  // TODO add writing configuration of experiments too
}

void EventData::ConnectEventTree(TTree* tree) {
  tree->Branch("Theta", &theta, "Theta/D");
  tree->Branch("Phi", &phi, "Phi/D");
  tree->Branch("ThetaReconstructed", &theta_rec, "ThetaReconstructed/D");
  tree->Branch("PhiReconstructed", &phi_rec, "PhiReconstructed/D");
  tree->Branch("EnergyDeposition", &energy_dep, "EnergyDeposition/D");
  tree->Branch("ParticleCount", &particle_count, "ParticleCount/L");
  tree->Branch("TrackLength", &track_length, "TrackLength/D");
  tree->Branch("MuonCount", &muon_count, "MuonCount/L");
  tree->Branch("EnergyStart", &energy_start, "EnergyStart/D");
  tree->Branch("EnergyEnd", &energy_end, "EnergyEnd/D");
  tree->Branch("Duration", &duration, "Duration/D");
  tree->Branch("DECOR", &muon_decor);
  tree->Branch("DECORW", &muon_decor_w);
  tree->Branch("SCT", &edep_count_sct);
  tree->Branch("CherenkovWD", &amplitude_qsm);

  for (size_t i = 0; i < particles.size(); i++) {
    tree->Branch("ParticleID", &particles[i].particle_id, "ParticleID/L");
    tree->Branch("ParticleNum", &particles[i].particle_num, "ParticleNum/L");
    tree->Branch("Coordinate", &particles[i].coordinate);
    tree->Branch("Momentum", &particles[i].momentum);
    tree->Branch("Energy", &particles[i].energy, "Energy/D");
  }
}

void EventData::Print() const { operator<<(G4cout); }
std::ostream& EventData::operator<<(std::ostream& os) const {
  os << event_id << '\t' << primary_particle_id << '\t' << particle_amount << '\t' << theta << '\t' << phi << '\t' << theta_rec << '\t' << phi_rec
     << '\t' << energy_dep << '\t' << particle_count << '\t' << track_length << '\t' << muon_count << '\t' << energy_start << '\t' << energy_end
     << '\t' << duration << '\t' << std::endl;
  return os;
}

void EventData::Clear(G4bool clear_header) {
  if (clear_header) {
    event_id = 0;
    primary_particle_id = 0;
    particle_amount = 0;
    theta = 0;
    phi = 0;
  }
  particles.clear();
  theta_rec = 0;
  phi_rec = 0;
  energy_dep = 0;
  particle_count = 0;
  track_length = 0;
  muon_count = 0;
  energy_start = 0;
  energy_end = 0;
  start_time = std::chrono::steady_clock::now();
  duration = 0;
  muon_nevod = std::make_pair(TrackData(), TrackData());
  muon_decor = vector3d<Double_t>();
  muon_decor_w = vector3d<Double_t>();
  edep_count_sct = vector3d<Double_t>();
  photoelectron_num.clear();
  amplitude_qsm = vector4d<Double_t>();
}

}  // namespace nevod