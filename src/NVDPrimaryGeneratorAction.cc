#include "NVDPrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "Randomize.hh"

#include "G4SystemOfUnits.hh" //xxx

extern G4float SM1x, SM1y, SM1z, SM2x, SM2y, SM2z, dt06, dA13; // DECOR, SCT

NVDPrimaryGeneratorAction::NVDPrimaryGeneratorAction() {
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);

  // default kinematic

  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *particle = particleTable->FindParticle("mu-");
  particleGun->SetParticleDefinition(particle);

  particleGun->SetParticlePosition(
      G4ThreeVector(-100.0 * cm, -1000.0 * cm, 100.0 * cm));
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0., 1., 0.));
  particleGun->SetParticleEnergy(100. * GeV);
}

NVDPrimaryGeneratorAction::~NVDPrimaryGeneratorAction() { delete particleGun; }

// EAS 1p
void NVDPrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {
  extern G4float PMDx, PMDy, PMDz, PE, PPx, PPy, PPz, PD;

  G4float smeSMx, smeSMy, smeSMz; // ???

  smeSMx = 4.5;
  smeSMy = 13.;
  //   smeSMz = 4.5;
  smeSMz = 4.5 - 0.3; // pravilno

  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *particle =
      particleTable->FindParticle("pi+"); // cascade

  if (PD == 1)
    particle = particleTable->FindParticle("gamma");
  if (PD == 2)
    particle = particleTable->FindParticle("e+");
  if (PD == 3)
    particle = particleTable->FindParticle("e-");
  if (PD == 5)
    particle = particleTable->FindParticle("mu+");
  if (PD == 6)
    particle = particleTable->FindParticle("mu-");
  if (PD == 7)
    particle = particleTable->FindParticle("pi0");
  if (PD == 8)
    particle = particleTable->FindParticle("pi+");
  if (PD == 9)
    particle = particleTable->FindParticle("pi-");
  if (PD == 10)
    particle = particleTable->FindParticle("kaon0L");
  if (PD == 11)
    particle = particleTable->FindParticle("kaon+");
  if (PD == 12)
    particle = particleTable->FindParticle("kaon-");
  if (PD == 13)
    particle = particleTable->FindParticle("neutron");
  if (PD == 14)
    particle = particleTable->FindParticle("proton");
  if (PD == 15)
    particle = particleTable->FindParticle("anti_proton");
  if (PD == 16)
    particle = particleTable->FindParticle("kaon0S");
  if (PD == 17)
    particle = particleTable->FindParticle("eta");
  if (PD == 18)
    particle = particleTable->FindParticle("lambda");
  if (PD == 19)
    particle = particleTable->FindParticle("sigma+");
  if (PD == 20)
    particle = particleTable->FindParticle("sigma0");
  if (PD == 21)
    particle = particleTable->FindParticle("sigma-");
  if (PD == 22)
    particle = particleTable->FindParticle("xi0");
  if (PD == 23)
    particle = particleTable->FindParticle("xi-");
  if (PD == 24)
    particle = particleTable->FindParticle("omega-");
  if (PD == 25)
    particle = particleTable->FindParticle("anti_neutron");
  if (PD == 26)
    particle = particleTable->FindParticle("anti_lambda");
  if (PD == 27)
    particle = particleTable->FindParticle("anti_sigma-");
  if (PD == 28)
    particle = particleTable->FindParticle("anti_sigma0");
  if (PD == 29)
    particle = particleTable->FindParticle("anti_sigma+");
  if (PD == 30)
    particle = particleTable->FindParticle("anti_xi0");
  if (PD == 31)
    particle = particleTable->FindParticle("anti_xi-");
  if (PD == 32)
    particle = particleTable->FindParticle("anti_omega-");
  if (PD == 50)
    particle = particleTable->FindParticle("omega");
  if (PD == 51)
    particle = particleTable->FindParticle("rho0");
  if (PD == 52)
    particle = particleTable->FindParticle("rho+");
  if (PD == 53)
    particle = particleTable->FindParticle("rho-");
  if (PD == 54)
    particle = particleTable->FindParticle("delta++");
  if (PD == 55)
    particle = particleTable->FindParticle("delta+");
  if (PD == 56)
    particle = particleTable->FindParticle("delta0");
  if (PD == 57)
    particle = particleTable->FindParticle("delta-");
  if (PD == 58)
    particle = particleTable->FindParticle("anti_delta++");
  if (PD == 59)
    particle = particleTable->FindParticle("anti_delta-");
  if (PD == 60)
    particle = particleTable->FindParticle("anti_delta0");
  if (PD == 61)
    particle = particleTable->FindParticle("anti_delta+");
  if (PD == 66)
    particle = particleTable->FindParticle("nu_e");
  if (PD == 67)
    particle = particleTable->FindParticle("anti_nu_e");
  if (PD == 68)
    particle = particleTable->FindParticle("nu_mu");
  if (PD == 69)
    particle = particleTable->FindParticle("anti_nu_mu");
  if (PD == 116)
    particle = particleTable->FindParticle("D0");
  if (PD == 117)
    particle = particleTable->FindParticle("D+");
  if (PD == 118)
    particle = particleTable->FindParticle("D-");
  if (PD == 119)
    particle = particleTable->FindParticle("anti_D0");
  if (PD == 131)
    particle = particleTable->FindParticle("tau+");
  if (PD == 132)
    particle = particleTable->FindParticle("tau-");
  if (PD == 133)
    particle = particleTable->FindParticle("nu_tau");
  if (PD == 134)
    particle = particleTable->FindParticle("anti_nu_tau");

  if (PD > 200)
    particle = particleTable->FindParticle("proton");

  particleGun->SetParticleDefinition(particle);

  particleGun->SetParticleMomentumDirection(
      G4ThreeVector(PMDx, PMDy, -1. * PMDz));
  particleGun->SetParticleEnergy(PE * GeV);
  particleGun->SetParticlePosition(G4ThreeVector(PPx * m, PPy * m, PPz * m));

  particleGun->GeneratePrimaryVertex(anEvent);
}
// EAS 1p
