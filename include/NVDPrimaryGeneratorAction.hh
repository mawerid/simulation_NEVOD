#ifndef NVDPRIMARYGENERATORACTION_HH
#define NVDPRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class NVDPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  NVDPrimaryGeneratorAction();
  ~NVDPrimaryGeneratorAction() override;

public:
  void GeneratePrimaries(G4Event *) override;

private:
  G4ParticleGun *particleGun;
};

#endif // NVDPRIMARYGENERATORACTION_HH
