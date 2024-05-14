#include "G4UImanager.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

#include "Randomize.hh"

#include <iostream>

//#define G4VIS_USE
//  #define G4MULTITHREADED

#define EVENT_COUNT 500
#define SEED 47212 // 345354

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

// shared variables
G4String input_file;
size_t input_shift;

G4int feuCount = 0;
G4long eventCount = 0;
G4int perevKM[546][4];

// thread local variables
G4String output_file;

G4long runNum = 0;
G4long eventNum = 0;

G4long photoelNum[546];
G4double energyDepNEVOD = 0;
G4long particleCountNEVOD = 0;

G4double theta = 0, phi = 0;   // bundle
G4double muonNEVOD[501][8][2]; // bundle
G4double muonDECOR[501][8][8][2];

G4double edepCountSCT[80];

} // namespace NEVOD

using namespace NEVOD;

int main(int argc, char **argv) {

  if (argc >= 3) {
    G4String set = argv[1];
    G4String shift = argv[2];

    input_file = std::string("../input/OTDCR_") + set + ".txt";
    input_shift = stoull(shift);
    output_file = std::string("../output/otdcr_") + set + "_" + shift + ".csv";
  } else {
    throw std::invalid_argument("No set number and shift provided.");
  }

  // Seed the random number generator manually
  CLHEP::HepRandom::setTheEngine(new CLHEP::Ranlux64Engine);

  G4long seed = SEED;
  CLHEP::HepRandom::setTheSeed(seed);

  // Run manager
#ifdef G4MULTITHREADED
  auto *runManager = new G4MTRunManager;
  runManager->SetNumberOfThreads(atoi(argv[3]));
#else
  auto *runManager = new G4RunManager;
#endif

  // UserInitialization classes - mandatory
  auto *detector = new DetectorConstruction;
  runManager->SetUserInitialization(detector);

  auto *physics = new FTFP_BERT; // optical

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
  auto *runAction = new RunAction;
  runManager->SetUserAction(runAction);

  auto *generatorAction = new PrimaryGeneratorAction(input_file, input_shift);
  runManager->SetUserAction(generatorAction);

  auto *eventAction = new EventAction;
  runManager->SetUserAction(eventAction);

  auto *steppingAction = new SteppingAction;
  runManager->SetUserAction(steppingAction);

  // Initialize G4 kernel
  runManager->Initialize();
  // Get the pointer to the UI manager and set verbosities
  auto *UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/run/verbose 0");
  UImanager->ApplyCommand("/event/verbose 0");
  UImanager->ApplyCommand("/tracking/verbose 0");

  // 4 vis
  // Get the pointer to the User Interface manager
#ifdef G4VIS_USE
  if (argc == 1 || argc >= 3) {
    // Define UI session for interactive mode
    auto *ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  } else {
    // Batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }
#endif

  // simulation
#ifndef G4VIS_USE
  for (size_t i = 0; i < EVENT_COUNT; i++) {
    runManager->BeamOn(1);
  }
#endif

// 4 vis
#ifdef G4VIS_USE
  delete visManager;
#endif

  delete runManager;

  return 0;
}