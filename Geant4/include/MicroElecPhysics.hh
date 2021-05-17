#ifndef MicroElecPhysics_h
#define MicroElecPhysics_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MicroElecPhysics : public G4VPhysicsConstructor {

public:
  MicroElecPhysics(G4int ver=1, const G4String& name = "MicroElec");

  ~MicroElecPhysics() override;

  void ConstructParticle() override;
  void ConstructProcess() override;

private:
    G4int  verbose;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
