/// \file NDDSiPixelHit.cc
/// \brief Implementation of the NDDSiPixelHit class

#include "NDDSiPixelHit.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<NDDSiPixelHit>* NDDSiPixelHitAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDSiPixelHit::NDDSiPixelHit()
    : G4VHit(),
      trackID(-1),
      enDep(0.),
      pos(G4ThreeVector()),
      momentum(G4ThreeVector()),
      field(G4ThreeVector()),
      particleCode(""),
      time(-1),
      pixelNumber(-1),
      particleName(""),
      pixelName("") {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDSiPixelHit::~NDDSiPixelHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDSiPixelHit::NDDSiPixelHit(const NDDSiPixelHit& right) : G4VHit() {
  trackID = right.trackID;
  enDep = right.enDep;
  pos = right.pos;
  momentum = right.momentum;
  field = right.field;
  particleCode = right.particleCode;
  pixelNumber = right.pixelNumber;
  pixelName = right.pixelName;
  time = right.time;
  particleName = right.particleName;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const NDDSiPixelHit& NDDSiPixelHit::operator=(const NDDSiPixelHit& right) {
  trackID = right.trackID;
  enDep = right.enDep;
  pos = right.pos;
  momentum = right.momentum;
  field = right.field;
  particleCode = right.particleCode;
  pixelNumber = right.pixelNumber;
  pixelName = right.pixelName;
  time = right.time;
  particleName = right.particleName;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int NDDSiPixelHit::operator==(const NDDSiPixelHit& right) const {
  return ((trackID == right.trackID) && (enDep == right.enDep) && (pos == right.pos) && (momentum == right.momentum) && (field == right.field) && (particleCode == right.particleCode) && (particleName == right.particleName) && (pixelNumber == right.pixelNumber) && (pixelName == right.pixelName) && (time == right.time));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDSiPixelHit::Draw() {
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    G4Circle circle(pos);
    circle.SetScreenSize(4.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1., 0., 0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDSiPixelHit::Print() {
  G4cout << "  trackID: " << trackID << "Edep: " << std::setw(7)
         << G4BestUnit(enDep, "Energy") << " Position: " << std::setw(7)
         << G4BestUnit(pos, "Length") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
