#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include <list>

#include "G4ParticleGun.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "control/Communicator.hh"
#include "control/InputManager.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4Box;

namespace nevod {

struct ParticleEntry {
  G4int id;
  Float_t coord_x, coord_y, coord_z;
  Float_t momentum_x, momentum_y, momentum_z;

  ParticleEntry(G4int particle_id, G4double x, G4double y, G4double z, G4double px, G4double py, G4double pz);
  //   : id(particle_id), coord_x(x), coord_y(y), coord_z(z), momentum_x(px), momentum_y(py), momentum_z(pz)
  // {}

  ~ParticleEntry() = default;
};

struct EventEntry {
  G4double theta, phi, energy_0, z_0;
  std::vector<ParticleEntry> particles;

  EventEntry(G4double theta, G4double phi, G4double energy_0, G4double z_0);
  //   : theta(theta), phi(phi), energy_0(energy_0), z_0(z_0)
  // {}

  ~EventEntry() = default;

  void AddParticle(ParticleEntry& p);  //{ particles.push_back(p); };

  EventEntry operator=(const EventEntry& event);
  // {
  //   theta = event.theta;
  //   phi = event.phi;
  //   energy_0 = event.energy_0;
  //   z_0 = event.z_0;
  //   particles = event.particles;
  //   return *this;
  // }

  ParticleEntry operator[](const G4int i);
  // {
  //   if (i >= particles.size()) return particles[0];
  //   return particles[i];
  // }

  G4int size() { return particles.size(); }
};

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
  G4ParticleGun* particle_gun_;
  Communicator* communicator_;
  InputManager* input_manager_;
  std::list<EventEntry> events_;

 public:
  PrimaryGeneratorAction();

  PrimaryGeneratorAction(Communicator* communicator, InputManager* input_manager);

  PrimaryGeneratorAction(Communicator* communicator);

  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* event);

  const G4ParticleGun* GetParticleGun() const;  // { return particle_gun_; }

  void ReadEvents(G4Event* event);
};

}  // namespace nevod

#endif  // PRIMARYGENERATORACTION_HH
