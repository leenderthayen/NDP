//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef RunMessenger_h
#define RunMessenger_h 1

#include "G4UImessenger.hh"

class NDDRunAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NDDRunMessenger : public G4UImessenger {
 public:
  NDDRunMessenger(NDDRunAction*);
  virtual ~NDDRunMessenger();

  virtual void SetNewValue(G4UIcommand*, G4String);

 private:
  NDDRunAction* fRunAction;

  G4UIdirectory* randomDir;

  G4UIcmdWithAnInteger* randomSaveCmd;
  G4UIcmdWithAString* randomReadCmd;
};

#endif
