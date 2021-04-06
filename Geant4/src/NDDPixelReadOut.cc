#include "NDDPixelReadOut.hh"
#include "NDDSiPixelSD.hh"

#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4Polyhedra.hh"
#include "G4Tubs.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

NDDPixelReadOut::NDDPixelReadOut(G4String& parallelWorldName) : G4VUserParallelWorld(parallelWorldName) {}

NDDPixelReadOut::~NDDPixelReadOut() {}

void NDDPixelReadOut::Construct() {
  G4double siThickness = 2 * mm;
  G4double siOuterRadius = 7.5 * cm;
  //G4double deadLayerThickness = 100. * nm;
  G4double deadLayerThickness = 0. * nm; //Removed the dead layer

  G4Material* dummyMat = nullptr;

  //TODO not hardcoded
  G4double xSilicon = 0;
  G4double ySilicon = 0;
  G4double zSilicon = 10 * mm + (siThickness + deadLayerThickness) / 2;

  G4VPhysicalVolume* physicalROWorld = GetWorld();
  G4LogicalVolume* logicalROWorld = physicalROWorld->GetLogicalVolume();

  G4VSolid* solidROSilicon = new G4Tubs("solidROSilicon", 0., siOuterRadius,
                                (siThickness + deadLayerThickness) / 2., 0., 360. * deg);
  G4LogicalVolume* logicalROSilicon = new G4LogicalVolume(solidROSilicon, dummyMat,
                                           "logicalROSilicon");
  new G4PVPlacement(
      0,
      G4ThreeVector(xSilicon, ySilicon, zSilicon),
      logicalROSilicon, "physicalROSilicon", logicalROWorld, false, 0);

  G4double pixelSize = 7 * mm;

  G4double zPlanes[2] = {0., siThickness + deadLayerThickness};
  G4double rInner[2] = {0., 0.};
  G4double rOuter[2] = {pixelSize / 2.0, pixelSize / 2.0};

  G4VSolid* solidPixel = new G4Polyhedra("solidROPixel", 0, 360. * deg, 6, 2,
                                   zPlanes, rInner, rOuter);
  G4LogicalVolume* logicalPixel =
      new G4LogicalVolume(solidPixel, dummyMat, "logicalROPixel");

  G4int pixelRings = 6;
  G4int columns = 2 * pixelRings + 1;

  //copy number
  G4int cn = 1;
  for (int column = 1; column <= columns; column++) {
    for (int row = std::abs((columns + 1) / 2 - column) / 2 - (column) % 2;
         row < columns - std::abs((columns + 1) / 2 - column) / 2 - 1; row++) {
      new G4PVPlacement(
          0, G4ThreeVector(-((columns + 1) / 2 - column) * pixelSize *
                               std::cos(M_PI / 6.0),
                           ((columns + 1) / 2 - row +
                            std::abs((columns + 1) / 2 - column) % 2 / 2. - 2) *
                               pixelSize,
                           -(siThickness + deadLayerThickness)/ 2.0),
          logicalPixel, "SiROPixel", logicalROSilicon, false, cn); //Added missing variable deadLayerThickness to the Z-coordinate calculation
      cn++;
    }
  }
}

void NDDPixelReadOut::ConstructSD() {
  //------------------------------------------------------------------
  // Sensitive Detector
  //------------------------------------------------------------------
  G4String pixelSDname = "/NND/SiPixel";
  G4String hitsCollectionName = "SiPixelHitCollection";
  NDDSiPixelSD * pixelSD = new NDDSiPixelSD(pixelSDname, hitsCollectionName);
  G4SDManager::GetSDMpointer()->AddNewDetector(pixelSD);
  SetSensitiveDetector("logicalROPixel", pixelSD);
}
