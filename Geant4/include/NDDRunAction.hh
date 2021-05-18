//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef NDDRunAction_h
#define NDDRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4String.hh"

//class NDDRunMessenger;
class NDDAnalysisManager;
class G4Run;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NDDRunAction : public G4UserRunAction {
 public:
  NDDRunAction(NDDAnalysisManager*);
  virtual ~NDDRunAction();

 public:
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

 private:
  //NDDRunMessenger* runMessenger;
  NDDAnalysisManager* analysisManager;
};

#endif
