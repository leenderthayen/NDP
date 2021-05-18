//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "NDDRunAction.hh"
//#include "NDDRunMessenger.hh"

#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"

#include "Randomize.hh"

#include "NDDAnalysisManager.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDRunAction::NDDRunAction(NDDAnalysisManager* anMan)
    : G4UserRunAction(), analysisManager(anMan)/*, runMessenger(0)*/ {
  //runMessenger = new NDDRunMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDRunAction::~NDDRunAction() {
  //delete runMessenger;
  delete analysisManager;
  delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDRunAction::BeginOfRunAction(const G4Run*) {

  analysisManager->Book();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDRunAction::EndOfRunAction(const G4Run*) {
  analysisManager->Save();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
