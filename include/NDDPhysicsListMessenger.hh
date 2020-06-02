//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef NDDPhysicsListMessenger_h
#define NDDPhysicsListMessenger_h 1

#include "G4SystemOfUnits.hh"
#include "G4UImessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
/*! \class PhysicsListMessenger*/
/*! class derived from G4UImessenger*/
/*! taken from an example*/

class NDDPhysicsList;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NDDPhysicsListMessenger : public G4UImessenger {
 public:
  NDDPhysicsListMessenger(NDDPhysicsList *);
  ~NDDPhysicsListMessenger();

  void SetNewValue(G4UIcommand *, G4String);

 private:
  NDDPhysicsList *pPhysicsList;

  G4UIdirectory *physDir;
  G4UIcmdWithADoubleAndUnit *gammaCutCmd;
  G4UIcmdWithADoubleAndUnit *electCutCmd;
  G4UIcmdWithADoubleAndUnit *protoCutCmd;
  G4UIcmdWithADoubleAndUnit *allCutCmd;
  G4UIcmdWithADoubleAndUnit *pE0Cmd;
  G4UIcmdWithAString *pListCmd;
  G4UIcmdWithAString *pSpectrumCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
