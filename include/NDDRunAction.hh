//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef NDDRunAction_h
#define NDDRunAction_h 1

#include "G4UserRunAction.hh"

class NDDRunMessenger;
class G4Run;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NDDRunAction : public G4UserRunAction {
 public:
  NDDRunAction();
  virtual ~NDDRunAction();

 public:
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

  void SetRndmFreq(G4int val) { fSaveRndm = val; }
  G4int GetRndmFreq() const { return fSaveRndm; }

 private:
  NDDRunMessenger* runMessenger;
  G4int fSaveRndm;
};

#endif
