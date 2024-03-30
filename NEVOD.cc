#include "G4UImanager.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

#include "Randomize.hh"

#include <fcntl.h>
#include <iostream>
#include <unistd.h>

#define G4VIS_USE
// #define G4MULTITHREADED

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

G4String input_file;
size_t input_shift;
G4String output_file;

G4long N_event = 0;
G4int otklik;

G4long runNum = 0;
G4long eventNum = 0;

G4long photoelecNum[600];
G4int PerevKM[600][4];
G4int feuNum;
G4float NVD_edep = 0;
G4float NVD_npe = 0;

G4double theta = 0, phi = 0; // bundle
G4float MuNVD[501][8][2];    // bundle
G4float MuDCR[501][8][8][2];
// G4float nMuNVD;
// G4float MuTrackLenNVD;
// G4float hitSMPLY[8][8]; // bundle new (trig) 04.08.20

G4float EdepCntSCT[80];

// G4float PMDx, PMDy, PMDz, PE, PPx, PPy, PPz, PD; // // massiv, EAS

// G4float nTrackSMX[8], nTrackSMY[8], nTrackSM[8], nTrackSMXw[8],
// nTrackSMYw[8],
//     nTrackSMw[8];
// G4float hSM[8], hSMw[8], nSM, nSMw;
// G4float muSM, muSMw, muDCR, muDCRw;
//
// G4int MuTrackPLX, MuTrackPLY;
// G4int hitSMPLYe, hitSMPLYo;
//
// G4long A, Nfe;
// G4float AmplKSM[7][4][4][6];
// G4double Q;
// G4double amp;
// G4int npl, nstr, nmod, nfeu;
//
// G4float eMuNVD, eMuNVD1;
//
// G4float EdepCntSCT1[9][5][2];

} // namespace NEVOD

using namespace NEVOD;

int main(int argc, char **argv) {

  G4String nabor = argv[1];
  G4String shift = argv[2];

  input_file = std::string("../ot/OTDCR_") + nabor + ".txt";
  input_shift = stoull(shift);
  output_file = std::string("../out/otdcr_") + nabor + "_" + shift + ".dat";

  // Seed the random number generator manually
  CLHEP::HepRandom::setTheEngine(new CLHEP::Ranlux64Engine);

  G4long myseed = 47212; // 345354
  CLHEP::HepRandom::setTheSeed(myseed);

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

  G4VUserPrimaryGeneratorAction *gen_action =
      new PrimaryGeneratorAction(input_file, input_shift);
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

  //  N_event = 0;
  //
  //  runNum = 0;
  //  eventNum = 0;
  //  theta = 0;
  //  phi = 0;

  otklik =
      open(output_file, O_CREAT | O_WRONLY,
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

    runManager->BeamOn(1);

#ifndef G4MULTITHREADED

#endif
  }
#endif

  close(otklik);

// 4 vis
#ifdef G4VIS_USE
  delete visManager;
#endif

  delete runManager;

  return 0;
}