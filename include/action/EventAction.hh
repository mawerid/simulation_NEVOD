#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4Timer.hh"
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
  Communicator* communicator_;
  G4bool is_first_event_;

  TFile* output_file_;
  TTree* run_header_tree_;
  TTree* event_tree_;

  EventData event_data_;
};

}  // namespace nevod

#endif  // EVENTACTION_HH
