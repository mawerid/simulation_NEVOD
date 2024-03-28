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

extern G4long NumPhotEl[600];
extern G4double Kv_Eff[70];

extern G4float NVD_edep, NVD_npe;

extern G4float MuNVD[501][8][2];
extern G4float MuDCR[501][8][8][2];

extern G4float EdepCntSCT[80];

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

  G4float eph;
  G4float edep;

  //  G4String particleName = aStep->GetTrack()
  //                              ->GetDynamicParticle()
  //                              ->GetParticleDefinition()
  //                              ->GetParticleName();
  G4int parentID = track->GetParentID();
  //  G4String processName =
  //      aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

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
          if (MuNVD[MuTrackID][0][0] < 0) {
            MuNVD[MuTrackID][0][0] = nbCtrlNVD;
            MuNVD[MuTrackID][1][0] = track->GetPosition().x();
            MuNVD[MuTrackID][2][0] = track->GetPosition().y();
            MuNVD[MuTrackID][3][0] = track->GetPosition().z();
            MuNVD[MuTrackID][4][0] = track->GetMomentumDirection().x();
            MuNVD[MuTrackID][5][0] = track->GetMomentumDirection().y();
            MuNVD[MuTrackID][6][0] = track->GetMomentumDirection().z();
            MuNVD[MuTrackID][7][0] =
                aStep->GetPreStepPoint()->GetKineticEnergy();
          }
          if (MuNVD[MuTrackID][0][0] >= 0 &&
              nbCtrlNVD != MuNVD[MuTrackID][0][0]) {
            MuNVD[MuTrackID][0][1] = nbCtrlNVD;
            MuNVD[MuTrackID][1][1] = track->GetPosition().x();
            MuNVD[MuTrackID][2][1] = track->GetPosition().y();
            MuNVD[MuTrackID][3][1] = track->GetPosition().z();
            MuNVD[MuTrackID][4][1] = track->GetMomentumDirection().x();
            MuNVD[MuTrackID][5][1] = track->GetMomentumDirection().y();
            MuNVD[MuTrackID][6][1] = track->GetMomentumDirection().z();
            MuNVD[MuTrackID][7][1] =
                aStep->GetPreStepPoint()->GetKineticEnergy();
          }
        } // else {
        //          G4cout << "!!! nbCtrlNVD is out of range ???" << '\t' <<
        //          nbCtrlNVD
        //                 << G4endl;
        //        }
      } // CtrlNVD

      // new 30.03.2020
      if (Vname == "SM0x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][0][nbPlaneDCR][0] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM1x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][1][nbPlaneDCR][0] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM2x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][2][nbPlaneDCR][0] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM3x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][3][nbPlaneDCR][0] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM4x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][4][nbPlaneDCR][0] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM5x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][5][nbPlaneDCR][0] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM6x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][6][nbPlaneDCR][0] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM7x") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][7][nbPlaneDCR][0] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }

      if (Vname == "SM0y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][0][nbPlaneDCR][1] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM1y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][1][nbPlaneDCR][1] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM2y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][2][nbPlaneDCR][1] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM3y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][3][nbPlaneDCR][1] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM4y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][4][nbPlaneDCR][1] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM5y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][5][nbPlaneDCR][1] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM6y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][6][nbPlaneDCR][1] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }
      if (Vname == "SM7y") {
        nbPlaneDCR = track->GetVolume()->GetCopyNo();
        if (nbPlaneDCR >= 0 && nbPlaneDCR < 8)
          MuDCR[MuTrackID][7][nbPlaneDCR][1] = 1;
        //        else
        //          G4cout << "!!! nbPlaneDCR is out of range ???" << '\t' <<
        //          nbPlaneDCR
        //                 << G4endl;
      }

    } // MuTrackID

  } // muon
  // bundles

  //  if (particleType != G4OpticalPhoton::OpticalPhotonDefinition()) {
  if (Vname == "WaterBox") {
    edep = aStep->GetTotalEnergyDeposit();

    NVD_edep = NVD_edep + edep / MeV;
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
            NumPhotEl[nc]++;
            NVD_npe = NVD_npe + 1.;
          } // proverka nkopy
            //          else
            //            G4cout << "!!! nPMT is out of range ???" << '\t' << nc
            //            << G4endl;

        } // rozigr phot-effecta
      }   // proverka diapazona energii
    }     // popal v photokatod

    if (Vname == "physScint") {
      edep = aStep->GetTotalEnergyDeposit();
      G4int ncnt;
      ncnt = track->GetVolume()->GetCopyNo(); //???
      if (ncnt >= 0 && ncnt < 80)
        EdepCntSCT[ncnt] = EdepCntSCT[ncnt] + edep / MeV;
      //      else
      //        G4cout << "!!! ncnt SCT is out of range ???" << '\t' << ncnt <<
      //        G4endl;
    }
  }
}

} // namespace NEVOD