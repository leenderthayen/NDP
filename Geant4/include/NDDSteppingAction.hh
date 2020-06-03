#ifndef NDDSteppingAction_h
#define NDDSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class NDDEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NDDSteppingAction : public G4UserSteppingAction {
 public:
  NDDSteppingAction(NDDEventAction*);
  ~NDDSteppingAction();

  void UserSteppingAction(const G4Step*);

 private:
  NDDEventAction* eventAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
