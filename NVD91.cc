#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4OpticalPhysics.hh"
#include "NVDDetectorConstruction.hh"
#include "NVDEventAction.hh"
#include "NVDPrimaryGeneratorAction.hh"
#include "NVDRunAction.hh"
#include "NVDSteppingAction.hh"
#include "QGSP_BERT.hh"

#include "G4Timer.hh"
#include "Randomize.hh"

#include <fcntl.h>
#include <iostream>
#include <unistd.h>

// #define G4VIS_USE
// #define G4UI_USE

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

float NormDistr(float a, float sigma);

G4long N_event;
G4int otklik;
G4long NumPhotEl[600];
// G4float TimePhotEl[600];
G4int PerevKM[600][4], PerevMK[7][4][4][6];
G4int Nfeu;
G4float NVD_edep, NVD_edep1, NVD_totnpe, NVD_npe;

G4float nRunN, nEvtN, SM1x, SM1y, SM1z, SM2x, SM2y, SM2z, dt06, dA13, Nqsm,
    Npmt, Aup, Adown; // DECOR, SCT

G4float Teta, Fi;            // bundle
G4int MuBundle;              // bundle
G4float MuNVD[501][8][2];    // bundle
G4float MuDCR[501][8][8][2]; // new 30.03.2020
G4double emu_bundle[501];    // bundle
G4float MuTrackLenNVD, nMuNVD, nMuSM[8], nMuSMwat[8], nHitSM, nHitSMwat,
    nMuDCR1, nMuDCRwat1, nMuDCR, nMuDCRwat;    // bundle
G4float hitSMPLY[8][8], hitSM[8], hitSMwat[8]; // bundle new (trig) 04.08.20

// // massiv, EAS
G4float PMDx, PMDy, PMDz, PE, PPx, PPy, PPz, PD, timC; // EAS

// Quantum efficiency of PMT
// Hamamatsu (interpolation)
G4double Kv_Eff[70] = {
    0.00144, 0.00326, 0.00607, 0.01112, 0.0175,  0.02482, 0.03376, 0.04295,
    0.05624, 0.07074, 0.08746, 0.10628, 0.12561, 0.14396, 0.15952, 0.17364,
    0.18658, 0.19539, 0.20421, 0.21335, 0.22272, 0.23155, 0.2387,  0.24584,
    0.24999, 0.25268, 0.25468, 0.2557,  0.25673, 0.25734, 0.25734, 0.25734,
    0.25734, 0.25528, 0.25241, 0.24954, 0.24667, 0.24224, 0.23436, 0.22647,
    0.21859, 0.21071, 0.2026,  0.19437, 0.18613, 0.1779,  0.16798, 0.15762,
    0.14725, 0.13666, 0.12585, 0.11504, 0.10466, 0.09489, 0.08596, 0.07922,
    0.07249, 0.06589, 0.05942, 0.05296, 0.04709, 0.04214, 0.03816, 0.03491,
    0.03148, 0.028,   0.025,   0.02243, 0.01997, 0.01752};

G4float EdepCntSCT[80], EdepDetNE[144]; // 112
G4float TimDetNE[144][5];               // min, th, max, mean, nparticles
G4float EdepStNE[36], TimStNE[36][5];   // 112

//===== PRISMA-URAN

G4float tim_distrPr[32][20003]; // time neutr distrib
G4float tim_distrU[72][20003];
G4float EdepPr[32], EdepU[72];
G4float TimeC; // time in corsika

G4float nnPr[32], nnU[72];
G4float enPr[32], enU[72];

G4int fHeT, fHeLi;
G4long Nid;

//===== PRISMA-URAN

int main(int argc, char **argv) {
  G4float cnt_part;

  //============

  G4float nTrackSMX[8], nTrackSMY[8], nTrackSM[8], nTrackSMXw[8], nTrackSMYw[8],
      nTrackSMw[8];
  G4float hSM[8], hSMw[8], nSM, nSMw;
  G4float muSM, muSMw, muDCR, muDCRw;

  G4int MuTrackPLX, MuTrackPLY, MuTrackPLYe, MuTrackPLYo;
  G4float hSMPLY[8][8];       // new (trig) 04.08.20
  G4int hitSMPLYe, hitSMPLYo; // new (trig) 04.08.20
  G4float hSMPLs[8][8][2];
  G4float nTrackSMX0[8], nTrackSMY0[8], nTrackSM0[8], nTrackSMXw0[8],
      nTrackSMYw0[8], nTrackSMw0[8];
  G4float hSM0[8], hSMw0[8], nSM0, nSMw0;
  G4float muSM0, muSMw0, muDCR0, muDCRw0;

  G4long A, Nfe;
  G4float AmplKSM[7][4][4][6];
  G4double amp, t_rez, Q, t_sh, gam;
  G4int npl, nstr, nmod, nfeu, nms[7] = {4, 3, 4, 3, 4, 3, 4};

  G4float eMuNVD, eMuNVD1;

  G4float EdepCntSCT1[9][5][2], EdepDetNE1[9][4][4]; // EdepDetNE1[7][4][4]
  G4float TimDetNE1[9][4][4][4];
  G4float EdepStNE1[9][4], TimStNE1[9][4][4];

  G4float MuTrackPlane, MuTrackPlaneE, MuTrackPlaneO;
  G4float TetaMu, FiMu;
  G4float PsiMuP, cosin1Mu, cosin2Mu, cosin3Mu, cosin1P, cosin2P, cosin3P;
  G4float NVDc[3][2], NEc[9][3][2], tNVD[2], tNE[9][2];
  G4bool flag_beam;

  //======= EAS
  float PrimPX, PrimPY, PrimPZ, PrimP;
  float TETA1, FI1;
  float Nee;
  float missNVD;
  // int nabor, naborname1e, naborname2t, n1file, nfiles, nshower, ncellx,
  // ncelly, sector, naxis;
  int nabor, naborname1e, naborname2t, n1file, nfiles, n1shower, nshowers,
      ncellx, ncelly, sector, naxis;
  int choiceXYaxis, choiceSecPart;
  // char filedat[256], filetxt[256], cfile[256], gfile[256];
  char filedat[512], filetxt[512], cfile[512], gfile[512];
  int cntblk;
  bool RUNE;
  int iFileHandle;
  float BUF1[5733], *buf1, BUF2[273];
  float NRUN, NEVRUN;
  float NEVENT, PART0, E0, H1INT, TETA, FI;
  float NGAM, NEL, NHADR, NMU, NALLPART;
  float NeNKGlong, sNKGlong;
  float PARTDESCR;
  int NLx, ILx, IDPART, IAx, IZx, INx, gen;
  float PX, PY, PZ, X, Y, T, mass, Ekin;
  float cnteas, cntev, P, R, X1, Y1, R1, Eth;
  float XAxisShift, YAxisShift;
  int TEMP, *temp;
  union b2h {
    float BUF3;
    char HEADER[5];
  } B2H{};
  G4float TEMP1, *temp1;

  temp = &TEMP;
  buf1 = &BUF1[0];

  temp1 = &TEMP1;

  //======= CORSIKA
  double PAMA[6001];
  double MASSES[76] = {
      0.E0,         0.E0,          0.51099892E-3, 0.51099892E-3,
      0.E0,         0.105658369E0, 0.105658369E0, 0.1349766E0,
      0.13957018E0, 0.13957018E0,  0.497648E0, // 10
      0.493677E0,   0.493677E0,    0.93956536E0,  0.93827203E0,
      0.93827203E0, 0.497648E0,    0.54775E0,     1.115683E0,
      1.18937E0,    1.192642E0, // 20
      1.197449E0,   1.31483E0,     1.32131E0,     1.67245E0,
      0.93956536E0, 1.115683E0,    1.18937E0,     1.192642E0,
      1.197449E0,   1.31483E0, // 30
      1.32131E0,    1.67245E0,     0.E0,          0.E0,
      0.E0,         0.E0,          0.E0,          0.E0,
      0.E0,         0.E0, // 40
      0.E0,         0.E0,          0.E0,          0.E0,
      0.E0,         0.E0,          0.E0,          0.E0,
      0.E0,         0.78259E0, // 50
      0.7690E0,     0.7665E0,      0.7665E0,      1.2305E0,
      1.2318E0,     1.2331E0,      1.2344E0,      1.2309E0,
      1.2323E0,     1.2336E0, // 60
      1.2349E0,     0.89610E0,     0.89166E0,     0.89166E0,
      0.89610E0,    0.E0,          0.E0,          0.E0,
      0.E0,         0.E0, // 70
      0.54775E0,    0.54775E0,     0.54775E0,     0.54775E0,
      0.E0};
  double MASSES2[101] = {
      0.E0,
      //   15*0.E0,
      0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0,
      0.E0, 0.E0, 0.E0, 1.8645E0, 1.8697E0, 1.8697E0, 1.8645E0, 1.9682E0, // 120
      1.9682E0, 2.9804E0, 2.0067E0, 2.0100E0, 2.0100E0, 2.0067E0, 2.1121E0,
      2.1121E0, 0.0E0, 3.096916E0, // 130
      1.77699E0, 1.77699E0, 0.E0, 0.E0, 0.E0, 0.E0, 2.28646E0, 2.4679E0,
      2.4710E0, 2.45402E0, // 140
      2.4529E0, 2.45376E0, 2.5757E0, 2.5780E0, 2.6975E0, 0.E0, 0.E0, 0.E0,
      2.28646E0, 2.4679E0, // 150
      2.4710E0, 2.45402E0, 2.4529E0, 2.45376E0, 2.5757E0, 2.5780E0, 2.6975E0,
      0.E0, 0.E0, 0.E0, // 160
      2.5184E0, 2.5175E0, 2.5180E0, 0.E0, 0.E0,
      //   5*0.E0     , //170
      0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 2.5184E0, 2.5175E0, 2.5180E0, 0.E0, 0.E0,
      //   5*0.E0     , //180
      0.E0, 0.E0, 0.E0, 0.E0, 0.E0,
      //   20*0.E0
      0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0,
      0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0, 0.E0};

  int IA, IC, IN1, IP;
  //======= CORSIKA

  //======= EAS

  std::ifstream ExpMuTrack; // DECOR, SCT
  std::ifstream EmuBundle;  // bundle
  G4double dNdlgEmu[101];   // bundle
  std::ofstream oFile;      // bundle

  G4float evNum_, y0_, E_, xMax_, yMax_, zMax_, eAxis0_, eAxis1_, eAxis2_,
      xIni_, yIni_, zIni_; // cascade

  nRunN = 0;
  nEvtN = 0;
  PrimPX = 0;
  PrimPY = 0;
  PrimPZ = 0;
  Eth = 2.;     // GeV
  missNVD = 0.; // m

  N_event = 0;
  NRUN = 0;
  NEVENT = 0;
  PART0 = 0;
  E0 = 0;
  Teta = 0;
  Fi = 0;
  XAxisShift = 0.; // m
  YAxisShift = 0.; // m
  H1INT = 0;
  NGAM = 0;
  NEL = 0;
  Nee = 0;
  NHADR = 0;
  NMU = 0;
  NeNKGlong = 0;
  sNKGlong = 0;

  NVD_edep = 0;
  NVD_npe = 0;
  MuBundle = 0.;
  MuTrackLenNVD = 0;
  nMuNVD = 0;
  eMuNVD = 0.;
  eMuNVD1 = 0.;
  nMuDCR = 0;
  nHitSM = 0;
  nMuDCRwat = 0;
  nHitSMwat = 0;

  PMDx = 0.;
  PMDy = 0.;
  PMDz = 1.;
  PE = 100.; // GeV
  PPx = 0.;
  PPy = 0.;
  PPz = 13.5;
  //   PPz = 0.;
  PD = 5;
  timC = 0.;

  for (G4int i = 0; i < 600; i++) {
    NumPhotEl[i] = 0;
  }
  for (int k = 0; k < 80; k++)
    EdepCntSCT[k] = 0.;
  for (int k = 0; k < 144; k++) // 112
    EdepDetNE[k] = 0.;
  for (int k = 0; k < 144; k++)
    for (int m = 0; m < 5; m++)
      TimDetNE[k][m] = 0.;
  for (int k = 0; k < 144; k++)
    TimDetNE[k][1] = -1.;
  for (int k = 0; k < 144; k++)
    TimDetNE[k][2] = 1.E20;

  for (int k = 0; k < 36; k++)
    EdepStNE[k] = 0.;
  for (int k = 0; k < 36; k++)
    for (int m = 0; m < 5; m++)
      TimStNE[k][m] = 0.;
  for (int k = 0; k < 36; k++)
    TimStNE[k][1] = -1.;
  for (int k = 0; k < 36; k++)
    TimStNE[k][2] = 1.E20;

  //======= NEVOD-EAS

  G4double x0, y0, z0, t0, xQ, yQ, zQ;
  G4bool fHitNEstat, fHitNDstat;

  x0 = 0;
  y0 = 0;
  z0 = 0;
  t0 = 0;
  xQ = 0;
  yQ = 0;
  zQ = 0;
  fHitNEstat = false;
  fHitNDstat = false;

  // NE stations
  G4float csc[9][4][3];
  csc[0][0][0] = -25.36;
  csc[0][0][1] = 5.89;
  csc[0][0][2] = -6.68;
  csc[0][1][0] = -37.61;
  csc[0][1][1] = 5.89;
  csc[0][1][2] = -6.68;
  csc[0][2][0] = -37.61;
  csc[0][2][1] = -7.32;
  csc[0][2][2] = -6.68;
  csc[0][3][0] = -25.36;
  csc[0][3][1] = -7.32;
  csc[0][3][2] = -6.68;

  csc[1][0][0] = -25.36;
  csc[1][0][1] = 37.34;
  csc[1][0][2] = -6.68;
  csc[1][1][0] = -37.61;
  csc[1][1][1] = 37.34;
  csc[1][1][2] = -6.68;
  csc[1][2][0] = -37.61;
  csc[1][2][1] = 24.14;
  csc[1][2][2] = -6.68;
  csc[1][3][0] = -25.36;
  csc[1][3][1] = 24.14;
  csc[1][3][2] = -6.68;

  csc[2][0][0] = 6.48;
  csc[2][0][1] = 12.65;
  csc[2][0][2] = 0.00;
  csc[2][1][0] = -6.87;
  csc[2][1][1] = 12.65;
  csc[2][1][2] = 0.00;
  csc[2][2][0] = -6.87;
  csc[2][2][1] = -12.63;
  csc[2][2][2] = 0.00;
  csc[2][3][0] = 6.48;
  csc[2][3][1] = -12.63;
  csc[2][3][2] = 0.00;

  csc[3][0][0] = 37.37;
  csc[3][0][1] = 10.96;
  csc[3][0][2] = -14.95;
  csc[3][1][0] = 22.47;
  csc[3][1][1] = 10.96;
  csc[3][1][2] = -15.27;
  csc[3][2][0] = 22.47;
  csc[3][2][1] = -3.95;
  csc[3][2][2] = -15.35;
  csc[3][3][0] = 37.37;
  csc[3][3][1] = -3.95;
  csc[3][3][2] = -15.08;

  csc[4][0][0] = 37.37;
  csc[4][0][1] = 45.57;
  csc[4][0][2] = -16.46;
  csc[4][1][0] = 22.98;
  csc[4][1][1] = 45.57;
  csc[4][1][2] = -16.18;
  csc[4][2][0] = 22.98;
  csc[4][2][1] = 28.48;
  csc[4][2][2] = -16.52;
  csc[4][3][0] = 37.37;
  csc[4][3][1] = 28.48;
  csc[4][3][2] = -16.52;

  csc[5][0][0] = 6.92; // 6.91
  csc[5][0][1] = -56.16;
  csc[5][0][2] = -16.15;
  csc[5][1][0] = -2.79;
  csc[5][1][1] = -56.16;
  csc[5][1][2] = -15.91;
  csc[5][2][0] = -2.79;
  csc[5][2][1] = -70.54;
  csc[5][2][2] = -15.93;
  csc[5][3][0] = 6.92; // 6.91
  csc[5][3][1] = -70.54;
  csc[5][3][2] = -16.10;

  csc[6][0][0] = -9.54;
  csc[6][0][1] = 53.62;
  csc[6][0][2] = -17.03;
  csc[6][1][0] = -20.56;
  csc[6][1][1] = 64.71;
  csc[6][1][2] = -17.80;
  csc[6][2][0] = -20.56;
  csc[6][2][1] = 53.62;
  csc[6][2][2] = -17.50;
  csc[6][3][0] = -9.54;
  csc[6][3][1] = 42.54;
  csc[6][3][2] = -16.93;

  csc[7][0][0] = 42.14;
  csc[7][0][1] = -16.80;
  csc[7][0][2] = -15.14;
  csc[7][1][0] = 26.94;
  csc[7][1][1] = -16.80;
  csc[7][1][2] = -15.46;
  csc[7][2][0] = 26.94;
  csc[7][2][1] = -32.10;
  csc[7][2][2] = -15.46;
  csc[7][3][0] = 42.14;
  csc[7][3][1] = -32.10;
  csc[7][3][2] = -14.69;

  csc[8][0][0] = -25.36;
  csc[8][0][1] = -25.75;
  csc[8][0][2] = -7.33;
  csc[8][1][0] = -37.26;
  csc[8][1][1] = -25.75;
  csc[8][1][2] = -7.36;
  csc[8][2][0] = -37.26;
  csc[8][2][1] = -39.55;
  csc[8][2][2] = -7.34;
  csc[8][3][0] = -25.36;
  csc[8][3][1] = -39.55;
  csc[8][3][2] = -7.28;

  //======= NEVOD-EAS

  //===== PRISMA-URAN

  for (G4int j = 0; j < 32; j++) {
    EdepPr[j] = 0.;
  }

  for (G4int j = 0; j < 72; j++) {
    EdepU[j] = 0.;
  }
  for (G4int j = 0; j < 32; j++) {
    for (G4int i = 0; i < 20003; i++) {
      tim_distrPr[j][i] = 0.;
    }
  }

  for (G4int j = 0; j < 72; j++) {
    for (G4int i = 0; i < 20003; i++) {
      tim_distrU[j][i] = 0.;
    }
  }

  for (G4int j = 0; j < 32; j++) {
    nnPr[j] = 0.;
    enPr[j] = 0.;
  }
  for (G4int j = 0; j < 72; j++) {
    nnU[j] = 0.;
    enU[j] = 0.;
  }

  //===== PRISMA-URAN

  for (int i = 0; i < 70; i++) {
    Kv_Eff[i] = Kv_Eff[i] / 2.65;
  }

  // Seed the random number generator manually
  CLHEP::HepRandom::setTheEngine(new CLHEP::Ranlux64Engine);

  G4long myseed = 47212; // 345354
  CLHEP::HepRandom::setTheSeed(myseed);

  G4Timer *fTimer;
  fTimer = new G4Timer();

  // Run manager
  G4RunManager *runManager = new G4RunManager;

  // UserInitialization classes - mandatory
  G4VUserDetectorConstruction *detector = new NVDDetectorConstruction;
  runManager->SetUserInitialization(detector);

  G4VModularPhysicsList *physics = new QGSP_BERT; // optical

  G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics();
  physics->RegisterPhysics(opticalPhysics);

  runManager->SetUserInitialization(physics);

// 4 vis
#ifdef G4VIS_USE
  // visualization manager
  G4VisManager *visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  // UserAction classes
  //
  G4UserRunAction *run_action = new NVDRunAction;
  runManager->SetUserAction(run_action);
  //
  G4VUserPrimaryGeneratorAction *gen_action = new NVDPrimaryGeneratorAction;
  runManager->SetUserAction(gen_action);
  //
  G4UserEventAction *event_action = new NVDEventAction;
  runManager->SetUserAction(event_action);
  //
  G4UserSteppingAction *stepping_action = new NVDSteppingAction;
  runManager->SetUserAction(stepping_action);

  // Initialize G4 kernel
  //
  runManager->Initialize();
  // Get the pointer to the UI manager and set verbosities
  //
  G4UImanager *UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/run/verbose 0");
  UImanager->ApplyCommand("/event/verbose 0");
  UImanager->ApplyCommand("/tracking/verbose 0");

  // 4 vis
  // Get the pointer to the User Interface manager
  //
#ifdef G4UI_USE
  if (argc == 1) // Define UI session for interactive mode
  {
    G4UIExecutive *ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute vis.mac");
#endif
    ui->SessionStart();
    delete ui;
  } else // Batch mode
  {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }
#endif

// 4 vis
#ifdef G4VIS_USE
  delete visManager;
#endif

// Job termination
// Free the store: user actions, physics_list and detector_description are
//                 owned and deleted by the run manager, so they should not
//                 be deleted in the main() program !

// simulation
//   for(int nevt=0; nevt<1; nevt++)
//   for(int nevt=0; nevt<500; nevt++) // 1 mu
//   for(int nevt=0; nevt<300; nevt++) // bundles
//   for(int nevt=0; nevt<25; nevt++) // cascade 25,3,13,20
//   for(int nevt=0; nevt<25-atoi(argv[2]); nevt++) // cascade
//   {

// 4 vis for EAS
#ifdef G4VIS_USE
  delete visManager;
#endif
  // 4 vis for EAS

  //======= OneTrack (DECOR SM00_SM01 <-> SM06_SM07)

  G4float SM1x, SM1y, SM1z, SM2x, SM2y, SM2z, dt06, dA13;
  G4float smeSMx, smeSMy, smeSMz;
  G4float VTx, VTy, VTz;

  N_event = 0;
  cntev = 0.;

  NRUN = 0;
  NEVENT = 0;
  PART0 = 0;
  E0 = 0;
  Teta = 0;
  Fi = 0;
  XAxisShift = 0;
  YAxisShift = 0;
  int icellx = 0;
  int icelly = 0;
  sector = 0;
  H1INT = 0;
  NGAM = 0;
  NEL = 0;
  NHADR = 0;
  NMU = 0;
  NeNKGlong = 0;
  sNKGlong = 0;

  smeSMx = 4.5;
  smeSMy = 13.;
  //   smeSMz = 4.5;
  smeSMz = 4.5 - 0.3; // pravilno
                      //   smeSMz = 4.5-0.3-0.3;

  G4float emu;
  G4float ax, bx, az, bz, SMx, SMy, SMz;

  nabor = 928;
  nabor = atoi(argv[1]);
  int shift = 0;
  shift = atoi(argv[2]);

  sprintf(cfile,
          "/home/mawerid/research/nevod/simulation/tmp/NVD91_Cascade/ot/"
          "OTDCR_%d.txt",
          nabor);         //
  ExpMuTrack.open(cfile); // 1 mu

  // prokrutka 1 mu
  for (int nevt = 0; nevt < shift; nevt++) {

    // horizontal mu
    ExpMuTrack >> NRUN;
    ExpMuTrack >> NEVENT;
    ExpMuTrack >> SM1x;
    ExpMuTrack >> SM1y;
    ExpMuTrack >> SM1z;
    ExpMuTrack >> SM2x;
    ExpMuTrack >> SM2y;
    ExpMuTrack >> SM2z;
  }
  // prokrutka 1 mu

  sprintf(gfile,
          "/home/mawerid/research/nevod/simulation/tmp/NVD91_Cascade/out/"
          "otdcr_%d_%d.dat",
          nabor, shift); //
  otklik =
      open(gfile, O_CREAT | O_WRONLY,
           S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // EAS

  // simulation
  for (int nevt = 0; nevt < 500; nevt++) // 1 mu
  //   for(int nevt=0; nevt<1000; nevt++) // emuspe
  {

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

    for (G4int ism = 0; ism < 8; ism++) //
    {
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

    for (G4int i = 0; i < 8; i++) // new (trig) 04.08.20
      for (G4int j = 0; j < 8; j++)
        hitSMPLY[i][j] = 0;

    NVD_edep = 0.;
    NVD_edep1 = 0.;
    NVD_npe = 0.;
    NVD_totnpe = 0.;
    MuBundle = 0.;
    MuTrackLenNVD = 0;
    nMuNVD = 0;
    eMuNVD = 0.;
    eMuNVD1 = 0.;
    nMuDCR = 0;
    nHitSM = 0;
    nMuDCRwat = 0;
    nHitSMwat = 0;
    for (G4int j = 0; j < 8; j++) // ###
    {
      nMuSM[j] = 0;
      nMuSMwat[j] = 0;
    }
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

    for (G4int i = 0; i < 600; i++) {
      NumPhotEl[i] = 0;
    }

    for (int k = 0; k < 80; k++)
      EdepCntSCT[k] = 0.;

    for (int k = 0; k < 144; k++) // 112
      EdepDetNE[k] = 0.;
    for (int k = 0; k < 144; k++)
      for (int m = 0; m < 5; m++)
        TimDetNE[k][m] = 0.;
    for (int k = 0; k < 144; k++)
      TimDetNE[k][1] = -1.;
    for (int k = 0; k < 144; k++)
      TimDetNE[k][2] = 1.E20;
    for (int k = 0; k < 36; k++)
      EdepStNE[k] = 0.;
    for (int k = 0; k < 36; k++)
      for (int m = 0; m < 5; m++)
        TimStNE[k][m] = 0.;
    for (int k = 0; k < 36; k++)
      TimStNE[k][1] = -1.;
    for (int k = 0; k < 36; k++)
      TimStNE[k][2] = 1.E20;

    P = sqrt(PMDx * PMDx + PMDy * PMDy + PMDz * PMDz);
    if (P != 0.)
      TetaMu = acos(PMDz / P) * 180. / M_PI;
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

    fTimer->Start();

    runManager->BeamOn(1);

    MuBundle = MuBundle + 1;

    // 04.09.20
    for (G4int ism = 0; ism < 8; ism++) {
      MuTrackPLX = 0;
      for (G4int ipl = 0; ipl < 8; ipl++)
        if (MuDCR[1][ism][ipl][0] > 0)
          MuTrackPLX = MuTrackPLX + 1;
      if (MuTrackPLX >= 5) {
        nTrackSMX[ism] = nTrackSMX[ism] + 1.;
        if (Fi >= 15. && Fi <= 75.) // 195-255 (exp)
          if (ism >= 2 && ism <= 7)
            nTrackSMXw[ism] = nTrackSMXw[ism] + 1.;
        if (Fi >= 285. && Fi <= 345.) // 105-165 (exp)
          if (ism >= 0 && ism <= 5)
            nTrackSMXw[ism] = nTrackSMXw[ism] + 1.;
      }

      MuTrackPLY = 0;
      for (G4int ipl = 0; ipl < 8; ipl++)
        if (MuDCR[1][ism][ipl][1] > 0)
          MuTrackPLY = MuTrackPLY + 1;
      if (MuTrackPLY >= 5) {
        nTrackSMY[ism] = nTrackSMY[ism] + 1.;
        if (Fi >= 15. && Fi <= 75.) // 195-255 (exp)
          if (ism >= 2 && ism <= 7)
            nTrackSMYw[ism] = nTrackSMYw[ism] + 1.;
        if (Fi >= 285. && Fi <= 345.) // 105-165 (exp)
          if (ism >= 0 && ism <= 5)
            nTrackSMYw[ism] = nTrackSMYw[ism] + 1.;
      }

      for (G4int ipl = 0; ipl < 8; ipl++)
        if (MuDCR[1][ism][ipl][1] > 0)
          hitSMPLY[ism][ipl] = hitSMPLY[ism][ipl] + 1.;

      if (MuTrackPLX >= 5 && MuTrackPLY >= 5) {
        nTrackSM[ism] = nTrackSM[ism] + 1.;
        if (Fi >= 15. && Fi <= 75.) // 195-255 (exp)
          if (ism >= 2 && ism <= 7)
            nTrackSMw[ism] = nTrackSMw[ism] + 1.;
        if (Fi >= 285. && Fi <= 345.) // 105-165 (exp)
          if (ism >= 0 && ism <= 5)
            nTrackSMw[ism] = nTrackSMw[ism] + 1.;
      }
    }
    // 04.09.20

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

    cntev = cntev + 1;

    // 04.09.20
    for (G4int ism = 0; ism < 8; ism++) //
    {
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
        if (Fi >= 15. && Fi <= 75.) // 195-255 (exp)
          if (ism >= 2 && ism <= 7)
            hSMw[ism] = 1;
        if (Fi >= 285. && Fi <= 345.) // 105-165 (exp)
          if (ism >= 0 && ism <= 5)
            hSMw[ism] = 1;
      }
    }

    for (G4int ism = 0; ism < 8; ism++) //
    {
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
    // 04.09.20

    MuTrackLenNVD = MuTrackLenNVD / 1000.; // m

    for (npl = 0; npl < 7; npl++)
      for (nstr = 0; nstr < 4; nstr++)
        for (nmod = 0; nmod < 4; nmod++)
          for (nfeu = 0; nfeu < 6; nfeu++) {
            AmplKSM[npl][nstr][nmod][nfeu] = 0;
          }

    for (G4int j = 0; j < Nfeu; j++) {
      npl = PerevKM[j][0];
      nstr = PerevKM[j][1];
      nmod = PerevKM[j][2];
      nfeu = PerevKM[j][3];
      Nfe = NumPhotEl[j];
      if (Nfe > 0) {
        amp = 0;
        for (G4int jj = 0; jj < Nfe; jj++)     // ???
        {                                      // Amp(Q)
          Q = -4. * log(1. - G4UniformRand()); // !!! ??? G4UniformRand != 1.
          if (Nfe > 1)
            Q = NormDistr(Q, 2.8); // delta_1e*A_1e = 0.7*4.0 = 2.8
          amp += Q;
        }
        if (amp < 1.0)
          amp = 1; //
        A = int(amp);
        AmplKSM[npl][nstr][nmod][nfeu] = A;
      } // if(Nfe > 0)
    }   // end cikl po feu

    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 5; j++)
        for (int k = 0; k < 2; k++)
          EdepCntSCT1[i][j][k] = 0.;

    for (int i = 0; i < 9; i++) // 7
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          EdepDetNE1[i][j][k] = 0.;
          for (int m = 0; m < 4; m++)
            TimDetNE1[i][j][k][m] = 0.;
        }
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 4; j++) {
        EdepStNE1[i][j] = 0.;
        for (int m = 0; m < 4; m++)
          TimStNE1[i][j][m] = 0.;
      }

    G4int kdet = 0;
    for (int i = 0; i < 9; i++)     // cluster   7
      for (int j = 0; j < 4; j++)   // station
        for (int k = 0; k < 4; k++) // detector
        {
          EdepDetNE1[i][j][k] = EdepDetNE[kdet];
          for (int m = 0; m < 3; m++) //
            TimDetNE1[i][j][k][m] = TimDetNE[kdet][m];
          if (TimDetNE[kdet][4] > 0.)
            TimDetNE1[i][j][k][3] = TimDetNE[kdet][3] / TimDetNE[kdet][4];
          kdet = kdet + 1;
        }
    G4int kst = 0;
    for (int i = 0; i < 9; i++)   // cluster
      for (int j = 0; j < 4; j++) // station
      {
        EdepStNE1[i][j] = EdepStNE[kst];
        for (int m = 0; m < 3; m++)
          TimStNE1[i][j][m] = TimStNE[kst][m];
        if (TimStNE[kst][4] > 0.)
          TimStNE1[i][j][3] = TimStNE[kst][3] / TimStNE[kst][4];
        kst = kst + 1;
      }

    G4float TEMP1, *temp1;
    temp1 = &TEMP1;

    // ###
    //  write
    TEMP1 = N_event;
    write(otklik, temp1, 4);

    TEMP1 = NRUN;
    write(otklik, temp1, 4);
    TEMP1 = NEVENT;
    write(otklik, temp1, 4);
    TEMP1 = PART0;
    write(otklik, temp1, 4);
    TEMP1 = E0;
    write(otklik, temp1, 4);
    //           TEMP1 = TETA*180./M_PI;
    TEMP1 = Teta;
    write(otklik, temp1, 4);
    //           TEMP1 = FI*180./M_PI;
    TEMP1 = Fi;
    write(otklik, temp1, 4);
    TEMP1 = XAxisShift;
    write(otklik, temp1, 4);
    TEMP1 = YAxisShift;
    write(otklik, temp1, 4);

    TEMP1 = H1INT;
    write(otklik, temp1, 4);
    TEMP1 = NGAM;
    write(otklik, temp1, 4);
    TEMP1 = NEL;
    write(otklik, temp1, 4);
    TEMP1 = NHADR;
    write(otklik, temp1, 4);
    TEMP1 = NMU;
    write(otklik, temp1, 4);
    TEMP1 = NeNKGlong;
    write(otklik, temp1, 4);
    TEMP1 = sNKGlong;
    write(otklik, temp1, 4);

    TEMP1 = NVD_edep;
    write(otklik, temp1, 4);
    TEMP1 = NVD_npe;
    write(otklik, temp1, 4);
    TEMP1 = MuBundle;
    write(otklik, temp1, 4);
    TEMP1 = MuTrackLenNVD;
    write(otklik, temp1, 4);
    TEMP1 = nMuNVD;
    write(otklik, temp1, 4);
    TEMP1 = eMuNVD;
    write(otklik, temp1, 4);
    TEMP1 = eMuNVD1;
    write(otklik, temp1, 4);

    TEMP1 = muDCR;           // 04.09.20
    write(otklik, temp1, 4); // ###
    TEMP1 = muSM;            // 04.09.20
    write(otklik, temp1, 4); // ###
    TEMP1 = nSM;             // ###
    write(otklik, temp1, 4); // ###
    TEMP1 = muDCRw;          // 04.09.20
    write(otklik, temp1, 4); // ###
    TEMP1 = muSMw;           // 04.09.20
    write(otklik, temp1, 4); // ###
    TEMP1 = nSMw;            // ###
    write(otklik, temp1, 4); // ###

    TEMP1 = muDCR0;          // 04.09.20
    write(otklik, temp1, 4); // ###
    TEMP1 = muSM0;           // 04.09.20
    write(otklik, temp1, 4); // ###
    TEMP1 = nSM0;            // ###
    write(otklik, temp1, 4); // ###
    TEMP1 = muDCRw0;         // 04.09.20
    write(otklik, temp1, 4); // ###
    TEMP1 = muSMw0;          // 04.09.20
    write(otklik, temp1, 4); // ###
    TEMP1 = nSMw0;           // ###
    write(otklik, temp1, 4); // ###

    write(otklik, AmplKSM, 7 * 4 * 4 * 6 * 4);
    //           write(otklik, nMuSM, 8*4);
    write(otklik, hSM, 8 * 4);        // ###
    write(otklik, nTrackSMX, 8 * 4);  // 02.04.09
    write(otklik, nTrackSMY, 8 * 4);  // 02.04.09
    write(otklik, nTrackSM, 8 * 4);   // 02.04.09
    write(otklik, hSM0, 8 * 4);       // ###
    write(otklik, nTrackSMX0, 8 * 4); // 02.04.09
    write(otklik, nTrackSMY0, 8 * 4); // 02.04.09
    write(otklik, nTrackSM0, 8 * 4);  // 02.04.09

    write(otklik, EdepCntSCT1, 9 * 5 * 2 * 4);
    write(otklik, EdepDetNE1, 9 * 4 * 4 * 4); // 7*4*4*4
    write(otklik, TimDetNE1, 9 * 4 * 4 * 4 * 4);
    write(otklik, EdepStNE1, 9 * 4 * 4);
    write(otklik, TimStNE1, 9 * 4 * 4 * 4);

    TEMP1 = -1.;
    write(otklik, temp1, 4);
    // ###

    N_event++;

    fTimer->Stop();

    G4cout << N_event << '\t' << NRUN << '\t' << NEVENT << '\t' << Teta << '\t'
           << Fi << '\t' << NVD_edep << '\t' << NVD_npe << '\t' << MuBundle
           << '\t' << MuTrackLenNVD << '\t' << nMuNVD << '\t' << eMuNVD / 1000.
           << '\t' << eMuNVD1 / 1000. << '\t' << muDCR << '\t' << muDCRw << '\t'
           << muSM << '\t' << muSMw << '\t' << nSM << '\t' << nSMw << '\t'
           << *fTimer << G4endl;
  }
// simulation

//???   ExpMuTrack.close();
//???   close(otklik);

//======= OneTrack (DECOR SM00_SM01 <-> SM06_SM07)

// 4 vis
#ifdef G4VIS_USE
  delete visManager;
#endif

  delete fTimer;

  delete runManager;

  return 0;
}

float NormDistr(float a, float sigma) {

  G4float g1, g2, norm;

  g1 = G4UniformRand();
  g2 = G4UniformRand();
  norm = sqrt(-2. * log(g1)) * cos(2. * M_PI * g2);
  norm = a + norm * sigma;

  return norm;
}
