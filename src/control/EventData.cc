#include "control/EventData.hh"

namespace nevod {

void Particles::push_back(const ParticleData particle) {
  particle_id.push_back(particle.particle_id);
  particle_num.push_back(particle.particle_num);
  coordinate.push_back(particle.coordinate);
  momentum.push_back(particle.momentum);
  energy.push_back(particle.energy);
}

void Particles::push_back(
    const ULong_t particle_id, const ULong_t particle_num, const TVector3 coordinate, const TVector3 momentum, const Double_t energy) {
  ParticleData particle;
  particle.particle_id = particle_id;
  particle.particle_num = particle_num;
  particle.coordinate = coordinate;
  particle.momentum = momentum;
  particle.energy = energy;
  push_back(particle);
}

void Particles::resize(const size_t size) {
  particle_id.resize(size);
  particle_num.resize(size);
  coordinate.resize(size);
  momentum.resize(size);
  energy.resize(size);
}

void Particles::clear() {
  particle_id.clear();
  particle_num.clear();
  coordinate.clear();
  momentum.clear();
  energy.clear();
}

ParticleData Particles::operator[](const size_t index) const {
  ParticleData particle;
  particle.particle_id = particle_id[index];
  particle.particle_num = particle_num[index];
  particle.coordinate = coordinate[index];
  particle.momentum = momentum[index];
  particle.energy = energy[index];
  return particle;
}

size_t Particles::size() const { return particle_id.size(); }

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

  tree->Branch("ParticleID", &particles.particle_id);
  tree->Branch("ParticleNum", &particles.particle_num);
  tree->Branch("Coordinate", &particles.coordinate);
  tree->Branch("Momentum", &particles.momentum);
  tree->Branch("Energy", &particles.energy);
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
  muon_decor = init_vector3d<Double_t>(0, 0, 0, 0);
  muon_decor_w = init_vector3d<Double_t>(0, 0, 0, 0);
  edep_count_sct = init_vector3d<Double_t>(0, 0, 0, 0);
  photoelectron_num.clear();
  amplitude_qsm = init_vector4d<Double_t>(0, 0, 0, 0, 0);
}

}  // namespace nevod