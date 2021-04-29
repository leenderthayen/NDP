#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class NDDDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;

class NDDDetectorMessenger : public G4UImessenger {
 public:
  NDDDetectorMessenger(NDDDetectorConstruction*);
  ~NDDDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);

 private:
  NDDDetectorConstruction* detector;

  G4UIdirectory* geomDir;
  G4UIdirectory* sourceDir;
  G4UIcmdWith3VectorAndUnit* sourcePosCmd;
  G4UIcmdWith3VectorAndUnit* detPosCmd;
  G4UIcmdWithAnInteger* sourceIDCmd;
  G4UIcmdWithADoubleAndUnit* siThicknessCmd;
  G4UIcmdWithADoubleAndUnit* mylarThickCmd;
  G4UIcmdWithAnInteger* buildBackCmd;
};

#endif
