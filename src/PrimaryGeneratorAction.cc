#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include <fstream>

#include "G4SystemOfUnits.hh"

namespace NEVOD {

extern std::ifstream ExpMuTrack;
extern G4float Teta, Fi;
extern G4float PMDx, PMDy, PMDz, PE, PPx, PPy, PPz, PD;

PrimaryGeneratorAction::PrimaryGeneratorAction() {
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);

  // default kinematic
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *particle = particleTable->FindParticle("mu-");
  particleGun->SetParticleDefinition(particle);

  particleGun->SetParticlePosition(
      G4ThreeVector(-100.0 * cm, -1000.0 * cm, 100.0 * cm));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete particleGun; }

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *particle;

  float NRUN;
  float NEVENT;

  G4float SM1x, SM1y, SM1z, SM2x, SM2y, SM2z;
  G4float smeSMx, smeSMy, smeSMz;
  G4float VTx, VTy, VTz;

  float P;
  G4float TetaMu, FiMu;

  smeSMx = 4.5;
  smeSMy = 13.;
  smeSMz = 4.5 - 0.3; // pravilno

  // horizontal mu
  ExpMuTrack >> NRUN;
  ExpMuTrack >> NEVENT;
  ExpMuTrack >> SM1x;
  ExpMuTrack >> SM1y;
  ExpMuTrack >> SM1z;
  ExpMuTrack >> SM2x;
  ExpMuTrack >> SM2y;
  ExpMuTrack >> SM2z;

  SM1x = SM1x - smeSMx;
  SM1y = SM1y - smeSMy;
  SM1z = SM1z - smeSMz;
  SM2x = SM2x - smeSMx;
  SM2y = SM2y - smeSMy;
  SM2z = SM2z - smeSMz;

  VTx = (SM2x - SM1x) /
        sqrt((SM2x - SM1x) * (SM2x - SM1x) + (SM2y - SM1y) * (SM2y - SM1y) +
             (SM2z - SM1z) * (SM2z - SM1z));
  VTy = (SM2y - SM1y) /
        sqrt((SM2x - SM1x) * (SM2x - SM1x) + (SM2y - SM1y) * (SM2y - SM1y) +
             (SM2z - SM1z) * (SM2z - SM1z));
  VTz = (SM2z - SM1z) /
        sqrt((SM2x - SM1x) * (SM2x - SM1x) + (SM2y - SM1y) * (SM2y - SM1y) +
             (SM2z - SM1z) * (SM2z - SM1z));

  PMDx = VTx; //
  PMDy = VTy;
  PMDz = -VTz; //???
  PE = 101.;
  PPx = SM1x;
  PPy = SM1y;
  PPz = SM1z;
  PD = 5;

  P = sqrt(PMDx * PMDx + PMDy * PMDy + PMDz * PMDz);
  if (P != 0.)
    TetaMu = std::acos(PMDz / P) * 180. / M_PI;
  else
    TetaMu = 0.;
  if (PMDx != 0.)
    FiMu = atan(PMDy / PMDx) * 180. / M_PI;
  else {
    if (PMDy == 0.)
      FiMu = 0.;
    if (PMDy > 0.)
      FiMu = 90.;
    if (PMDy < 0.)
      FiMu = 270.;
  }
  if (PMDy == 0.) {
    if (PMDx > 0.)
      FiMu = 0.;
    if (PMDx < 0.)
      FiMu = 180.;
  }
  if (FiMu > 0.) {
    if (PMDx > 0. && PMDy > 0.)
      FiMu = 0. + FiMu;
    if (PMDx < 0. && PMDy < 0.)
      FiMu = 180. + FiMu;
  }
  if (FiMu < 0.) {
    if (PMDx > 0. && PMDy < 0.)
      FiMu = 360. + FiMu;
    if (PMDx < 0. && PMDy > 0.)
      FiMu = 180. + FiMu;
  }
  if (FiMu >= 0. && FiMu < 180.)
    FiMu = FiMu + 180.; // experiment DECOR, muon bundles
  else
    FiMu = FiMu - 180.; // experiment DECOR, muon bundles

  Teta = TetaMu;
  Fi = FiMu;

  if (PD == 5)
    particle = particleTable->FindParticle("mu+");
  if (PD == 6)
    particle = particleTable->FindParticle("mu-");

  particleGun->SetParticleDefinition(particle);

  particleGun->SetParticleMomentumDirection(
      G4ThreeVector(PMDx, PMDy, -1. * PMDz));
  particleGun->SetParticleEnergy(PE * GeV);
  particleGun->SetParticlePosition(G4ThreeVector(PPx * m, PPy * m, PPz * m));

  particleGun->GeneratePrimaryVertex(anEvent);
}
// EAS 1p

} // namespace NEVOD