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
{
   timeBirth = timeEnd = 0.;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDTrackingAction::~NDDTrackingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4ParticleDefinition* particle = track->GetDefinition();
  G4String name     = particle->GetParticleName();
  G4double meanLife = particle->GetPDGLifeTime();
  G4double ekin     = track->GetKineticEnergy();
  timeBirth       = track->GetGlobalTime();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDTrackingAction::PostUserTrackingAction(const G4Track* track)
{
 //  G4AnalysisManager* analysis = G4AnalysisManager::Instance();
 //
 //  const G4ParticleDefinition* particle = track->GetParticleDefinition();
 //  G4String name     = particle->GetParticleName();
 //  G4double meanLife = particle->GetPDGLifeTime();
 //  G4double ekin     = track->GetKineticEnergy();
 //  fTimeEnd         = track->GetGlobalTime();
 //  if ((particle->GetPDGStable())&&(ekin == 0.)) fTimeEnd = DBL_MAX;
 //
 //  // count population of ions with meanLife > 0.
 //  if ((G4IonTable::IsIon(particle))&&(meanLife != 0.)) {
 //    G4int id = run->GetIonId(name);
 //    G4double unit = analysis->GetH1Unit(id);
 //    G4double tmin = analysis->GetH1Xmin(id)*unit;
 //    G4double tmax = analysis->GetH1Xmax(id)*unit;
 //    G4double binWidth = analysis->GetH1Width(id)*unit;
 //
 //    G4double t1 = std::max(fTimeBirth,tmin);
 //    G4double t2 = std::min(fTimeEnd  ,tmax);
 //    for (G4double time = t1; time<t2; time+= binWidth)
 //       analysis->FillH1(id,time);
 //  }
 //
 // // keep only emerging particles
 // G4StepStatus status = track->GetStep()->GetPostStepPoint()->GetStepStatus();
 // if (status != fWorldBoundary) return;
 //
 // fEventAction->AddEflow(ekin);
 // run->ParticleFlux(name,ekin);
 //
 // // histograms: energy flow and activities of emerging particles
 //
 // G4int ih1 = 0, ih2 = 0;
 // G4String type   = particle->GetParticleType();
 // G4double charge = particle->GetPDGCharge();
 // G4double time   = track->GetGlobalTime();
 // if (charge > 3.)  {ih1 = 10; ih2 = 20;}
 // else if (particle == G4Gamma::Gamma())       {ih1 = 4;  ih2 = 14;}
 // else if (particle == G4Electron::Electron()) {ih1 = 5;  ih2 = 15;}
 // else if (particle == G4Positron::Positron()) {ih1 = 5;  ih2 = 15;}
 // else if (particle == G4Neutron::Neutron())   {ih1 = 6;  ih2 = 16;}
 // else if (particle == G4Proton::Proton())     {ih1 = 7;  ih2 = 17;}
 // else if (particle == G4Deuteron::Deuteron()) {ih1 = 8;  ih2 = 18;}
 // else if (particle == G4Alpha::Alpha())       {ih1 = 9;  ih2 = 19;}
 // else if (type == "nucleus")                  {ih1 = 10; ih2 = 20;}
 // else if (type == "baryon")                   {ih1 = 11; ih2 = 21;}
 // else if (type == "meson")                    {ih1 = 12; ih2 = 22;}
 // else if (type == "lepton")                   {ih1 = 13; ih2 = 23;};
 // if (ih1 > 0) analysis->FillH1(ih1,ekin);
 // if (ih2 > 0) analysis->FillH1(ih2,time);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
