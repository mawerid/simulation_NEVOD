#include <iostream>

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4RunManagerFactory.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"
#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"
#include "TTree.h"
#include "action/ActionInitialization.hh"
#include "control/Communicator.hh"
#include "control/InputManager.hh"
#include "detector/DetectorConstruction.hh"

#define EVENT_COUNT 1000

int main(int argc, char** argv) {
  ROOT::EnableThreadSafety();

  if (argc < 2) throw std::invalid_argument("No configuration file provided");

  auto* communicator = new nevod::Communicator(argv[1]);
  communicator->PrintStartMessage();
  auto params = communicator->GetSimulationParams();

  // Seed the random number generator manually
  CLHEP::HepRandom::setTheEngine(new CLHEP::Ranlux64Engine);
  CLHEP::HepRandom::setTheSeed(params.seed);

  // Run manager
#ifdef G4MULTITHREADED
  auto* run_manager = new G4MTRunManager;
#else
  auto* run_manager = new G4RunManager;
#endif

  run_manager->SetVerboseLevel(params.verbose ? 1 : 0);

  auto* ui_manager = G4UImanager::GetUIpointer();
  ui_manager->ApplyCommand("/control/cout/ignoreThreadsExcept 1000000");
  ui_manager->ApplyCommand("/process/em/verbose 0");
  ui_manager->ApplyCommand("/process/had/verbose 0");

  // UserInitialization classes - mandatory
  run_manager->SetUserInitialization(new nevod::DetectorConstruction(communicator));

  auto* physics_list = new FTFP_BERT;  // optical

  if (!params.use_ui) physics_list->RegisterPhysics(new G4OpticalPhysics);

  run_manager->SetUserInitialization(physics_list);

  auto* input_manager = new nevod::InputManager(communicator, params.initial_offset);
  input_manager->DetectFiles(params.input_path);

  if (input_manager->GetFilesNumber() == 0) throw std::invalid_argument("No files found in the input directory");

  run_manager->SetUserInitialization(new nevod::ActionInitialization(communicator, input_manager));

  G4UIExecutive* ui_executive = nullptr;
  G4VisManager* vis_manager = nullptr;

  if (params.use_ui) {
    vis_manager = new G4VisExecutive;
    vis_manager->Initialize();
    ui_executive = new G4UIExecutive(argc, argv);
    ui_executive->SessionStart();
    delete ui_executive;
  } else {
    run_manager->SetVerboseLevel(0);
#if defined(G4MULTITHREADED)
    run_manager->SetNumberOfThreads(params.thread_num);
    run_manager->SetEventModulo(params.epoch_num);
#endif
    run_manager->Initialize();

    G4int total_events_count = params.epoch_num * input_manager->GetFilesNumber();

    communicator->SetTotalEventCount(total_events_count);
    G4cout << "Events to simulate: " << total_events_count << G4endl;
    run_manager->BeamOn(total_events_count);
  }

  // // Initialize G4 kernel
  // run_manager->Initialize();
  // // Get the pointer to the UI manager and set verbosities
  // auto* UImanager = G4UImanager::GetUIpointer();
  // UImanager->ApplyCommand("/run/verbose 0");
  // UImanager->ApplyCommand("/event/verbose 0");
  // UImanager->ApplyCommand("/tracking/verbose 0");

  // 4 vis
  // Get the pointer to the User Interface manager
  // #ifdef G4VIS_USE
  //   if (argc == 1 || argc >= 3) {
  //     // Define UI session for interactive mode
  //     auto* ui = new G4UIExecutive(argc, argv);
  //     UImanager->ApplyCommand("/control/execute vis.mac");
  //     ui->SessionStart();
  //     delete ui;
  //   } else {
  //     // Batch mode
  //     G4String command = "/control/execute ";
  //     G4String fileName = argv[1];
  //     UImanager->ApplyCommand(command + fileName);
  //   }
  // #endif

  // simulation
  // #ifndef G4VIS_USE
  //   for (size_t i = 0; i < EVENT_COUNT; i++) {
  //     run_manager->BeamOn(1);
  //   }
  // #endif

  communicator->MergeOutputFiles();
  communicator->PrintEndMessage();

  delete vis_manager;
  delete communicator;
  delete input_manager;
  delete run_manager;

  return 0;
}