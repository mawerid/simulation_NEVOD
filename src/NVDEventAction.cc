#include "NVDEventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Trajectory.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

#include <fcntl.h>
#include <iostream>
#include <unistd.h> //xxx

// extern G4long N_event;
// extern G4long NumPhotEl[600];
//// extern G4float TimePhotEl[600];
// extern G4int PerevKM[600][4];
// extern G4int Nfeu;

// float NormDistr(float a, float sigma);

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NVDEventAction::NVDEventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NVDEventAction::~NVDEventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NVDEventAction::BeginOfEventAction(const G4Event *) {
  /*
   extern G4float nRunN, nEvtN;

     for(G4int i=0; i<600; i++)
     {
       NumPhotEl[i] = 0;
  //     TimePhotEl[i] = 0;
     }

  //   G4cout << N_event << '\t' << nRunN << '\t' << nEvtN << G4endl;
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NVDEventAction::EndOfEventAction(const G4Event *) {
  /*
  // bundles
   extern G4float MuNVD[301][7][2];
   extern G4float MuDCR[301][8][8];
   extern G4int MuBundle;
   extern G4float Teta, Fi;

  //###
   G4float MuTrackPlane;
   extern G4float nMuSM[8], nHitSM, nHitSMwat, nMuDCR, nMuDCRwat;

     for(G4int j=0; j<8; j++)
       nMuSM[j] = 0;

     for(G4int j=0; j<8; j++)
       for(G4int i=0; i<301; i++)
       {
         MuTrackPlane = 0;
         for(G4int k=0; k<8; k++)
           MuTrackPlane = MuTrackPlane + MuDCR[i][j][k];
         if(MuTrackPlane >= 5)
           nMuSM[j] = nMuSM[j] + 1;
       }

     nMuDCR = 0;
     nHitSM = 0;
     for(G4int j=0; j<8; j++)
     {
       nMuDCR = nMuDCR + nMuSM[j];
       if(nMuSM[j] > 0) nHitSM = nHitSM + 1;
     }

     nMuDCRwat = 0;
     nHitSMwat = 0;
     if(Fi >= 15 && Fi <= 75) // ???
     {
       for(G4int j=2; j<8; j++)
       {
         nMuDCRwat = nMuDCRwat + nMuSM[j];
         if(nMuSM[j] > 0) nHitSMwat = nHitSMwat + 1;
       }
     }

     if(Fi >= 285 && Fi <= 345) // ???
     {
       for(G4int j=0; j<6; j++)
       {
         nMuDCRwat = nMuDCRwat + nMuSM[j];
         if(nMuSM[j] > 0) nHitSMwat = nHitSMwat + 1;
       }
     }

   extern G4float MuTrackLenNVD, nMuNVD;
     MuTrackLenNVD = 0;
     nMuNVD = 0;
     for(G4int i=0; i<301; i++)
     {
       if(MuNVD[i][0][0] >= 0 && MuNVD[i][0][1] >= 0)
       {
         nMuNVD = nMuNVD + 1;
         MuTrackLenNVD = MuTrackLenNVD +
         sqrt( (MuNVD[i][1][1]-MuNVD[i][1][0])*(MuNVD[i][1][1]-MuNVD[i][1][0]) +
               (MuNVD[i][2][1]-MuNVD[i][2][0])*(MuNVD[i][2][1]-MuNVD[i][2][0]) +
               (MuNVD[i][3][1]-MuNVD[i][3][0])*(MuNVD[i][3][1]-MuNVD[i][3][0])
  );
       }
     }
     MuTrackLenNVD = MuTrackLenNVD/1000.; // m

   extern G4int otklik;
   extern G4float nRunN, nEvtN;
   extern G4float NVD_edep, NVD_totnpe, NVD_edep1;

   G4float TEMP, *temp;
     temp = &TEMP;
  //###

     G4long A, Nfe;
     G4float AmplKSM[7][4][4][6];
  //   G4float TimePhotElKSM[7][4][4][6]={0};
     G4double amp, t_rez, Q, t_sh, gam;
     G4int npl, nstr, nmod, nfeu, nms[7]={4,3,4,3,4,3,4};

     for(npl=0; npl<7; npl++)
     for(nstr=0; nstr<4; nstr++)
     for(nmod=0; nmod<4; nmod++)
     for(nfeu=0; nfeu<6; nfeu++)
     {
       AmplKSM[npl][nstr][nmod][nfeu] = 0;
     }

  //   G4cout << "Nfeu = " << Nfeu << G4endl;

     for(G4int j=0; j<Nfeu; j++)
     {
       npl  = PerevKM[j][0];
       nstr = PerevKM[j][1];
       nmod = PerevKM[j][2];
       nfeu = PerevKM[j][3];
       Nfe = NumPhotEl[j];
       if(Nfe > 0)
  //     if(Nfe > 1) // ??? threshold 2 pe
  //     if(Nfe > 2) // ??? threshold 3 pe
       {
  //       G4cout << j << '\t' << NumPhotEl[j] << G4endl;
         amp=0;
         for(G4int jj=0; jj<Nfe; jj++) // ???
  //       for(G4int jj=1; jj<Nfe; jj++) // ???
  //       for(G4int jj=2; jj<Nfe; jj++) // ???
         {  //Amp(Q)
           Q = -4.*log(1.-G4UniformRand()); // !!! ??? G4UniformRand != 1.
           if(Nfe > 1)
             Q = NormDistr(Q,2.8); // delta_1e*A_1e = 0.7*4.0 = 2.8
           amp += Q;
         }
  //       if(amp < 1.0) amp = 0; // ne srabativaet diskriminator
         if(amp < 1.0) amp = 1; //
         A = int(amp);
  //       if(A > 4095) A = 4095;
  //       if(A < 0) A = 0; // ???
         AmplKSM[npl][nstr][nmod][nfeu] = A;
  //       AmplKSM[npl][nstr][nmod][nfeu] = Nfe; // ###
       } // if(Nfe > 0)

  //     if(j == 0) AmplKSM[npl][nstr][nmod][nfeu] = 3333; // check
  //     if(j == 545) AmplKSM[npl][nstr][nmod][nfeu] = 5555; // check

     } // end cikl po feu

  //   AmplKSM[1][1][1][1] = 7777; // check

  //###
  // write
     TEMP = N_event;
     write(otklik, temp, 4);
     TEMP = nRunN;
     write(otklik, temp, 4);
     TEMP = nEvtN;
     write(otklik, temp, 4);
     TEMP = NVD_edep;
     write(otklik, temp, 4);
     TEMP = NVD_edep1;
     write(otklik, temp, 4);
     TEMP = NVD_totnpe;
     write(otklik, temp, 4);
     TEMP = MuBundle;
     write(otklik, temp, 4);
     TEMP = MuTrackLenNVD;
     write(otklik, temp, 4);
     TEMP = nMuNVD;
     write(otklik, temp, 4);
     TEMP = nMuDCR;
     write(otklik, temp, 4);
     TEMP = nHitSM;
     write(otklik, temp, 4);
     TEMP = nMuDCRwat;
     write(otklik, temp, 4);
     TEMP = nHitSMwat;
     write(otklik, temp, 4);
  //###


  //   for(npl=0; npl<7; npl++)
  //   for(nstr=0; nstr<4; nstr++)
  //   for(nmod=0; nmod<4; nmod++)
  //   for(nfeu=0; nfeu<6; nfeu++)
  //   {
  //     TEMP = AmplKSM[npl][nstr][nmod][nfeu];
  //     write(otklik, temp, 4);
  //   }

     write(otklik, AmplKSM, 7*4*4*6*4);
  //   write(otklik, nMuSM, 8*4);

  //###
     TEMP = -1.;
     write(otklik, temp, 4);
  //###

     N_event++;
  */
}

/*
float NormDistr(float a, float sigma)
{

 G4float g1,g2,norm;

   g1 = G4UniformRand();
   g2 = G4UniformRand();
   norm = sqrt(-2.*log(g1))*cos(2.*M_PI*g2);
   norm = a+norm*sigma;

   return norm;
}
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
