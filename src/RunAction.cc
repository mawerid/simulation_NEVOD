#include "RunAction.hh"
#include "G4Run.hh"

namespace NEVOD {

RunAction::RunAction() = default;

RunAction::~RunAction() = default;

void RunAction::BeginOfRunAction(const G4Run *aRun) {}

void RunAction::EndOfRunAction(const G4Run *aRun) {}

} // namespace NEVOD