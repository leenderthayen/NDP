#include "NDDSteppingAction.hh"

#include "NDDDetectorConstruction.hh"
#include "NDDEventAction.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"

#include "G4RunManager.hh"

#include <iostream>

NDDSteppingAction::NDDSteppingAction(NDDEventAction *ea) : eventAction(ea) {}

NDDSteppingAction::~NDDSteppingAction() {}

void NDDSteppingAction::UserSteppingAction(const G4Step *aStep) {
  const G4String particleName =
      aStep->GetTrack()->GetDefinition()->GetParticleName();

  G4VPhysicalVolume *pVol =
      aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  G4VPhysicalVolume *pVolPost =
      aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume();

  if (pVol && particleName == "e-") {
    eventAction->AddVisitedVolume(aStep->GetPreStepPoint()->GetTotalEnergy(),
                                  aStep->GetPreStepPoint()->GetGlobalTime(),
                                  pVol->GetName());
  }

  // if (pVol->GetName() == "Dead") {
  //   eventAction->AddDepositedEnDead(aStep->GetTotalEnergyDeposit());
  // } else if (pVol->GetName() == "EastFoil" || pVol->GetName() == "WestFoil") {
  //   eventAction->AddDepositedEnFoil(aStep->GetTotalEnergyDeposit());
  //   eventAction->SetFoilPoE(aStep->GetPreStepPoint()->GetPosition());
  // } else if (pVol->GetName() == "Carrier") {
  //   eventAction->AddDepositedEnCarrier(aStep->GetTotalEnergyDeposit());
  // } else if (pVol->GetName() == "SourceHolder") {
  //   eventAction->AddDepositedEnSourceHolder(aStep->GetTotalEnergyDeposit());
  // } else if (particleName == "gamma" &&
  //            pVolPost->GetName() == "World") {
  //   if (aStep->GetTrack()->GetCreatorProcess() &&
  //       aStep->GetTrack()->GetCreatorProcess()->GetProcessName() == "eBrem") {
  //       eventAction->AddBremsstrahlungLoss(aStep->GetPreStepPoint()->GetTotalEnergy());
  //     aStep->GetTrack()->SetTrackStatus(fStopAndKill);
  //   }
  // }
  //
  // if (particleName == "e-") {
  //   if (pVol->GetName() == "Dead" && pVolPost->GetName() == "World") {
  //     eventAction->SetAngleOut(
  //         aStep->GetPostStepPoint()->GetMomentum().theta());
  //   } else if (pVol->GetName() == "Carrier" || pVol->GetName() == "EastFoil" ||
  //              pVol->GetName() == "WestFoil") {
  //     if (pVolPost->GetName() == "World" &&
  //         aStep->IsFirstStepInVolume()) {
  //       eventAction->SetAngleSourceOut(
  //           aStep->GetPostStepPoint()->GetMomentum().theta());
  //     }
  //   }
  // }
}
