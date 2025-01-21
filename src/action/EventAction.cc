#include "EventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Trajectory.hh"

namespace NEVOD {

double normDistribution(double a, double sigma) {
  G4double g1, g2, norm;

  g1 = G4UniformRand();
  g2 = G4UniformRand();
  norm = std::sqrt(-2. * log(g1)) * cos(2. * M_PI * g2);
  norm = a + norm * sigma;

  return norm;
}

// shared variables
extern G4long eventCount;
extern G4int feuCount;
extern G4int perevKM[546][4];

// thread local variables
// extern G4int otklik;
extern G4String output_file;

extern G4long runNum;
extern G4long eventNum;
extern G4long photoelNum[546];
extern G4double energyDepNEVOD;
extern G4long particleCountNEVOD;

extern G4double theta, phi;
extern G4double muonNEVOD[501][8][2];
extern G4double muonDECOR[501][8][8][2];

extern G4double edepCountSCT[80];

EventAction::EventAction() { fTimer = new G4Timer; }

EventAction::~EventAction() { delete fTimer; }

void EventAction::BeginOfEventAction(const G4Event*) {
  for (G4int i = 1; i < 2; i++)
    for (G4int j = 0; j < 8; j++)
      for (G4int k = 0; k < 2; k++)
        muonNEVOD[i][j][k] = 0.;
  for (G4int i = 1; i < 2; i++)
    for (G4int j = 0; j < 8; j++)
      for (G4int k = 0; k < 8; k++)
        for (G4int m = 0; m < 2; m++)
          muonDECOR[i][j][k][m] = 0.;  // new 30.03.2020
  for (G4int i = 1; i < 2; i++)
    for (G4int k = 0; k < 2; k++)
      muonNEVOD[i][0][k] = -1.;

  for (G4long& i: photoelNum)
    i = 0;

  for (G4double& k: edepCountSCT)
    k = 0.;

  energyDepNEVOD = 0.;
  particleCountNEVOD = 0.;

  fTimer->Start();
}

void EventAction::EndOfEventAction(const G4Event*) {
  // init some values
  G4long muonCountNEVOD;
  G4double muonTrackLengthNEVOD;
  G4long hitSMPLY[8][8];

  G4long nTrackSMX[8], nTrackSMY[8], nTrackSM[8], nTrackSMXw[8], nTrackSMYw[8], nTrackSMw[8];
  G4long hSM[8], hSMw[8], nSM, nSMw;
  G4long muSM, muSMw, muDCR, muDCRw;

  G4long MuTrackPLX, MuTrackPLY;
  G4long hitSMPLYe, hitSMPLYo;

  G4long amplitude, photoelCount;
  G4double amplitudeKSM[7][4][4][6];
  G4double Q;
  G4double amplitude_d;
  G4long planeNum, strideNum, moduleNum, feuNum;

  G4double muonEnergyNevodStart, muonEnergyNevodEnd;

  G4double edepCountSCT1[9][5][2];

  // sett all to zero
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

  for (auto& i: hitSMPLY)
    for (G4long& j: i)
      j = 0;

  muonTrackLengthNEVOD = 0;
  muonCountNEVOD = 0;
  muonEnergyNevodStart = 0.;
  muonEnergyNevodEnd = 0.;

  for (auto& i: edepCountSCT1)
    for (auto& j: i)
      for (G4double& k: j)
        k = 0.;

  for (planeNum = 0; planeNum < 7; planeNum++)
    for (strideNum = 0; strideNum < 4; strideNum++)
      for (moduleNum = 0; moduleNum < 4; moduleNum++)
        for (feuNum = 0; feuNum < 6; feuNum++) {
          amplitudeKSM[planeNum][strideNum][moduleNum][feuNum] = 0;
        }

  for (G4int ism = 0; ism < 8; ism++) {
    MuTrackPLX = 0;
    for (G4int ipl = 0; ipl < 8; ipl++)
      if (muonDECOR[1][ism][ipl][0] > 0) MuTrackPLX++;
    if (MuTrackPLX >= 5) {
      nTrackSMX[ism]++;
      if (phi >= 15. && phi <= 75.)  // 195-255 (exp)
        if (ism >= 2) nTrackSMXw[ism]++;
      if (phi >= 285. && phi <= 345.)  // 105-165 (exp)
        if (ism >= 0 && ism <= 5) nTrackSMXw[ism]++;
    }

    // start calculations
    MuTrackPLY = 0;
    for (G4int ipl = 0; ipl < 8; ipl++)
      if (muonDECOR[1][ism][ipl][1] > 0) MuTrackPLY++;
    if (MuTrackPLY >= 5) {
      nTrackSMY[ism]++;
      if (phi >= 15. && phi <= 75.)  // 195-255 (exp)
        if (ism >= 2) nTrackSMYw[ism]++;
      if (phi >= 285. && phi <= 345.)  // 105-165 (exp)
        if (ism >= 0 && ism <= 5) nTrackSMYw[ism]++;
    }

    for (G4int ipl = 0; ipl < 8; ipl++)
      if (muonDECOR[1][ism][ipl][1] > 0) hitSMPLY[ism][ipl]++;

    if (MuTrackPLX >= 5 && MuTrackPLY >= 5) {
      nTrackSM[ism]++;
      if (phi >= 15. && phi <= 75.)  // 195-255 (exp)
        if (ism >= 2) nTrackSMw[ism]++;
      if (phi >= 285. && phi <= 345.)  // 105-165 (exp)
        if (ism >= 0 && ism <= 5) nTrackSMw[ism]++;
    }
  }

  for (G4int i = 1; i < 2; i++) {
    if (muonNEVOD[i][0][0] >= 0 && muonNEVOD[i][0][1] >= 0) {
      muonCountNEVOD++;
      muonTrackLengthNEVOD += sqrt(
          (muonNEVOD[i][1][1] - muonNEVOD[i][1][0]) * (muonNEVOD[i][1][1] - muonNEVOD[i][1][0]) +
          (muonNEVOD[i][2][1] - muonNEVOD[i][2][0]) * (muonNEVOD[i][2][1] - muonNEVOD[i][2][0]) +
          (muonNEVOD[i][3][1] - muonNEVOD[i][3][0]) * (muonNEVOD[i][3][1] - muonNEVOD[i][3][0]));
      muonEnergyNevodStart += muonNEVOD[i][7][0];
      muonEnergyNevodEnd += muonNEVOD[i][7][1];
    }
  }

  for (G4int ism = 0; ism < 8; ism++) {
    hitSMPLYe = 0;
    for (G4int ipl = 0; ipl < 7; ipl = ipl + 2)  //
    {
      hitSMPLYe += hitSMPLY[ism][ipl];  // 02.09.20 ok
    }
    hitSMPLYo = 0;
    for (G4int ipl = 1; ipl < 8; ipl = ipl + 2)  //
    {
      hitSMPLYo += hitSMPLY[ism][ipl];  // 02.09.20 ok
    }

    if (hitSMPLYe >= 2 && hitSMPLYo >= 2) {
      hSM[ism] = 1;
      if (phi >= 15. && phi <= 75.)  // 195-255 (exp)
        if (ism >= 2) hSMw[ism] = 1;
      if (phi >= 285. && phi <= 345.)  // 105-165 (exp)
        if (ism >= 0 && ism <= 5) hSMw[ism] = 1;
    }
  }

  for (G4int ism = 0; ism < 8; ism++) {
    if (hSM[ism] > 0) nSM++;
    if (hSMw[ism] > 0) nSMw++;
    if (nTrackSM[ism] > 0) muSM++;
    if (nTrackSMw[ism] > 0) muSMw++;
    if (nTrackSMX[ism] > 0) muDCR += nTrackSMX[ism];
    if (nTrackSMXw[ism] > 0) muDCRw += nTrackSMXw[ism];
  }

  G4int kcnt;
  // top
  for (G4int j = 0; j < 5; j++) {
    kcnt = 9 * j;
    for (G4int i = 0; i < 4; i++) {
      edepCountSCT1[j * 2][i][1] = edepCountSCT[kcnt];  // plane, counter, bottom
      kcnt++;                                           // (0)/top(1)
    }
  }
  for (G4int j = 0; j < 4; j++) {
    kcnt = 4 + 9 * j;
    for (G4int i = 0; i < 5; i++) {
      edepCountSCT1[j * 2 + 1][i][1] = edepCountSCT[kcnt];
      kcnt++;
    }
  }
  // bottom
  for (G4int j = 0; j < 5; j++) {
    kcnt = 9 * j + 40;
    for (G4int i = 0; i < 4; i++) {
      edepCountSCT1[j * 2][i][0] = edepCountSCT[kcnt];
      kcnt++;
    }
  }
  for (G4int j = 0; j < 4; j++) {
    kcnt = 4 + 9 * j + 40;
    for (G4int i = 0; i < 5; i++) {
      edepCountSCT1[j * 2 + 1][i][0] = edepCountSCT[kcnt];
      kcnt++;
    }
  }

  muonTrackLengthNEVOD /= 1000.;  // m

  for (G4int j = 0; j < feuCount; j++) {
    planeNum = perevKM[j][0];
    strideNum = perevKM[j][1];
    moduleNum = perevKM[j][2];
    feuNum = perevKM[j][3];
    photoelCount = photoelNum[j];
    if (photoelCount > 0) {
      amplitude_d = 0;
      for (G4int i = 0; i < photoelCount; i++) {             // Amp(Q)
        Q = -4. * log(1. - G4UniformRand());                 // !!! ??? G4UniformRand != 1.
        if (photoelCount > 1) Q = normDistribution(Q, 2.8);  // delta_1e*A_1e = 0.7*4.0 = 2.8
        amplitude_d += Q;
      }
      if (amplitude_d < 1.0) amplitude_d = 1.0;
      //      amplitude = long(amplitude_d);
      amplitudeKSM[planeNum][strideNum][moduleNum][feuNum] = amplitude_d;
    }  // if(photoelCount > 0)
  }  // end cikl po feu

  EventOutput event{
      eventCount,
      runNum,
      eventNum,
      theta,
      phi,
      energyDepNEVOD,
      particleCountNEVOD,
      muonTrackLengthNEVOD,
      muonCountNEVOD,
      muonEnergyNevodStart,
      muonEnergyNevodEnd,
      muDCR,
      muSM,
      nSM,
      muDCRw,
      muSMw,
      nSMw};

  FileWriter file_writer = FileWriter(output_file);

  file_writer.writeEvent(event, amplitudeKSM, hSM, nTrackSMX, nTrackSMY, nTrackSM, edepCountSCT1);

  fTimer->Stop();

  eventCount++;

  G4cout << eventCount << '\t' << runNum << '\t' << eventNum << '\t' << theta << '\t' << phi << '\t' << energyDepNEVOD << '\t' << particleCountNEVOD
         << '\t' << muonTrackLengthNEVOD << '\t' << muonCountNEVOD << '\t' << muonEnergyNevodStart / 1000. << '\t' << muonEnergyNevodEnd / 1000.
         << '\t' << muDCR << '\t' << muDCRw << '\t' << muSM << '\t' << muSMw << '\t' << nSM << '\t' << nSMw << '\t' << *fTimer << G4endl;
}

}  // namespace NEVOD