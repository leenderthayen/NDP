#ifndef NDDDetectorConstruction_h
#define NDDDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Polyhedra.hh"
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
  inline void SetSiDetectorThickness(G4double d) {siThickness = d;}
  inline void SetMylarThickness(G4double d) {mylarThickness = d;}
  inline void SetAlumThickness(G4double d) {AlumThickness = d;}
  inline void SetBackingConfig(G4int r) { backingConfig = r;}

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

  G4double siThickness;
  G4double siBackingThickness;
  G4double siOuterRadius;
  G4double carrierThickness;
  G4double mylarThickness;
  G4double AlumThickness;
  G4double carrierRadius;
  G4double sourceRadius;
  G4double stepSize;

  std::vector<G4int> sourceIDs;
  std::vector<G4ThreeVector> sourcePos;

  G4int backingConfig;

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
  G4Material* cadmiumMaterial;
  G4Material* pmmaMaterial;
  G4Material* alumCeramicMaterial;
  G4Material* berylliumMaterial;
  G4Material* germaniumMaterial;
  G4Material* waterMaterial;
  G4Material* carrierMaterial;

  G4Box* solidWorld;
  G4LogicalVolume* logicalWorld;
  G4VPhysicalVolume* physicalWorld;

  G4Tubs* solidSilicon;
  G4Tubs* solidAlumF;
  G4Tubs* solidAlumB;
  G4Tubs* solidFoil;
  G4Tubs* solidbackFoil;
  G4LogicalVolume* logicalAlumF;
  G4LogicalVolume* logicalAlumB;
  G4LogicalVolume* logicalSilicon;
  G4LogicalVolume* logicalFoil;
  G4LogicalVolume* logicalbackFoil;
  G4VPhysicalVolume* physicalAlumF;
  G4VPhysicalVolume* physicalAlumB;
  G4VPhysicalVolume* physicalSilicon;
  G4VPhysicalVolume* physicalFoil;
  G4VPhysicalVolume* physicalbackFoil;

  G4Polyhedra* solidBacking;
  G4LogicalVolume* logicalBacking;
  G4VPhysicalVolume* physicalBacking;

  G4Tubs* solidbArmor;
  G4LogicalVolume* logicalbArmor;
  G4VPhysicalVolume* physicalbArmor;

  G4Tubs* solidRoger;
  G4LogicalVolume* logicalRoger;
  G4VPhysicalVolume* physicalRoger;

  G4Tubs* solidCM1;
  G4LogicalVolume* logicalCM1;
  G4VPhysicalVolume* physicalCM1;

  G4Tubs* solidCM2;
  G4LogicalVolume* logicalCM2;
  G4VPhysicalVolume* physicalCM2;

  G4Tubs* ArmorTube;
  G4Polyhedra* ArmorInVoid;
  G4SubtractionSolid* CopperArmor;
  G4LogicalVolume* logicalCArmor;
  G4VPhysicalVolume* physicalCArmor;

  G4Box* AlBox;
  G4Tubs* AlBore;
  G4SubtractionSolid* AlBlock;
  G4LogicalVolume* logicalAlBlock;
  G4VPhysicalVolume* physicalAlBlock;

  G4Box* AlPad;
  G4LogicalVolume* logicalAlPad;
  G4VPhysicalVolume* physicalAlPadUF;
  G4VPhysicalVolume* physicalAlPadLF;
  G4VPhysicalVolume* physicalAlPadUB;
  G4VPhysicalVolume* physicalAlPadLB;

  G4Box* AlTopLayer;
  G4Tubs* DiskSlit;
  G4SubtractionSolid* AlSlit;
  G4LogicalVolume* logicalAlSlit;
  G4VPhysicalVolume* physicalAlSlitF;
  G4VPhysicalVolume* physicalAlSlitB;

  G4Box* AlEarBox;
  G4Tubs* DiskCut;
  G4SubtractionSolid* AlEar;
  G4LogicalVolume* logicalAlEar;
  G4VPhysicalVolume* physicalAlEarF1;
  G4VPhysicalVolume* physicalAlEarF2;
  G4VPhysicalVolume* physicalAlEarF3;
  G4VPhysicalVolume* physicalAlEarF4;
  G4VPhysicalVolume* physicalAlEarB1;
  G4VPhysicalVolume* physicalAlEarB2;
  G4VPhysicalVolume* physicalAlEarB3;
  G4VPhysicalVolume* physicalAlEarB4;

  G4Tubs* Spacer;
  G4LogicalVolume* logicalSpacer;
  G4VPhysicalVolume* physicalSpacer;

  G4Polyhedra* HexNut;
  G4LogicalVolume* logicalHexNut;
  G4VPhysicalVolume* physicalHexNut;

  G4Tubs* HolderArm;
  G4LogicalVolume* logicalHolderArm;
  G4VPhysicalVolume* physicalHolderArm;

  G4Tubs* solidSourceHolder;
  G4LogicalVolume* logicalSourceHolder;
  G4VPhysicalVolume* physicalSourceHolder;

  G4Tubs* solidSourceRegion;
  G4LogicalVolume* logicalSourceRegion;
  G4VPhysicalVolume* physicalSourceRegion;

  G4Box* solidRectCarrier;
  G4Tubs* solidCarrier;
  G4LogicalVolume* logicalCarrier;
  G4VPhysicalVolume* physicalCarrier;

  G4UserLimits* stepLimitMyl;
  G4UserLimits* stepLimitCar;
};

#endif
