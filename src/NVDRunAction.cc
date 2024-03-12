#include "NVDRunAction.hh"
#include "G4Run.hh"
#include "G4Timer.hh"

NVDRunAction::NVDRunAction() {
  //   timer = new G4Timer;
}

NVDRunAction::~NVDRunAction() {
  //   delete timer;
}

void NVDRunAction::BeginOfRunAction(const G4Run *aRun) {
  //   G4cout << "### RUN " << aRun->GetRunID() << " start" << G4endl;
  //   timer->Start();
}

void NVDRunAction::EndOfRunAction(const G4Run *aRun) {
  /*
   extern G4float nRunN, nEvtN;
   extern G4long N_event;
   extern G4float NVD_edep, NVD_npe, NVD_totnpe, NVD_edep1;
   extern G4int MuBundle;
   extern G4float Teta, Fi;
   extern G4float MuTrackLenNVD, nMuNVD, nMuSM[8], nHitSM, nHitSMwat, nMuDCR,
  nMuDCRwat;

     timer->Stop();

  //   G4cout << "RUN " << aRun->GetRunID() << " " << *timer << G4endl;
  //   G4cout << "Event = " << aRun->GetNumberOfEvent() << " " << *timer <<
  G4endl;

  //   G4cout << N_event << '\t' << nRunN << '\t' << NVD_npe << '\t' << NVD_edep
  << '\t' << *timer << G4endl; // cascade

  // bundle
     G4cout << N_event << '\t' << nRunN << '\t' << nEvtN << '\t'
  //          << NVD_edep << '\t' << NVD_edep1 << '\t' << NVD_npe << '\t' <<
  NVD_totnpe << '\t'
            << NVD_edep << '\t' << NVD_npe << '\t'
  //          << Teta << '\t' << Fi << '\t'
            << MuBundle << '\t' << MuTrackLenNVD << '\t' << nMuNVD << '\t'
            << nMuDCR << '\t' << nMuDCRwat << '\t' << nHitSM << '\t' <<
  nHitSMwat << '\t'
            << *timer << G4endl;
  // bundle
  */
  //   G4cout << "RUN " << aRun->GetRunID() << " " << *timer << G4endl;
}