#include "NDDSiPixelSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4FieldManager.hh"
#include "G4Field.hh"
#include "G4TouchableHistory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDSiPixelSD::NDDSiPixelSD(const G4String& name, const G4String& hitsCollectionName)
    : G4VSensitiveDetector(name), fHitsCollection(nullptr) {
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDSiPixelSD::~NDDSiPixelSD() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDSiPixelSD::Initialize(G4HCofThisEvent* hce) {
  // Create hits collection

  fHitsCollection =
      new NDDSiPixelHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce

  G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hcID, fHitsCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool NDDSiPixelSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  // energy deposit
  G4double enDep = aStep->GetTotalEnergyDeposit();

  if (enDep == 0.) return false;

  NDDSiPixelHit* newHit = new NDDSiPixelHit();

  G4VPhysicalVolume* pVolPre =
      aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

  G4ThreeVector pos = aStep->GetPreStepPoint()->GetPosition();


  G4ThreeVector siPos = pVolPre->GetObjectTranslation();
  if (pVolPre->GetName() != "physicalSilicon") {
      G4VPhysicalVolume* pVolPost = aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
      if (pVolPost) {
        siPos = pVolPost->GetObjectTranslation();
      }
  }

  newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
  newHit->SetEnDep(enDep);
  newHit->SetPos(G4ThreeVector(pos.x(), pos.y(), pos.z() - siPos.z())); // Probably offset by half the pixel width
  newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
  newHit->SetMomentum(aStep->GetPreStepPoint()->GetMomentum());
  newHit->SetParticleCode(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
  newHit->SetParticleName(aStep->GetTrack()->GetDefinition()->GetParticleName());

  G4TouchableHistory* theTouchable =
      (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  newHit->SetPixelNumber(theTouchable->GetVolume()->GetCopyNo());
  newHit->SetPixelName(theTouchable->GetVolume()->GetName());

  fHitsCollection->insert(newHit);

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDSiPixelSD::EndOfEvent(G4HCofThisEvent* hce) {
  G4int nofHits = fHitsCollection->entries();
  if (verboseLevel > 1) {
    G4cout << G4endl << "-------->Hits Collection: in this event they are "
           << nofHits << " hits in the tracker chambers: " << G4endl;
  }

  G4int hciD = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hciD, fHitsCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
