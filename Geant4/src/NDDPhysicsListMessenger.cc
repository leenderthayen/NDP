#include "NDDPhysicsListMessenger.hh"
#include "NDDPhysicsList.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDPhysicsListMessenger::NDDPhysicsListMessenger(NDDPhysicsList* pPhys)
    : pPhysicsList(pPhys) {
  physDir = new G4UIdirectory("/NDD/phys/");
  physDir->SetGuidance("NDD physics list commands");

  gammaCutCmd = new G4UIcmdWithADoubleAndUnit("/NDD/phys/setGCut", this);
  gammaCutCmd->SetGuidance("Set gamma cut.");
  gammaCutCmd->SetParameterName("Gcut", false);
  gammaCutCmd->SetUnitCategory("Length");
  gammaCutCmd->SetRange("Gcut>0.0");
  gammaCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  electCutCmd = new G4UIcmdWithADoubleAndUnit("/NDD/phys/setECut", this);
  electCutCmd->SetGuidance("Set electron cut.");
  electCutCmd->SetParameterName("Ecut", false);
  electCutCmd->SetUnitCategory("Length");
  electCutCmd->SetRange("Ecut>0.0");
  electCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  protoCutCmd = new G4UIcmdWithADoubleAndUnit("/NDD/phys/setPCut", this);
  protoCutCmd->SetGuidance("Set positron cut.");
  protoCutCmd->SetParameterName("Pcut", false);
  protoCutCmd->SetUnitCategory("Length");
  protoCutCmd->SetRange("Pcut>0.0");
  protoCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  allCutCmd = new G4UIcmdWithADoubleAndUnit("/NDD/phys/setCuts", this);
  allCutCmd->SetGuidance("Set cut for all.");
  allCutCmd->SetParameterName("cut", false);
  allCutCmd->SetUnitCategory("Length");
  allCutCmd->SetRange("cut>0.0");
  allCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  pListCmd = new G4UIcmdWithAString("/NDD/phys/addPhysics", this);
  pListCmd->SetGuidance("Add modula physics list.");
  pListCmd->SetParameterName("PList", false);
  pListCmd->AvailableForStates(G4State_PreInit);
  pListCmd->SetCandidates(
      "emstandard_opt0 emstandard_opt1 emstandard_opt2 emstandard_opt3 emstandard_opt4 "
      "standardSS standardGS standardWVI emlivermore empenelope "
      "DNA DNA_opt2 DNA_opt4 DNA_opt6");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDPhysicsListMessenger::~NDDPhysicsListMessenger() {
  delete gammaCutCmd;
  delete electCutCmd;
  delete protoCutCmd;
  delete allCutCmd;
  delete pListCmd;
  delete physDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDPhysicsListMessenger::SetNewValue(G4UIcommand* command,
                                       G4String newValue) {
  if (command == gammaCutCmd) {
    pPhysicsList->SetCutForGamma(gammaCutCmd->GetNewDoubleValue(newValue));
  }

  else if (command == electCutCmd) {
    pPhysicsList->SetCutForElectron(electCutCmd->GetNewDoubleValue(newValue));
  }

  else if (command == protoCutCmd) {
    pPhysicsList->SetCutForPositron(protoCutCmd->GetNewDoubleValue(newValue));
  }

  else if (command == allCutCmd) {
    G4double cut = allCutCmd->GetNewDoubleValue(newValue);
    pPhysicsList->SetCutForGamma(cut);
    pPhysicsList->SetCutForElectron(cut);
    pPhysicsList->SetCutForPositron(cut);
  }

  else if (command == pListCmd) {
    pPhysicsList->ReplaceEMPhysicsList(newValue);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
