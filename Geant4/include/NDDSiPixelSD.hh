/// \file NDDSiPixelSD.hh
/// \brief Definition of the NDDSiPixelSD class

#ifndef NDDSiPixelSD_h
#define NDDSiPixelSD_h 1

#include "NDDSiPixelHit.hh"

#include "G4VSensitiveDetector.hh"
#include "G4ThreeVector.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// SiPixel sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero
/// energy deposit.

class NDDSiPixelSD : public G4VSensitiveDetector {
 public:
  NDDSiPixelSD(const G4String& name, const G4String& hitsCollectionName);
  virtual ~NDDSiPixelSD();

  // methods from base class
  virtual void Initialize(G4HCofThisEvent* hitCollection);
  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent* hitCollection);

 private:
  NDDSiPixelHitsCollection* fHitsCollection;
  G4ThreeVector initPos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
