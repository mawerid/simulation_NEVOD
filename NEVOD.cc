#include "G4UImanager.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

#include "G4Timer.hh"
#include "Randomize.hh"

#include <cmath>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

// #define G4VIS_USE
#define G4MULTITHREADED

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#ifdef G4VIS_USE
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#endif

namespace NEVOD {

G4long N_event;
G4int otklik;
std::ifstream ExpMuTrack;

G4float NRUN;
G4float NEVENT;

G4long NumPhotEl[600];
G4int PerevKM[600][4];
G4int Nfeu;
G4float NVD_edep;
G4float NVD_npe;

G4float Teta, Fi;         // bundle
G4int MuBundle;           // bundle
G4float MuNVD[501][8][2]; // bundle
G4float MuDCR[501][8][8][2];
G4float nMuNVD;
G4float MuTrackLenNVD;
G4float hitSMPLY[8][8]; // bundle new (trig) 04.08.20

G4float EdepCntSCT[80];

G4float PMDx, PMDy, PMDz, PE, PPx, PPy, PPz, PD; // // massiv, EAS

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

} // namespace NEVOD

using namespace NEVOD;

int main(int argc, char **argv) {
  int nabor;
  char cfile[512], gfile[512];

  N_event = 0;
  NRUN = 0;
  NEVENT = 0;
  Teta = 0;
  Fi = 0;

  NVD_edep = 0;
  NVD_npe = 0;
  MuBundle = 0.;
  MuTrackLenNVD = 0;
  nMuNVD = 0;

  PMDx = 0.;
  PMDy = 0.;
  PMDz = 1.;
  PE = 100.; // GeV
  PPx = 0.;
  PPy = 0.;
  PPz = 13.5;
  PD = 5;

  for (long &i : NumPhotEl) {
    i = 0;
  }

  for (double &i : Kv_Eff) {
    i = i / 2.65;
  }

  // Seed the random number generator manually
  CLHEP::HepRandom::setTheEngine(new CLHEP::Ranlux64Engine);

  G4long myseed = 47212; // 345354
  CLHEP::HepRandom::setTheSeed(myseed);

  G4Timer *fTimer;
  fTimer = new G4Timer();

  // Run manager
#ifdef G4MULTITHREADED
  auto *runManager = new G4MTRunManager;
  runManager->SetNumberOfThreads(atoi(argv[3]));
#else
  auto *runManager = new G4RunManager;
#endif

  // UserInitialization classes - mandatory
  G4VUserDetectorConstruction *detector = new DetectorConstruction;
  runManager->SetUserInitialization(detector);

  G4VModularPhysicsList *physics = new FTFP_BERT; // optical

#ifndef G4VIS_USE
  auto *opticalPhysics = new G4OpticalPhysics;
  physics->RegisterPhysics(opticalPhysics);
#endif

  runManager->SetUserInitialization(physics);

// 4 vis
#ifdef G4VIS_USE
  // visualization manager
  G4VisManager *visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  // UserAction classes
  G4UserRunAction *run_action = new RunAction;
  runManager->SetUserAction(run_action);

  G4VUserPrimaryGeneratorAction *gen_action = new PrimaryGeneratorAction;
  runManager->SetUserAction(gen_action);

  G4UserEventAction *event_action = new EventAction;
  runManager->SetUserAction(event_action);

  G4UserSteppingAction *stepping_action = new SteppingAction;
  runManager->SetUserAction(stepping_action);

  // Initialize G4 kernel
  runManager->Initialize();
  // Get the pointer to the UI manager and set verbosities
  G4UImanager *UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/run/verbose 0");
  UImanager->ApplyCommand("/event/verbose 0");
  UImanager->ApplyCommand("/tracking/verbose 0");

  G4float SM1x, SM1y, SM1z, SM2x, SM2y, SM2z;

  N_event = 0;

  NRUN = 0;
  NEVENT = 0;
  Teta = 0;
  Fi = 0;

  nabor = atoi(argv[1]);
  int shift;
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

  // 4 vis
  // Get the pointer to the User Interface manager
#ifdef G4VIS_USE
  if (argc == 1 || argc == 3) // Define UI session for interactive mode
  {
    auto *ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  } else // Batch mode
  {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }
#endif

  // simulation
#ifndef G4VIS_USE
  for (int nevt = 0; nevt < 500; nevt++) {
    fTimer->Start();

    runManager->BeamOn(1);

    fTimer->Stop();

#ifndef G4MULTITHREADED
    G4cout << N_event << '\t' << Teta << '\t' << Fi << '\t' << NVD_edep << '\t'
           << NVD_npe << '\t' << MuBundle << '\t' << MuTrackLenNVD << '\t'
           << nMuNVD << '\t' << eMuNVD / 1000. << '\t' << eMuNVD1 / 1000.
           << '\t' << muDCR << '\t' << muDCRw << '\t' << muSM << '\t' << muSMw
           << '\t' << nSM << '\t' << nSMw << '\t' << *fTimer << G4endl;
#endif
  }
#endif

  ExpMuTrack.close();
  close(otklik);

// 4 vis
#ifdef G4VIS_USE
  delete visManager;
#endif

  delete fTimer;

  delete runManager;

  return 0;
}