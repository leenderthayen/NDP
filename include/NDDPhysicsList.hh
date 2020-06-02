//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef NDDPhysicsList_h
#define NDDPhysicsList_h 1

#include "G4VModularPhysicsList.hh"

class NDDPhysicsListMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NDDPhysicsList : public G4VModularPhysicsList {
 public:
  NDDPhysicsList();
  virtual ~NDDPhysicsList();

  void ConstructParticle();

  void SetCuts();
  void SetCutForGamma(G4double);
  void SetCutForElectron(G4double);
  void SetCutForPositron(G4double);

  void ReplaceEMPhysicsList(const G4String& name);

 private:
  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;

  NDDPhysicsListMessenger* pMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
