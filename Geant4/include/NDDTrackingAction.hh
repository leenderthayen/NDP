#ifndef NDDTrackingAction_h
#define NDDTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"

class NDDEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NDDTrackingAction : public G4UserTrackingAction {

  public:
    NDDTrackingAction(NDDEventAction*);
   ~NDDTrackingAction();

    virtual void  PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

  private:
    NDDEventAction* eventAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
