//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef NDDPrimaryGeneratorAction_h
#define NDDPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4GeneralParticleSource;
class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NDDPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  NDDPrimaryGeneratorAction();
  ~NDDPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

 private:
  G4GeneralParticleSource* particleGun;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
