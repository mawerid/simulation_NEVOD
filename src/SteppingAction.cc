#include "SteppingAction.hh"

#include "G4Colour.hh"
#include "G4Polyline.hh"
#include "G4SteppingManager.hh"
#include "G4VisAttributes.hh"

#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4Step.hh"
#include "Randomize.hh"
#include "globals.hh"

#include "G4SystemOfUnits.hh"

namespace NEVOD {

extern G4long photoelNum[546];

extern G4double energyDepNEVOD;
extern G4long particleCountNEVOD;

extern G4double muonNEVOD[501][8][2];
extern G4double muonDECOR[501][8][8][2];

extern G4double edepCountSCT[80];

// Quantum efficiency of PMT
// Hamamatsu (interpolation)
G4double Kv_Eff[70] = {
    0.000543396, 0.00123019, 0.00229057, 0.00419623, 0.00660377, 0.00936604,
    0.0127396,   0.0162075,  0.0212226,  0.0266943,  0.0330038,  0.0401057,
    0.0474,      0.0543245,  0.0601962,  0.0655245,  0.0704075,  0.0737321,
    0.0770604,   0.0805094,  0.0840453,  0.0873774,  0.0900755,  0.0927698,
    0.0943358,   0.0953509,  0.0961057,  0.0964906,  0.0968792,  0.0971094,
    0.0971094,   0.0971094,  0.0971094,  0.0963321,  0.0952491,  0.094166,
    0.093083,    0.0914113,  0.0884377,  0.0854604,  0.0824868,  0.0795132,
    0.0764528,   0.0733472,  0.0702377,  0.0671321,  0.0633887,  0.0594792,
    0.055566,    0.0515698,  0.0474906,  0.0434113,  0.0394943,  0.0358075,
    0.0324377,   0.0298943,  0.0273547,  0.0248642,  0.0224226,  0.0199849,
    0.0177698,   0.0159019,  0.0144,     0.0131736,  0.0118792,  0.010566,
    0.00943396,  0.00846415, 0.00753585, 0.00661132};

// Hamamatsu (interpolation, step = 0.05)
G4double EnPhot[70] = {
    1.85 * eV, 1.9 * eV,  1.95 * eV, 2.0 * eV,  2.05 * eV, 2.1 * eV,  2.15 * eV,
    2.2 * eV,  2.25 * eV, 2.3 * eV,  2.35 * eV, 2.4 * eV,  2.45 * eV, 2.5 * eV,
    2.55 * eV, 2.6 * eV,  2.65 * eV, 2.7 * eV,  2.75 * eV, 2.8 * eV,  2.85 * eV,
    2.9 * eV,  2.95 * eV, 3.0 * eV,  3.05 * eV, 3.1 * eV,  3.15 * eV, 3.2 * eV,
    3.25 * eV, 3.3 * eV,  3.35 * eV, 3.4 * eV,  3.45 * eV, 3.5 * eV,  3.55 * eV,
    3.6 * eV,  3.65 * eV, 3.7 * eV,  3.75 * eV, 3.8 * eV,  3.85 * eV, 3.9 * eV,
    3.95 * eV, 4.0 * eV,  4.05 * eV, 4.1 * eV,  4.15 * eV, 4.2 * eV,  4.25 * eV,
    4.3 * eV,  4.35 * eV, 4.4 * eV,  4.45 * eV, 4.5 * eV,  4.55 * eV, 4.6 * eV,
    4.65 * eV, 4.7 * eV,  4.75 * eV, 4.8 * eV,  4.85 * eV, 4.9 * eV,  4.95 * eV,
    5.0 * eV,  5.05 * eV, 5.1 * eV,  5.15 * eV, 5.2 * eV,  5.25 * eV, 5.3 * eV};

G4double sh = (EnPhot[69] - EnPhot[0]) / 69.;

SteppingAction::SteppingAction() = default;

void SteppingAction::UserSteppingAction(const G4Step *aStep) {
  const G4Track *track = aStep->GetTrack();
  G4String Vname = track->GetVolume()->GetName();
  G4ParticleDefinition *particleType = track->GetDefinition();
  G4double rand;
  G4int nc, mmm;

  G4double eph;
  G4double edep;

  G4int parentID = track->GetParentID();
  // bundles
  G4int MuTrackID = track->GetTrackID();
  G4int nbCtrlNVD, nbPlaneDCR;

  if (particleType == G4MuonPlus::MuonPlusDefinition() ||
      particleType == G4MuonMinus::MuonMinusDefinition()) {
    if (MuTrackID == 1 && parentID == 0) // EAS
    {
      if (Vname == "CtrlNVD") {
        nbCtrlNVD = track->GetVolume()->GetCopyNo();
        if (nbCtrlNVD >= 0 && nbCtrlNVD < 7) {
          if (muonNEVOD[MuTrackID][0][0] < 0) {
            muonNEVOD[MuTrackID][0][0] = nbCtrlNVD;
            muonNEVOD[MuTrackID][1][0] = track->GetPosition().x();
            muonNEVOD[MuTrackID][2][0] = track->GetPosition().y();
            muonNEVOD[MuTrackID][3][0] = track->GetPosition().z();
            muonNEVOD[MuTrackID][4][0] = track->GetMomentumDirection().x();
            muonNEVOD[MuTrackID][5][0] = track->GetMomentumDirection().y();
            muonNEVOD[MuTrackID][6][0] = track->GetMomentumDirection().z();
            muonNEVOD[MuTrackID][7][0] =
                aStep->GetPreStepPoint()->GetKineticEnergy();
          }
          if (muonNEVOD[MuTrackID][0][0] >= 0 &&
              nbCtrlNVD != muonNEVOD[MuTrackID][0][0]) {
            muonNEVOD[MuTrackID][0][1] = nbCtrlNVD;
            muonNEVOD[MuTrackID][1][1] = track->GetPosition().x();
            muonNEVOD[MuTrackID][2][1] = track->GetPosition().y();
            muonNEVOD[MuTrackID][3][1] = track->GetPosition().z();
            muonNEVOD[MuTrackID][4][1] = track->GetMomentumDirection().x();
            muonNEVOD[MuTrackID][5][1] = track->GetMomentumDirection().y();
            muonNEVOD[MuTrackID][6][1] = track->GetMomentumDirection().z();
            muonNEVOD[MuTrackID][7][1] =
                aStep->GetPreStepPoint()->GetKineticEnergy();
          }
        }
      } // CtrlNVD

      if (Vname == "physCount") {
        edep = aStep->GetTotalEnergyDeposit();
        G4int ncnt;
        ncnt = track->GetVolume()->GetCopyNo(); //???
        if (ncnt >= 0 && ncnt < 80)
          edepCountSCT[ncnt] += edep / MeV;
      }

      // new 30.03.2020
      if (Vname == "SM0x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][0][nbPlaneDCR][0] = 1;
      }
      if (Vname == "SM1x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][1][nbPlaneDCR][0] = 1;
      }
      if (Vname == "SM2x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][2][nbPlaneDCR][0] = 1;
      }
      if (Vname == "SM3x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][3][nbPlaneDCR][0] = 1;
      }
      if (Vname == "SM4x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][4][nbPlaneDCR][0] = 1;
      }
      if (Vname == "SM5x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][5][nbPlaneDCR][0] = 1;
      }
      if (Vname == "SM6x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][6][nbPlaneDCR][0] = 1;
      }
      if (Vname == "SM7x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][7][nbPlaneDCR][0] = 1;
      }

      if (Vname == "SM0y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][0][nbPlaneDCR][1] = 1;
      }
      if (Vname == "SM1y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][1][nbPlaneDCR][1] = 1;
      }
      if (Vname == "SM2y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][2][nbPlaneDCR][1] = 1;
      }
      if (Vname == "SM3y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][3][nbPlaneDCR][1] = 1;
      }
      if (Vname == "SM4y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][4][nbPlaneDCR][1] = 1;
      }
      if (Vname == "SM5y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][5][nbPlaneDCR][1] = 1;
      }
      if (Vname == "SM6y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][6][nbPlaneDCR][1] = 1;
      }
      if (Vname == "SM7y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          muonDECOR[MuTrackID][7][nbPlaneDCR][1] = 1;
      }

    } // MuTrackID

  } // muon
  // bundles

  //  if (particleType != G4OpticalPhoton::OpticalPhotonDefinition()) {
  if (Vname == "WaterBox") {
    edep = aStep->GetTotalEnergyDeposit();

    energyDepNEVOD += edep / MeV;
  }
  //  }

  if (particleType == G4OpticalPhoton::OpticalPhotonDefinition()) {

    if (Vname == "AirTube") {

      aStep->GetTrack()->SetTrackStatus(fStopAndKill); // alt
    }

    if (Vname == "Photokat") {
      eph = aStep->GetTrack()->GetKineticEnergy();

      aStep->GetTrack()->SetTrackStatus(fStopAndKill); // alt

      rand = G4UniformRand();

      mmm = int((eph - EnPhot[0]) / sh + 0.5);
      if (mmm >= 0 && mmm < 70) {
        if (Kv_Eff[mmm] > rand) {
          nc = track->GetVolume()->GetCopyNo();
          if (nc >= 0 && nc < 600) {
            photoelNum[nc]++;
            particleCountNEVOD++;
          } // proverka nkopy

        } // rozigr phot-effecta
      }   // proverka diapazona energii
    }     // popal v photokatod


  }
}

} // namespace NEVOD