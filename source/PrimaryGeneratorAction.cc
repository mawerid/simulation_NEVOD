#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"

#include "G4SystemOfUnits.hh"

#include "G4AutoLock.hh"

namespace {
G4Mutex mutex = G4MUTEX_INITIALIZER;
}

namespace NEVOD {

extern G4long runNum;
extern G4long eventNum;

extern G4double theta, phi;

FileReader *PrimaryGeneratorAction::fileReader = nullptr;

PrimaryGeneratorAction::PrimaryGeneratorAction(G4String &fileName,
                                               size_t shift) {
  {
    G4AutoLock lock(&mutex);
    if (!fileReader)
      fileReader = new FileReader(fileName, shift);
  }

  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);

  // default kinematic
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *particle = particleTable->FindParticle("mu-");
  particleGun->SetParticleDefinition(particle);

  particleGun->SetParticlePosition(
      G4ThreeVector(-100.0 * cm, -1000.0 * cm, 100.0 * cm));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  delete particleGun;
  {
    G4AutoLock lock(&mutex);
    if (fileReader) {
      delete fileReader;
      fileReader = nullptr;
    }
  }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *particle;

  EventInit event;
  if (fileReader) {
    G4AutoLock lock(&mutex);
    event = fileReader->getEvent();
  }

  G4double shiftX, shiftY, shiftZ;
  G4double directionX, directionY, directionZ;
  G4double track_length;

  shiftX = 4.5;
  shiftY = 13.;
  shiftZ = 4.5 - 0.3; // pravilno

  runNum = event.runNum;
  eventNum = event.eventNum;

  event.startX -= shiftX;
  event.startY -= shiftY;
  event.startZ -= shiftZ;
  event.endX -= shiftX;
  event.endY -= shiftY;
  event.endZ -= shiftZ;

  track_length =
      std::hypot((event.endX - event.startX), (event.endY - event.startY),
                 (event.endZ - event.startZ));

  directionX = (event.endX - event.startX) / track_length;
  directionY = (event.endY - event.startY) / track_length;
  directionZ = (event.endZ - event.startZ) / track_length;

  // -1 * z because angles are the angles of origin, not direction
  theta = std::acos(-1 * directionZ) * 180. / M_PI;

  phi = std::atan2(directionY, directionX) * 180. / M_PI;

  if (phi < 0.)
    phi += 360.;

#ifdef G4VIS_USE
  particle = particleTable->FindParticle("geantino");
#else
  if (event.particle_num == 5)
    particle = particleTable->FindParticle("mu+");
  if (event.particle_num == 6)
    particle = particleTable->FindParticle("mu-");
#endif

  particleGun->SetParticleDefinition(particle);

  particleGun->SetParticleMomentumDirection(
      G4ThreeVector(directionX, directionY, directionZ));
  particleGun->SetParticleEnergy(event.energy * GeV);
  particleGun->SetParticlePosition(
      G4ThreeVector(event.startX * m, event.startY * m, event.startZ * m));

  particleGun->GeneratePrimaryVertex(anEvent);
}

} // namespace NEVOD