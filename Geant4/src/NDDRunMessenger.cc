//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "NDDRunMessenger.hh"
#include "NDDRunAction.hh"

#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDRunMessenger::NDDRunMessenger(NDDRunAction* action)
    : G4UImessenger(),
      fRunAction(action),
      randomDir(0),
      randomSaveCmd(0),
      randomReadCmd(0) {
  randomDir = new G4UIdirectory("/rndm/");
  randomDir->SetGuidance("Rndm status control.");

  randomSaveCmd = new G4UIcmdWithAnInteger("/rndm/save", this);
  randomSaveCmd->SetGuidance(
      "set frequency to save rndm status on external files.");
  randomSaveCmd->SetGuidance("freq = 0 not saved");
  randomSaveCmd->SetGuidance("freq > 0 saved on: beginOfRun.rndm");
  randomSaveCmd->SetGuidance("freq = 1 saved on:   endOfRun.rndm");
  randomSaveCmd->SetGuidance("freq = 2 saved on: endOfEvent.rndm");
  randomSaveCmd->SetParameterName("frequency", false);
  randomSaveCmd->SetRange("frequency>=0 && frequency<=2");
  randomSaveCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  randomReadCmd = new G4UIcmdWithAString("/rndm/read", this);
  randomReadCmd->SetGuidance("get rndm status from an external file.");
  randomReadCmd->SetParameterName("fileName", true);
  randomReadCmd->SetDefaultValue("beginOfRun.rndm");
  randomReadCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDRunMessenger::~NDDRunMessenger() {
  delete randomSaveCmd;
  delete randomReadCmd;
  delete randomDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDRunMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
  if (command == randomSaveCmd)
    fRunAction->SetRndmFreq(randomSaveCmd->GetNewIntValue(newValues));

  if (command == randomReadCmd) {
    G4cout << "\n---> rndm status restored from file: " << newValues << G4endl;
    G4Random::restoreEngineStatus(newValues);
    G4Random::showEngineStatus();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
