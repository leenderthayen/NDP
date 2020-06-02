#include "NDDPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDPrimaryGeneratorAction::NDDPrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(), particleGun(0) {
  particleGun = new G4GeneralParticleSource();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDPrimaryGeneratorAction::~NDDPrimaryGeneratorAction() { delete particleGun; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  particleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
