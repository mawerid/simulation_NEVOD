#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

namespace NEVOD {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

public:
  void GeneratePrimaries(G4Event *) override;

private:
  G4ParticleGun *particleGun;
};

} // namespace NEVOD

#endif // PRIMARYGENERATORACTION_HH
