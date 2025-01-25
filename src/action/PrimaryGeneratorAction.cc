#include "action/PrimaryGeneratorAction.hh"

namespace nevod {

PrimaryGeneratorAction::PrimaryGeneratorAction(Communicator* communicator, InputManager* input_manager)
    : G4VUserPrimaryGeneratorAction(), communicator_(communicator), input_manager_(input_manager) {
  current_epoch_ = 0;
  epoch_num_ = communicator_->GetTotalEpochNum();

  auto params = communicator_->GetSimulationParams();
  input_path_ = params.input_path;

  event_data_ = communicator_->GetEventData();

  // Set number of primary particles generated in single gun
  G4int particle_num = 1;
  particle_gun_ = new G4ParticleGun(particle_num);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete particle_gun_; }

const G4ParticleGun* PrimaryGeneratorAction::GetParticleGun() const { return particle_gun_; }

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  if (current_epoch_ >= epoch_num_) {
    current_epoch_ = 0;
    ReadEvents();
  }

  communicator_->SetCurrentEpoch(current_epoch_);

  // launch the particles
  G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();

  for (auto& particle: event_data_->particles) {
    G4ParticleDefinition* particle_definition = nullptr;
    if (use_ui)
      particle_definition = particle_table->FindParticle("geantino");
    else
      particle_definition = particle_table->FindParticle(particle.particle_id);

    if (particle_definition == nullptr && communicator_->GetSimulationParams().verbose) {
      std::cout << "Didn't find particle \"" << particle.particle_id << "\". Skipping it.\n";
      continue;
    }

    // -1 * z because angles are the angles of origin, not direction
    event_data_->theta_rec = std::acos(-1 * particle.momentum.Z()) * 180. / M_PI;

    event_data_->phi_rec = std::atan2(particle.momentum.Y(), particle.momentum.X()) * 180. / M_PI;

    if (event_data_->phi_rec < 0.0) event_data_->phi_rec += 360.0;

    G4double momentum_abs = std::hypot(particle.momentum.X(), particle.momentum.Y());

    G4ThreeVector start_position =
        G4ThreeVector((particle.coordinate.X() - shift_x), (particle.coordinate.Y() - shift_y), (particle.coordinate.Z() - shift_z));

    G4ThreeVector start_momentum =
        G4ThreeVector(particle.momentum.X() / momentum_abs, particle.momentum.Y() / momentum_abs, particle.momentum.Z() / momentum_abs);

    particle_gun_->SetParticleDefinition(particle_definition);
    particle_gun_->SetParticlePosition(start_position);
    particle_gun_->SetParticleMomentumDirection(start_momentum);
    particle_gun_->SetParticleEnergy(particle.energy);

    particle_gun_->GeneratePrimaryVertex(event);
  }

  G4cout << "Launched event " << event->GetEventID() << G4endl;

  event_data_->start_time = std::chrono::steady_clock::now();

  current_epoch_++;
}

void PrimaryGeneratorAction::ReadEvents() {
  auto file = input_manager_->GetNextFile();

  auto input_file = new TFile((input_path_ + file.GetFileName()).c_str(), "READ");

  auto header_tree = (TTree*)input_file->Get("HeaderTree");
  auto particles_tree = (TTree*)input_file->Get("ParticlesTree");

  event_data_->Clear();

  if (!header_read_) {
    header_tree->SetBranchAddress("EventID", &event_data_->event_id);
    header_tree->SetBranchAddress("PrimaryParticleID", &event_data_->primary_particle_id);
    header_tree->SetBranchAddress("ParticleAmount", &event_data_->particle_amount);
    header_tree->SetBranchAddress("Theta", &event_data_->theta);
    header_tree->SetBranchAddress("Phi", &event_data_->phi);

    header_tree->GetEntry(0);
    header_read_ = true;
  }

  for (Long64_t i = 0; i < particles_tree->GetEntries(); ++i) {
    ParticleData particle;

    particles_tree->SetBranchAddress("ParticleID", &particle.particle_id);
    particles_tree->SetBranchAddress("ParticleNum", &particle.particle_num);
    particles_tree->SetBranchAddress("Coordinate", &particle.coordinate);
    particles_tree->SetBranchAddress("Momentum", &particle.momentum);
    particles_tree->SetBranchAddress("Energy", &particle.energy);

    particles_tree->GetEntry(i);
    event_data_->particles.push_back(particle);
  }

  input_file->Close();
  delete input_file;
}

}  // namespace nevod