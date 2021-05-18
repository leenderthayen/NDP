#include "NDDActionInitialization.hh"
#include "NDDPrimaryGeneratorAction.hh"
#include "NDDRunAction.hh"
#include "NDDEventAction.hh"
#include "NDDSteppingAction.hh"
#include "NDDTrackingAction.hh"
#include "NDDAnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDActionInitialization::NDDActionInitialization() : G4VUserActionInitialization() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDActionInitialization::~NDDActionInitialization() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDActionInitialization::BuildForMaster() const {
  NDDAnalysisManager* analysisManager = new NDDAnalysisManager();
  SetUserAction(new NDDRunAction(analysisManager));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDActionInitialization::Build() const {
  SetUserAction(new NDDPrimaryGeneratorAction);

  NDDAnalysisManager* analysisManager = new NDDAnalysisManager();

  SetUserAction(new NDDRunAction(analysisManager));

  NDDEventAction* eventAction = new NDDEventAction(analysisManager);
  SetUserAction(eventAction);

  SetUserAction(new NDDSteppingAction(eventAction));
  SetUserAction(new NDDTrackingAction(eventAction));
}
