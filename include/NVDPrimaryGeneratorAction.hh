#ifndef NVDPrimaryGeneratorAction_h
#define NVDPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class NVDPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  NVDPrimaryGeneratorAction();
  ~NVDPrimaryGeneratorAction();

public:
  void GeneratePrimaries(G4Event *);

private:
  G4ParticleGun *particleGun;
};

#endif /*NVDPrimaryGeneratorAction_h*/
