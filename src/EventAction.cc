#include "EventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Trajectory.hh"

#include <unistd.h>

namespace NEVOD {

double NormDistr(double a, double sigma) {

  G4double g1, g2, norm;

  g1 = G4UniformRand();
  g2 = G4UniformRand();
  norm = std::sqrt(-2. * log(g1)) * cos(2. * M_PI * g2);
  norm = a + norm * sigma;

  return norm;
}

extern G4long N_event;
extern G4int otklik;

extern G4long runNum;
extern G4long eventNum;

extern G4long photoelecNum[600];
extern G4int PerevKM[600][4];
extern G4int feuNum;
extern G4float NVD_edep;
extern G4float NVD_npe;

extern G4double theta, phi;
extern G4float MuNVD[501][8][2];
extern G4float MuDCR[501][8][8][2];

extern G4float EdepCntSCT[80];

G4float nMuNVD;
G4float MuTrackLenNVD;
G4float hitSMPLY[8][8];

G4float nTrackSMX[8], nTrackSMY[8], nTrackSM[8], nTrackSMXw[8], nTrackSMYw[8],
    nTrackSMw[8];
G4float hSM[8], hSMw[8], nSM, nSMw;
G4float muSM, muSMw, muDCR, muDCRw;

G4int MuTrackPLX, MuTrackPLY;
G4int hitSMPLYe, hitSMPLYo;

G4long A, Nfe;
G4float AmplKSM[7][4][4][6];
G4double Q;
G4double amp;
G4int npl, nstr, nmod, nfeu;

G4float eMuNVD, eMuNVD1;

G4float EdepCntSCT1[9][5][2];

EventAction::EventAction() { timer = new G4Timer; }

EventAction::~EventAction() { delete timer; }

void EventAction::BeginOfEventAction(const G4Event *) {
  for (G4int ism = 0; ism < 8; ism++) {
    hSM[ism] = 0;
    hSMw[ism] = 0;
    nTrackSMX[ism] = 0;
    nTrackSMY[ism] = 0;
    nTrackSM[ism] = 0;
    nTrackSMXw[ism] = 0;
    nTrackSMYw[ism] = 0;
    nTrackSMw[ism] = 0;
  }

  nSM = 0;
  nSMw = 0;
  muSM = 0;
  muSMw = 0;
  muDCR = 0;
  muDCRw = 0;

  for (auto &i : hitSMPLY)
    for (float &j : i)
      j = 0;

  NVD_edep = 0.;
  NVD_npe = 0.;
  MuTrackLenNVD = 0;
  nMuNVD = 0;
  eMuNVD = 0.;
  eMuNVD1 = 0.;
  for (int i = 1; i < 2; i++)
    for (int j = 0; j < 8; j++)
      for (int k = 0; k < 2; k++)
        MuNVD[i][j][k] = 0.;
  for (int i = 1; i < 2; i++)
    for (int j = 0; j < 8; j++)
      for (int k = 0; k < 8; k++)
        for (int m = 0; m < 2; m++)
          MuDCR[i][j][k][m] = 0.; // new 30.03.2020
  for (int i = 1; i < 2; i++)
    for (int k = 0; k < 2; k++)
      MuNVD[i][0][k] = -1.;

  for (long &i : photoelecNum)
    i = 0;

  for (float &k : EdepCntSCT)
    k = 0.;

  for (auto &i : EdepCntSCT1)
    for (auto &j : i)
      for (float &k : j)
        k = 0.;

  for (npl = 0; npl < 7; npl++)
    for (nstr = 0; nstr < 4; nstr++)
      for (nmod = 0; nmod < 4; nmod++)
        for (nfeu = 0; nfeu < 6; nfeu++) {
          AmplKSM[npl][nstr][nmod][nfeu] = 0;
        }

  timer->Start();
}

void EventAction::EndOfEventAction(const G4Event *) {

  timer->Stop();

  for (G4int ism = 0; ism < 8; ism++) {
    MuTrackPLX = 0;
    for (G4int ipl = 0; ipl < 8; ipl++)
      if (MuDCR[1][ism][ipl][0] > 0)
        MuTrackPLX = MuTrackPLX + 1;
    if (MuTrackPLX >= 5) {
      nTrackSMX[ism] = nTrackSMX[ism] + 1.;
      if (phi >= 15. && phi <= 75.) // 195-255 (exp)
        if (ism >= 2)
          nTrackSMXw[ism] = nTrackSMXw[ism] + 1.;
      if (phi >= 285. && phi <= 345.) // 105-165 (exp)
        if (ism >= 0 && ism <= 5)
          nTrackSMXw[ism] = nTrackSMXw[ism] + 1.;
    }

    MuTrackPLY = 0;
    for (G4int ipl = 0; ipl < 8; ipl++)
      if (MuDCR[1][ism][ipl][1] > 0)
        MuTrackPLY = MuTrackPLY + 1;
    if (MuTrackPLY >= 5) {
      nTrackSMY[ism] = nTrackSMY[ism] + 1.;
      if (phi >= 15. && phi <= 75.) // 195-255 (exp)
        if (ism >= 2)
          nTrackSMYw[ism] = nTrackSMYw[ism] + 1.;
      if (phi >= 285. && phi <= 345.) // 105-165 (exp)
        if (ism >= 0 && ism <= 5)
          nTrackSMYw[ism] = nTrackSMYw[ism] + 1.;
    }

    for (G4int ipl = 0; ipl < 8; ipl++)
      if (MuDCR[1][ism][ipl][1] > 0)
        hitSMPLY[ism][ipl] = hitSMPLY[ism][ipl] + 1.;

    if (MuTrackPLX >= 5 && MuTrackPLY >= 5) {
      nTrackSM[ism] = nTrackSM[ism] + 1.;
      if (phi >= 15. && phi <= 75.) // 195-255 (exp)
        if (ism >= 2)
          nTrackSMw[ism] = nTrackSMw[ism] + 1.;
      if (phi >= 285. && phi <= 345.) // 105-165 (exp)
        if (ism >= 0 && ism <= 5)
          nTrackSMw[ism] = nTrackSMw[ism] + 1.;
    }
  }

  for (G4int i = 1; i < 2; i++) {
    if (MuNVD[i][0][0] >= 0 && MuNVD[i][0][1] >= 0) {
      nMuNVD = nMuNVD + 1;
      MuTrackLenNVD =
          MuTrackLenNVD + sqrt((MuNVD[i][1][1] - MuNVD[i][1][0]) *
                                   (MuNVD[i][1][1] - MuNVD[i][1][0]) +
                               (MuNVD[i][2][1] - MuNVD[i][2][0]) *
                                   (MuNVD[i][2][1] - MuNVD[i][2][0]) +
                               (MuNVD[i][3][1] - MuNVD[i][3][0]) *
                                   (MuNVD[i][3][1] - MuNVD[i][3][0]));
      eMuNVD = eMuNVD + MuNVD[i][7][0];
      eMuNVD1 = eMuNVD1 + MuNVD[i][7][1];
    }
  }

  G4int kcnt;
  // top
  for (int j = 0; j < 5; j++) {
    kcnt = 9 * j;
    for (int i = 0; i < 4; i++) {
      EdepCntSCT1[j * 2][i][1] = EdepCntSCT[kcnt]; // plane, counter, bottom
      kcnt = kcnt + 1;                             // (0)/top(1)
    }
  }
  for (int j = 0; j < 4; j++) {
    kcnt = 4 + 9 * j;
    for (int i = 0; i < 5; i++) {
      EdepCntSCT1[j * 2 + 1][i][1] = EdepCntSCT[kcnt];
      kcnt = kcnt + 1;
    }
  }
  // bottom
  for (int j = 0; j < 5; j++) {
    kcnt = 9 * j + 40;
    for (int i = 0; i < 4; i++) {
      EdepCntSCT1[j * 2][i][0] = EdepCntSCT[kcnt];
      kcnt = kcnt + 1;
    }
  }
  for (int j = 0; j < 4; j++) {
    kcnt = 4 + 9 * j + 40;
    for (int i = 0; i < 5; i++) {
      EdepCntSCT1[j * 2 + 1][i][0] = EdepCntSCT[kcnt];
      kcnt = kcnt + 1;
    }
  }

  for (G4int ism = 0; ism < 8; ism++) {
    hitSMPLYe = 0;
    for (G4int ipl = 0; ipl < 7; ipl = ipl + 2) //
    {
      hitSMPLYe = hitSMPLYe + hitSMPLY[ism][ipl]; // 02.09.20 ok
    }
    hitSMPLYo = 0;
    for (G4int ipl = 1; ipl < 8; ipl = ipl + 2) //
    {
      hitSMPLYo = hitSMPLYo + hitSMPLY[ism][ipl]; // 02.09.20 ok
    }

    if (hitSMPLYe >= 2 && hitSMPLYo >= 2) {
      hSM[ism] = 1;
      if (phi >= 15. && phi <= 75.) // 195-255 (exp)
        if (ism >= 2)
          hSMw[ism] = 1;
      if (phi >= 285. && phi <= 345.) // 105-165 (exp)
        if (ism >= 0 && ism <= 5)
          hSMw[ism] = 1;
    }
  }

  for (G4int ism = 0; ism < 8; ism++) {
    if (hSM[ism] > 0)
      nSM = nSM + 1;
    if (hSMw[ism] > 0)
      nSMw = nSMw + 1;
    if (nTrackSM[ism] > 0)
      muSM = muSM + 1.;
    if (nTrackSMw[ism] > 0)
      muSMw = muSMw + 1.;
    if (nTrackSMX[ism] > 0)
      muDCR = muDCR + nTrackSMX[ism];
    if (nTrackSMXw[ism] > 0)
      muDCRw = muDCRw + nTrackSMXw[ism];
  }

  MuTrackLenNVD = MuTrackLenNVD / 1000.; // m

  for (G4int j = 0; j < feuNum; j++) {
    npl = PerevKM[j][0];
    nstr = PerevKM[j][1];
    nmod = PerevKM[j][2];
    nfeu = PerevKM[j][3];
    Nfe = photoelecNum[j];
    if (Nfe > 0) {
      amp = 0;
      for (G4int i = 0; i < Nfe; i++) {      // Amp(Q)
        Q = -4. * log(1. - G4UniformRand()); // !!! ??? G4UniformRand != 1.
        if (Nfe > 1)
          Q = NormDistr(Q, 2.8); // delta_1e*A_1e = 0.7*4.0 = 2.8
        amp += Q;
      }
      if (amp < 1.0)
        amp = 1;
      A = int(amp);
      AmplKSM[npl][nstr][nmod][nfeu] = A;
    } // if(Nfe > 0)
  }   // end cikl po feu

  //  G4float TEMP1, *temp1;
  //  temp1 = &TEMP1;
  //
  //  //  write
  //  TEMP1 = N_event;
  //  write(otklik, temp1, 4);
  //
  //  TEMP1 = runNum;
  //  write(otklik, temp1, 4);
  //  TEMP1 = eventNum;
  //  write(otklik, temp1, 4);
  //  //           TEMP1 = TETA*180./M_PI;
  //  TEMP1 = theta;
  //  write(otklik, temp1, 4);
  //  //           TEMP1 = FI*180./M_PI;
  //  TEMP1 = phi;
  //  write(otklik, temp1, 4);
  //
  //  TEMP1 = NVD_edep;
  //  write(otklik, temp1, 4);
  //  TEMP1 = NVD_npe;
  //  write(otklik, temp1, 4);
  //  TEMP1 = MuTrackLenNVD;
  //  write(otklik, temp1, 4);
  //  TEMP1 = nMuNVD;
  //  write(otklik, temp1, 4);
  //  TEMP1 = eMuNVD;
  //  write(otklik, temp1, 4);
  //  TEMP1 = eMuNVD1;
  //  write(otklik, temp1, 4);
  //
  //  TEMP1 = muDCR;           // 04.09.20
  //  write(otklik, temp1, 4); // ###
  //  TEMP1 = muSM;            // 04.09.20
  //  write(otklik, temp1, 4); // ###
  //  TEMP1 = nSM;             // ###
  //  write(otklik, temp1, 4); // ###
  //  TEMP1 = muDCRw;          // 04.09.20
  //  write(otklik, temp1, 4); // ###
  //  TEMP1 = muSMw;           // 04.09.20
  //  write(otklik, temp1, 4); // ###
  //  TEMP1 = nSMw;            // ###
  //  write(otklik, temp1, 4); // ###
  //
  //  write(otklik, AmplKSM, 7 * 4 * 4 * 6 * 4);
  //  write(otklik, hSM, 8 * 4);       // ###
  //  write(otklik, nTrackSMX, 8 * 4); // 02.04.09
  //  write(otklik, nTrackSMY, 8 * 4); // 02.04.09
  //  write(otklik, nTrackSM, 8 * 4);  // 02.04.09
  //
  //  write(otklik, EdepCntSCT1, 9 * 5 * 2 * 4);
  //
  //  TEMP1 = -1.;
  //  write(otklik, temp1, 4);

  N_event++;

  G4cout << N_event << '\t' << theta << '\t' << phi << '\t' << NVD_edep << '\t'
         << NVD_npe << '\t' << MuTrackLenNVD << '\t' << nMuNVD << '\t'
         << eMuNVD / 1000. << '\t' << eMuNVD1 / 1000. << '\t' << muDCR << '\t'
         << muDCRw << '\t' << muSM << '\t' << muSMw << '\t' << nSM << '\t'
         << nSMw << '\t' << *timer << G4endl;
}

} // namespace NEVOD