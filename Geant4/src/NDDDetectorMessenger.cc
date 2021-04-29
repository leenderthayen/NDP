#include "NDDDetectorMessenger.hh"
#include "NDDDetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

NDDDetectorMessenger::NDDDetectorMessenger(NDDDetectorConstruction* myDet)
    : detector(myDet) {
  geomDir = new G4UIdirectory("/NDD/geometry/");
  geomDir->SetGuidance("Commands related to the detector geometry");

  sourceIDCmd = new G4UIcmdWithAnInteger("/NDD/geometry/addSourceID", this);
  sourceIDCmd->SetGuidance(
      "Choose the source ID:\n  0: 45Ca 500nm\n  1: 133Ba 12.5um\n  2: 133Ba "
      "500nm quasi-sealed\n  3: 207Bi 5um\n  4: 113Sn, 139Ce 5um");
  sourceIDCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  sourcePosCmd =
      new G4UIcmdWith3VectorAndUnit("/NDD/geometry/addSourcePosition", this);
  sourcePosCmd->SetGuidance(
      "Set the source position of the last given ID. Event generation is automatically confined to "
      "carrier volume.");
  sourcePosCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  detPosCmd =
      new G4UIcmdWith3VectorAndUnit("/NDD/geometry/detectorPosition", this);
  detPosCmd->SetGuidance(
      "Set the detector position of the last given ID.");
  detPosCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  deadLayerThicknessCmd = new G4UIcmdWithADoubleAndUnit("NDD/geometry/deadLayerThickness", this);
  deadLayerThicknessCmd->SetGuidance("Set the dead layer thickness in front the Silicon detectors.");
  deadLayerThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  physicalDeadLayerCmd = new G4UIcmdWithABool("NDD/geometry/physicalDeadLayer", this);
  physicalDeadLayerCmd->SetGuidance("Enable/disable physical dead layer in front the Silicon detector.");
  physicalDeadLayerCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  siThicknessCmd = new G4UIcmdWithADoubleAndUnit("NDD/geometry/siThickness", this);
  siThicknessCmd->SetGuidance("Set the Silicon detector thickness.");
  siThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

NDDDetectorMessenger::~NDDDetectorMessenger() {
  delete geomDir;
  delete sourceIDCmd;
  delete sourcePosCmd;
  delete deadLayerThicknessCmd;
  delete physicalDeadLayerCmd;
  delete siThicknessCmd;
}

void NDDDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == sourceIDCmd) {
        detector->AddSourceID(sourceIDCmd->GetNewIntValue(newValue));
    } else if (command == sourcePosCmd) {
        detector->AddSourcePosition(sourcePosCmd->GetNew3VectorValue(newValue));
    } else if (command == siThicknessCmd) {
        detector->SetSiDetectorThickness(siThicknessCmd->GetNewDoubleValue(newValue));
    } else if (command == detPosCmd) {
        detector->SetDetectorPosition(detPosCmd->GetNew3VectorValue(newValue));
    } else if (command == deadLayerThicknessCmd) {
        detector->SetDeadLayerThickness(deadLayerThicknessCmd->GetNewDoubleValue(newValue));
    } else if (command == physicalDeadLayerCmd) {
        detector->SetBuildDeadLayer(physicalDeadLayerCmd->GetNewBoolValue(newValue));
    }
}
