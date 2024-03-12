#include "NVDEventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"

NVDEventAction::NVDEventAction() = default;

NVDEventAction::~NVDEventAction() = default;

void NVDEventAction::BeginOfEventAction(const G4Event *) {}

void NVDEventAction::EndOfEventAction(const G4Event *) {}
