#ifndef NDDDetectorConstruction_h
#define NDDDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"

#include "G4PVParameterised.hh"

#include "G4ThreeVector.hh"

class G4UserLimits;
class NDDDetectorMessenger;

class NDDDetectorConstruction : public G4VUserDetectorConstruction {
 public:
  NDDDetectorConstruction();
  ~NDDDetectorConstruction();

 public:
  virtual G4VPhysicalVolume* Construct();

  inline void AddSourceID(G4int i) { sourceIDs.push_back(i); };
  inline void AddSourcePosition(G4ThreeVector v) { sourcePos.push_back(v); };
  inline void SetDetectorPosition(G4ThreeVector v) { detectorPosition = v;}
  inline void SetDeadLayerThickness(G4double d) { deadLayerThickness = d;}
  inline void SetBuildDeadLayer(G4bool b) { buildPhysicalDeadLayer = b;}
  inline void SetSiDetectorThickness(G4double d) {siThickness = d;}

 private:
  friend class NDDPixelReadOut;

  void BuildWorld();
  void BuildMaterials();
  void BuildSiDetector();
  void BuildSources();
  virtual void ConstructSDandField();
  void BuildVisualisation();

  void SetStepLimits();

  void BuildSource(G4int, G4ThreeVector);

  G4ThreeVector detectorPosition;

  NDDDetectorMessenger* detMess;

  G4bool buildPhysicalDeadLayer;

  G4double siThickness;
  G4double siBackingThickness;
  G4double siOuterRadius;
  G4double carrierThickness;
  G4double mylarThickness;
  G4double deadLayerThickness;
  G4double carrierRadius;
  G4double sourceRadius;
  G4double stepSize;

  std::vector<G4int> sourceIDs;
  std::vector<G4ThreeVector> sourcePos;

  G4Material* airMaterial;
  G4Material* vacuumMaterial;
  G4Material* siliconMaterial;
  G4Material* tinMaterial;
  G4Material* copperMaterial;
  G4Material* stainlessSteelMaterial;
  G4Material* aluminiumMaterial;
  G4Material* aluminaMaterial;
  G4Material* mylarMaterial;
  G4Material* kaptonMaterial;
  G4Material* sixFsixFMaterial;
  G4Material* CaCl2Material;
  G4Material* bariumMaterial;
  G4Material* bismuthMaterial;
  G4Material* pmmaMaterial;
  G4Material* berylliumMaterial;
  G4Material* germaniumMaterial;
  G4Material* waterMaterial;

  G4Box* solidWorld;
  G4LogicalVolume* logicalWorld;
  G4VPhysicalVolume* physicalWorld;

  G4Tubs* solidSilicon;
  G4Tubs* solidFoil;
  G4LogicalVolume* logicalSilicon;
  G4LogicalVolume* logicalFoil;
  G4VPhysicalVolume* physicalSilicon;
  G4VPhysicalVolume* physicalFoil;

  G4Tubs* solidBacking;
  G4LogicalVolume* logicalBacking;
  G4VPhysicalVolume* physicalBacking;

  G4Tubs* solidDead;
  G4LogicalVolume* logicalDead;
  G4VPhysicalVolume* physicalDead;

  G4Tubs* solidSourceHolder;
  G4LogicalVolume* logicalSourceHolder;
  G4VPhysicalVolume* physicalSourceHolder;

  G4Tubs* solidSourceRegion;
  G4LogicalVolume* logicalSourceRegion;
  G4VPhysicalVolume* physicalSourceRegion;

  G4Tubs* solidCarrier;
  G4LogicalVolume* logicalCarrier;
  G4VPhysicalVolume* physicalCarrier;

  G4UserLimits* stepLimitMyl;
  G4UserLimits* stepLimitDead;
  G4UserLimits* stepLimitCar;
};

#endif
