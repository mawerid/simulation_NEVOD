#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "FileReader.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

namespace NEVOD {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  explicit PrimaryGeneratorAction(G4String &fileName, size_t shift = 0);
  ~PrimaryGeneratorAction() override;

public:
  void GeneratePrimaries(G4Event *anEvent) override;

private:
  static FileReader *fileReader;
  G4ParticleGun *particleGun;
};

} // namespace NEVOD

#endif // PRIMARYGENERATORACTION_HH
