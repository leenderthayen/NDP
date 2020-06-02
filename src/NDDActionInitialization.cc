#include "NDDActionInitialization.hh"
#include "NDDPrimaryGeneratorAction.hh"
#include "NDDRunAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDActionInitialization::NDDActionInitialization() : G4VUserActionInitialization() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDActionInitialization::~NDDActionInitialization() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDActionInitialization::BuildForMaster() const {
  SetUserAction(new NDDRunAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDActionInitialization::Build() const {
  SetUserAction(new NDDPrimaryGeneratorAction);

  SetUserAction(new NDDRunAction);
}
