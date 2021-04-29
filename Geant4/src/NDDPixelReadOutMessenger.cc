#include "NDDPixelReadOut.hh"
#include "NDDPixelReadOutMessenger.hh" // Careful, this include must come after the ReadOut.hh, look into this, fishy

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

NDDPixelReadOutMessenger::NDDPixelReadOutMessenger(NDDPixelReadOut* myRO)
    : pro(myRO) {
  readoutDir = new G4UIdirectory("/NDD/readout/");
  readoutDir->SetGuidance("Commands related to the detector readout");

  pixelRingsCmd = new G4UIcmdWithAnInteger("/NDD/readout/pixelRings", this);
  pixelRingsCmd->SetGuidance(
      "Set the number of pixel rings surrounding the central pixel");
  pixelRingsCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  pixelSizeCmd = new G4UIcmdWithADoubleAndUnit("/NDD/readout/pixelSize", this);
  pixelSizeCmd->SetGuidance("Set the radius of individual Silicon pixels.");
  pixelSizeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  enableDeadLayerCmd = new G4UIcmdWithABool("/NDD/readout/enableDeadLayer", this);
  enableDeadLayerCmd->SetGuidance("Enable/Disable the dead layer in the readout");
  enableDeadLayerCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  deadLayerThicknessCmd = new G4UIcmdWithADoubleAndUnit("NDD/readout/deadLayerThickness", this);
  deadLayerThicknessCmd->SetGuidance("Set the dead layer thickness in front the Silicon detectors.");
  deadLayerThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

NDDPixelReadOutMessenger::~NDDPixelReadOutMessenger() {
  delete readoutDir;
  delete pixelRingsCmd;
  delete enableDeadLayerCmd;
  delete deadLayerThicknessCmd;
}

void NDDPixelReadOutMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == pixelRingsCmd) {
        pro->SetPixelRings(pixelRingsCmd->GetNewIntValue(newValue));
    } else if (command == pixelSizeCmd) {
        pro->SetPixelSize(pixelSizeCmd->GetNewDoubleValue(newValue));
    } else if (command == enableDeadLayerCmd) {
        pro->EnableDeadLayer(enableDeadLayerCmd->GetNewBoolValue(newValue));
    } else if (command == deadLayerThicknessCmd) {
        pro->SetDeadLayerThickness(deadLayerThicknessCmd->GetNewDoubleValue(newValue));
    }
}
