#include "NDDTrackingAction.hh"

#include "NDDEventAction.hh"

#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4StepStatus.hh"
#include "G4ParticleTypes.hh"
#include "G4IonTable.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDTrackingAction::NDDTrackingAction(NDDEventAction* event)
:G4UserTrackingAction(), eventAction(event)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDTrackingAction::~NDDTrackingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4ParticleDefinition* particle = track->GetDefinition();
  G4String name = particle->GetParticleName();
  G4double ekin = track->GetKineticEnergy();
  G4double timeBirth = track->GetGlobalTime();
  G4int trackID = track->GetTrackID();
  G4ThreeVector creationPos = track->GetPosition();
  G4String process = track->GetCreatorProcess()->GetProcessName();

  eventAction->AddPrimaryEvent(trackID, ekin, timeBirth, creationPos, process, name);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDTrackingAction::PostUserTrackingAction(const G4Track* track)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
