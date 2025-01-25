#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4RandomTools.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserEventAction.hh"
#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"
#include "TTree.h"
#include "control/Communicator.hh"
#include "globals.hh"

namespace nevod {

class RunAction;

class EventAction : public G4UserEventAction {
 public:
  EventAction(RunAction* run_action);
  EventAction(RunAction* run_action, Communicator* communicator);
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

 private:
  Communicator* communicator_ = nullptr;
  G4bool header_processed_ = false;

  TFile* output_file_ = nullptr;
  TTree* run_header_tree_ = nullptr;
  TTree* event_tree_ = nullptr;
  G4int current_epoch_ = 0;

  EventData* event_data_ = nullptr;
};

}  // namespace nevod

#endif  // EVENTACTION_HH
