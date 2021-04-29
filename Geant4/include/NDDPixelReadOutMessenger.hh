#ifndef NDDPixelReadOutMessenger_h
#define NDDPixelReadOutMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class NDDPixelReadOut;
class G4UIdirectory;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;

class NDDPixelReadOutMessenger : public G4UImessenger {
 public:
  NDDPixelReadOutMessenger(NDDPixelReadOut*);
  ~NDDPixelReadOutMessenger();

  void SetNewValue(G4UIcommand*, G4String);

 private:
  NDDPixelReadOut* pro;

  G4UIdirectory* readoutDir;
  G4UIcmdWithAnInteger* pixelRingsCmd;
  G4UIcmdWithABool* enableDeadLayerCmd;
  G4UIcmdWithADoubleAndUnit* pixelSizeCmd;
};

#endif
