#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "control/Communicator.hh"
#include "control/InputManager.hh"
#include "globals.hh"

namespace nevod {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
  G4ParticleGun* particle_gun_ = nullptr;
  Communicator* communicator_ = nullptr;
  InputManager* input_manager_ = nullptr;
  EventData* event_data_ = nullptr;

  G4String input_path_;
  size_t current_epoch_ = 0;
  size_t epoch_num_ = 0;
  G4bool header_read_ = false;
  G4bool use_ui = false;

  G4double shift_x = 4.5 * m;
  G4double shift_y = 13.0 * m;
  G4double shift_z = (4.5 - 0.3) * m;  // right

 public:
  PrimaryGeneratorAction();

  PrimaryGeneratorAction(Communicator* communicator, InputManager* input_manager);

  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* event);

  const G4ParticleGun* GetParticleGun() const;

  void ReadEvents();
};

}  // namespace nevod

#endif  // PRIMARYGENERATORACTION_HH
