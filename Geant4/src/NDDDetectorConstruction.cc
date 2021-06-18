#include "NDDDetectorConstruction.hh"
#include "NDDDetectorMessenger.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4NistManager.hh"

#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Polyhedra.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

#include "G4UserLimits.hh"
#include "G4SystemOfUnits.hh"

NDDDetectorConstruction::NDDDetectorConstruction()
    : solidWorld(0),
      logicalWorld(0),
      physicalWorld(0),
      siThickness(2. * mm),
      siBackingThickness(1. * mm),
      mylarThickness(5. * um),
      AlumThickness(1. * um),
      siOuterRadius(6.157 * cm),
      stepLimitMyl(0),
      stepLimitCar(0) {
  detMess = new NDDDetectorMessenger(this);
  detectorPosition = G4ThreeVector(0, 0, 10 * mm);
}

NDDDetectorConstruction::~NDDDetectorConstruction() {
  delete stepLimitMyl;
  delete stepLimitCar;
  delete detMess;
}

G4VPhysicalVolume* NDDDetectorConstruction::Construct() {

  BuildMaterials();
  BuildWorld();
  BuildSiDetector();
  BuildSources();
  BuildVisualisation();

  SetStepLimits();

  return physicalWorld;
}

void NDDDetectorConstruction::BuildMaterials() {
  // Material definitions
  G4double a, z;
  G4double density, pressure;
  G4int nel, natoms;

  G4NistManager* man = G4NistManager::Instance();

  waterMaterial = man->FindOrBuildMaterial("G4_WATER");

  // airMaterial at STP
  density = 1.293 * mg / cm3;
  G4Element* N = new G4Element("Nitrogen", "N", z = 7., a = 14.01 * g / mole);
  G4Element* O = new G4Element("Oxygen", "O", z = 8., a = 16.00 * g / mole);
  airMaterial = new G4Material("Air", density, nel = 2);
  airMaterial->AddElement(N, 70 * perCent);
  airMaterial->AddElement(O, 30 * perCent);

  // "Vacuum": 1e-8 Torr (low density air taken proportional to pressure)
  G4double densityAirSTP = density;
  pressure = 1e-8;  // Torr
  density = (pressure / 760.) * densityAirSTP;
  vacuumMaterial = new G4Material("Vacuum", density, nel = 2);
  vacuumMaterial->AddElement(N, 70 * perCent);
  vacuumMaterial->AddElement(O, 30 * perCent);

  // Silicon: properties from wikipedia
  siliconMaterial = new G4Material("Silicon", z = 14., a = 28.086 * g / mole,
                                   density = 2.329 * g / cm3);

  // copperMaterial: properties from wikipedia
  copperMaterial =
      new G4Material("copperMaterial", z = 29., a = 63.546 * g / mole,
                     density = 8.94 * g / cm3);

  // Stainless steel: model from
  // http://hypernews.slac.stanford.edu/HyperNews/geant4/get/geometry/915/1.html
  G4int nSS = 6;
  G4double fractionmass;
  density = 8.06 * g / cm3;
  stainlessSteelMaterial = new G4Material("Stainless Steel", density, nSS);
  G4Element* C = new G4Element("Carbon", "C", z = 6., a = 12.011 * g / mole);
  G4Element* Si =
      new G4Element("Silicon", "Si", z = 14., a = 28.086 * g / mole);
  G4Element* Cr =
      new G4Element("Chromium", "Cr", z = 24., a = 51.996 * g / mole);
  G4Element* Mn =
      new G4Element("Manganese", "Mn", z = 25., a = 54.938 * g / mole);
  G4Element* Fe = new G4Element("Iron", "Fe", z = 26., a = 55.845 * g / mole);
  G4Element* Ni = new G4Element("Nickel", "Ni", z = 28., a = 58.693 * g / mole);
  stainlessSteelMaterial->AddElement(C, fractionmass = 0.001);
  stainlessSteelMaterial->AddElement(Si, fractionmass = 0.007);
  stainlessSteelMaterial->AddElement(Cr, fractionmass = 0.18);
  stainlessSteelMaterial->AddElement(Mn, fractionmass = 0.01);
  stainlessSteelMaterial->AddElement(Fe, fractionmass = 0.712);
  stainlessSteelMaterial->AddElement(Ni, fractionmass = 0.09);

  aluminiumMaterial = new G4Material("Aluminum", z = 13, a = 26.9815 * g / mole,
                                     density = 2.70 * g / cm3);

  tinMaterial = new G4Material("Tin", z = 50, a = 118.710 * g / mole,
                               density = 7.365 * g / cm3);

  bariumMaterial = new G4Material("Barium", z = 56, a = 137.327 * g / mole,
                                  density = 3510 * kg / m3);

  bismuthMaterial = new G4Material("Bismuth", z = 83, a = 208.980 * g / mole,
                                   density = 9.78 * g / cm3);

  cadmiumMaterial = new G4Material("Cadmium", z = 48, a = 112.414 * g / mole,
                                  density = 8.65 * g / cm3);

  G4Element* H = new G4Element("Hydrogen", "H", z = 1., a = 1.0008 * g / mole);
  mylarMaterial = new G4Material("Mylar", 1.370 * g / cm3, 3);
  mylarMaterial->AddElement(C, fractionmass = 0.62500);
  mylarMaterial->AddElement(H, fractionmass = 0.04167);
  mylarMaterial->AddElement(O, fractionmass = 0.33333);

  kaptonMaterial = new G4Material("Kapton", 1.42 * g / cm3, 4);
  kaptonMaterial->AddElement(N, fractionmass = 0.074);
  kaptonMaterial->AddElement(C, fractionmass = 0.691);
  kaptonMaterial->AddElement(O, fractionmass = 0.209);
  kaptonMaterial->AddElement(H, fractionmass = 0.026);

  pmmaMaterial = new G4Material("PMMA", 1.18 * g / cm3, 3);
  pmmaMaterial->AddElement(C, fractionmass = 0.6);
  pmmaMaterial->AddElement(H, fractionmass = 0.08);
  pmmaMaterial->AddElement(O, fractionmass = 0.32);

  sixFsixFMaterial = new G4Material("sixFsixF", 1.480 * g / cm3, 5);
  G4Element* F = new G4Element("Fluorine", "F", z = 9., a = 18.9984 * g / mole);
  sixFsixFMaterial->AddElement(H, fractionmass = 0.27027);
  sixFsixFMaterial->AddElement(C, fractionmass = 0.48648);
  sixFsixFMaterial->AddElement(N, fractionmass = 0.02703);
  sixFsixFMaterial->AddElement(O, fractionmass = 0.05405);
  sixFsixFMaterial->AddElement(F, fractionmass = 0.16216);

  aluminaMaterial = new G4Material("Alumina", 1.370 * g / cm3, 2);
  G4Element* Al =
      new G4Element("Aluminum", "Al", z = 13., a = 26.9815 * g / mole);
  aluminaMaterial->AddElement(Al, fractionmass = 0.53);
  aluminaMaterial->AddElement(O, fractionmass = 0.47);

  CaCl2Material = new G4Material("CaCl2", 2.15 * g / cm3, 2);
  G4Element* Ca = new G4Element("Calcium", "Ca", z = 20, a = 44.98 * g / mole);
  G4Element* Cl = new G4Element("Chlorine", "Cl", z = 17, a = 35.5 * g / mole);
  CaCl2Material->AddElement(Ca, fractionmass = 0.387);
  CaCl2Material->AddElement(Cl, fractionmass = 0.613);

  //96% Alumina Ceramic. 96% Al2O3 + 4% CaO
  alumCeramicMaterial = new G4Material("AlCeramic", 3.72 * g / cm3, 3);
  alumCeramicMaterial->AddElement(Al, fractionmass = 0.508);
  alumCeramicMaterial->AddElement(O, fractionmass = 0.463);
  alumCeramicMaterial->AddElement(Ca, fractionmass = 0.029);

  berylliumMaterial = new G4Material("Beryllium", z = 4, a = 9.0121 * g / mole,
                                     density = 1.85 * g / cm3);

  germaniumMaterial = new G4Material(
      "Germanium", z = 32., a = 72.630 * g / mole, density = 5.323 * g / cm3);

  // waterMaterial = new G4Material("Water", 1.000 * g / cm3, 2);
  // waterMaterial->AddElement(H, natoms = 2);
  // waterMaterial->AddElement(O, natoms = 1);
  // Print all the material definitions
  G4cout << G4endl << "Material Definitions : " << G4endl << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

void NDDDetectorConstruction::BuildWorld() {
  // World volume
  G4double worldX = 0.5 * m;
  G4double worldY = 0.5 * m;
  G4double worldZ = 6.0 * m;
  solidWorld = new G4Box("World", worldX, worldY, worldZ);
  logicalWorld = new G4LogicalVolume(solidWorld, vacuumMaterial, "World");
  physicalWorld =
      new G4PVPlacement(0, G4ThreeVector(), logicalWorld, "World", 0, false, 0);
}

void NDDDetectorConstruction::BuildSiDetector() {
  // Simple model for Silicon active region
  G4double xSilicon = detectorPosition.x();
  G4double ySilicon = detectorPosition.y();
  G4double zSilicon = detectorPosition.z() + siThickness / 2.0;

  G4double rotationAngle = -257.5 * deg;

  solidSilicon = new G4Tubs("solidSilicon", 0., siOuterRadius,
                                siThickness / 2., 0., 360. * deg);
  logicalSilicon = new G4LogicalVolume(solidSilicon, siliconMaterial,
                                           "logicalSilicon");
  physicalSilicon = new G4PVPlacement(
      0,
      G4ThreeVector(xSilicon, ySilicon, zSilicon),
      logicalSilicon, "physicalSilicon", logicalWorld, false, 0);

  //Varaibles for the Backing ------------------------------------------------
  //Ceramic Dimensions
  //G4double ceramicDiameter = 15.2 * cm;
  G4double ceramicDiameter = 14.7066 * cm;
  G4double czPlanes[2] = {0., siBackingThickness};
  G4double crInner[2] = {0., 0.};
  G4double crOuter[2] = {ceramicDiameter / 2.0, ceramicDiameter / 2.0};
  G4double zBack = zSilicon + siThickness / 2.;

  //Bottom of the Armor
  G4double bArmorDiameter = 17.78 * cm;
  G4double bArmorThickness = 0.5512 * cm;
  G4double zbArmor = zSilicon + siThickness / 2. + siBackingThickness + bArmorThickness / 2.;

  //Rogers Material Transition board
  G4double RO4350Diameter = 17.78 * cm;
  G4double RO4350Thickness = 0.4572 * cm;
  G4double zRoger =
      zSilicon + siThickness / 2. + siBackingThickness + bArmorThickness + RO4350Thickness / 2.0;

  //First Copper Mount
  G4double CuMount1Dia = 19.05 * cm;
  G4double CuMount1Thick = 0.57404 * cm;
  G4double zCuMount1 =
      zSilicon + siThickness / 2. + siBackingThickness + bArmorThickness + RO4350Thickness + CuMount1Thick/2.0;

  //Second Copper Mount
  G4double CuMount2InnerDia = 13.8176 * cm;
  G4double CuMount2Dia = 19.05 * cm;
  G4double CuMount2Thick = 0.7874 * cm;
  G4double zCuMount2 =
      zSilicon + siThickness / 2. + siBackingThickness + bArmorThickness + RO4350Thickness + CuMount1Thick + CuMount2Thick/2.0;

  //Detector Armor Dimensions
  G4double ArmorThickness = 0.9525 * cm;
  G4double ArmorRad = 8.89 * cm;
  G4double ArmorInRad = 14.7066 * cm;
  G4double azPlanes[2] = {-(ArmorThickness/1.5), (ArmorThickness/1.5)};
  G4double arInner[2] = {0., 0.};
  G4double arOuter[2] = {ArmorInRad / 2.0, ArmorInRad / 2.0};
  G4double zCuArmor = zSilicon + siThickness / 2. + siBackingThickness - ArmorThickness/2.0;
  //--------------------------------------------------------------------------

  if(backingConfig>0) {
    // Alumina backing (?)
    solidBacking = new G4Polyhedra("Backing", -11.25 * deg, 360. * deg, 16, 2, czPlanes, crInner, crOuter);
    logicalBacking =
        new G4LogicalVolume(solidBacking, alumCeramicMaterial, "Backing");
    physicalBacking = new G4PVPlacement(
        0, G4ThreeVector(xSilicon, ySilicon, zBack),
        logicalBacking, "Backing", logicalWorld, false, 0);


    // Bottom of the Armor and copper board
    solidbArmor = new G4Tubs("Backing", 0., bArmorDiameter/2.0,
                              bArmorThickness / 2., 0., 360. * deg);
    logicalbArmor =
        new G4LogicalVolume(solidbArmor, copperMaterial, "Backing");
    physicalbArmor = new G4PVPlacement(
        0, G4ThreeVector(xSilicon, ySilicon, zbArmor),
        logicalbArmor, "Backing", logicalWorld, false, 0);


    // Rogers Material Transition board
    solidRoger = new G4Tubs("Backing", 0., RO4350Diameter/2.0,
                              RO4350Thickness / 2., 0., 360. * deg);
    logicalRoger =
        new G4LogicalVolume(solidRoger, alumCeramicMaterial, "Backing");
    physicalRoger = new G4PVPlacement(
        0, G4ThreeVector(xSilicon, ySilicon, zRoger),
        logicalRoger, "Backing", logicalWorld, false, 0);


    // First Copper Mount
    solidCM1 = new G4Tubs("Backing", 0., CuMount1Dia/2.0,
                              CuMount1Thick / 2., 0., 360. * deg);
    logicalCM1 =
        new G4LogicalVolume(solidCM1, copperMaterial, "Backing");
    physicalCM1 = new G4PVPlacement(
        0, G4ThreeVector(xSilicon, ySilicon, zCuMount1),
        logicalCM1, "Backing", logicalWorld, false, 0);


    // Second Copper Mount
    solidCM2 = new G4Tubs("Backing", CuMount2InnerDia/2.0, CuMount2Dia/2.0,
                              CuMount2Thick / 2., 0., 360. * deg);
    logicalCM2 =
        new G4LogicalVolume(solidCM2, copperMaterial, "Backing");
    physicalCM2 = new G4PVPlacement(
        0, G4ThreeVector(xSilicon, ySilicon, zCuMount2),
        logicalCM2, "Backing", logicalWorld, false, 0);

    // Detector Armor
    ArmorTube = new G4Tubs("Backing", 0., ArmorRad,
                                  ArmorThickness / 2., 0., 360. * deg);

    // Void inside of the Armor
    ArmorInVoid = new G4Polyhedra("Backing", -11.25 * deg, 360. * deg, 16, 2, azPlanes, arInner, arOuter);
    CopperArmor = new G4SubtractionSolid("Backing", ArmorTube, ArmorInVoid, 0, G4ThreeVector());
    logicalCArmor =
        new G4LogicalVolume(CopperArmor, copperMaterial, "Backing");
    physicalCArmor = new G4PVPlacement(
        0, G4ThreeVector(xSilicon, ySilicon, zCuArmor),
        logicalCArmor, "Backing", logicalWorld, false, 0);
  }

}

void NDDDetectorConstruction::BuildSources() {
  if (sourceIDs.size() != sourcePos.size()) {
    G4cout << "ERROR: incompatible amount of source IDs and corresponding positions. Not building sources." << G4endl;
  } else {
    for (G4int i = 0; i < sourceIDs.size(); i++) {
      BuildSource(sourceIDs[i], sourcePos[i]);
    }
  }
}

void NDDDetectorConstruction::BuildSource(G4int id, G4ThreeVector pos) {
  carrierRadius = 1.5 * mm;
  G4double foilRadius;
  G4double ringOuterRadius, ringInnerRadius;
  G4double eastFoilThickness, westFoilThickness;
  G4double ringThickness;
  G4double SpacerThickness;
  G4double AlBSide,AlBThickness,BoreRadius,AlSlitLength,AlSlitHeight,AlSlitDepth,AlSourceOD;

  G4LogicalVolume* motherVolume = logicalWorld;

  G4RotationMatrix* yRot = new G4RotationMatrix;  // Rotates X and Z axes only
  G4RotationMatrix* zRot90 = new G4RotationMatrix;  // Rotates X and Z axes only
  G4RotationMatrix* zRot180 = new G4RotationMatrix;  // Rotates X and Z axes only
  G4RotationMatrix* zRot270 = new G4RotationMatrix;  // Rotates X and Z axes only

  G4double HNHeight = 0.2778*2.0*cm;
  G4double HNZplane[2] = {-HNHeight/2.0,HNHeight/2.0};
  G4double HNrInner[2] = {0., 0.};
  G4double HNrOuter[2] = {0.6416*cm, 0.6416*cm};

  if (id == 0) {
    // 45Ca 500nm
    carrierThickness = 60 * nm;
    foilRadius = 2.3 * cm;
    eastFoilThickness = 500. * nm;
    ringOuterRadius = 2.54 / 2. * cm;
    ringInnerRadius = 2.3 / 2. * cm;
    ringThickness = 3.0 * mm;

    solidCarrier = new G4Tubs("Carrier", 0.0, carrierRadius,
                              carrierThickness / 2., 0.0, 360. * deg);
    logicalCarrier =
        new G4LogicalVolume(solidCarrier, CaCl2Material, "Carrier");
    physicalCarrier =
        new G4PVPlacement(0, G4ThreeVector(pos.x(), pos.y(), pos.z()),
                          logicalCarrier, "Carrier", motherVolume, false, 0);

    solidFoil = new G4Tubs("Foil", 0., foilRadius,
                               eastFoilThickness / 2., 0., 360. * deg);
    logicalFoil =
        new G4LogicalVolume(solidFoil, sixFsixFMaterial, "Foil");
    physicalFoil = new G4PVPlacement(
        0,
        G4ThreeVector(pos.x(), pos.y(),
                      pos.z() + (eastFoilThickness + carrierThickness) / 2.0),
        logicalFoil, "Foil", motherVolume, false, 0);

    solidSourceHolder =
        new G4Tubs("SourceHolder", ringInnerRadius, ringOuterRadius,
                   ringThickness / 2.0, 0.0, 360. * deg);
    logicalSourceHolder = new G4LogicalVolume(
        solidSourceHolder, aluminiumMaterial, "SourceHolder");
    physicalSourceHolder = new G4PVPlacement(
        0, G4ThreeVector(pos.x(), pos.y(), pos.z()), logicalSourceHolder,
        "SourceHolder", motherVolume, false, 0);
  } else if (id == 1) {
    // 133Ba 12.5um
    carrierThickness = 10.0 * nm;
    foilRadius = 11.0 * mm;
    eastFoilThickness = 10.0 * um;
    westFoilThickness = 12.5 * um;
    ringInnerRadius = 8.0 * mm;
    ringOuterRadius = 15.0 * mm;
    ringThickness = 1. * mm;

    solidFoil = new G4Tubs("Foil", 0., foilRadius,
                               eastFoilThickness / 2., 0, 360 * deg);
    logicalFoil =
        new G4LogicalVolume(solidFoil, mylarMaterial, "Foil");
    physicalFoil = new G4PVPlacement(
        0,
        G4ThreeVector(pos.x(), pos.y(),
                      pos.z() + (eastFoilThickness + carrierThickness) / 2.0),
        logicalFoil, "Foil", motherVolume, false, 0);

    // solidWestFoil = new G4Tubs("WestFoil", 0., foilRadius,
    //                            westFoilThickness / 2., 0, 360 * deg);
    // logicalWestFoil =
    //     new G4LogicalVolume(solidWestFoil, kaptonMaterial, "WestFoil");
    // physicalWestFoil = new G4PVPlacement(
    //     0,
    //     G4ThreeVector(pos.x(), pos.y(),
    //                   pos.z() - (westFoilThickness + carrierThickness) / 2.0),
    //     logicalWestFoil, "WestFoil", motherVolume, false, 0);

    solidCarrier = new G4Tubs("Carrier", 0.0, carrierRadius,
                              carrierThickness / 2.0, 0.0, 360. * deg);
    logicalCarrier =
        new G4LogicalVolume(solidCarrier, bariumMaterial, "Carrier");
    physicalCarrier =
        new G4PVPlacement(0, G4ThreeVector(pos.x(), pos.y(), pos.z()),
                          logicalCarrier, "Carrier", motherVolume, false, 0);

    solidSourceHolder =
        new G4Tubs("SourceHolder", ringInnerRadius, ringOuterRadius,
                   ringThickness / 2.0, 0, 360. * deg);
    logicalSourceHolder =
        new G4LogicalVolume(solidSourceHolder, pmmaMaterial, "SourceHolder");
    physicalSourceHolder = new G4PVPlacement(
        0, G4ThreeVector(pos.x(), pos.y(),
                         pos.z() + (carrierThickness + ringThickness) / 2.0 +
                             westFoilThickness),
        logicalSourceHolder, "SourceHolder", motherVolume, false, 0);
  } else if (id == 2) {
    // 133Ba 500nm, quasi sealed
  } else if (id == 3 || id == 4 || id == 5) {
    if (id == 3) {
      // 207Bi 5 um
      carrierThickness = 7. * um;
      carrierMaterial = bismuthMaterial;

    } else if (id == 4) {
      // 113Sn, 139Ce 5 um
      carrierThickness = 1. * nm;
      carrierMaterial = tinMaterial;
    }
    else {
      // 109Cd
      carrierThickness = 1. * nm;
      carrierMaterial = cadmiumMaterial;
    }
    foilRadius = 0.87 * 2.54 / 2.0 * cm;
    eastFoilThickness = 5.0 * um;
    westFoilThickness = 5.0 * um;
    //ringInnerRadius = 0.87 * 2.54 / 2.0 * cm;
    ringInnerRadius = 17.52/2.0 * mm;
    ringOuterRadius = 2.54 / 2.0 * cm;
    ringThickness = 0.13 * 2.54 * cm;

    //Al Layer on top of the Front Foil
    solidAlumF = new G4Tubs("Foil", 0., foilRadius,
                               AlumThickness / 2.0, 0, 360. * deg);
    logicalAlumF =
        new G4LogicalVolume(solidAlumF, aluminiumMaterial, "Foil");
    physicalAlumF = new G4PVPlacement(
        0,
        G4ThreeVector(pos.x(), pos.y(),
                      pos.z() + mylarThickness + (carrierThickness + AlumThickness) / 2.0),
        logicalAlumF, "Foil", motherVolume, false, 0);

    //Front Foil
    solidFoil = new G4Tubs("Foil", 0., foilRadius,
                               mylarThickness / 2.0, 0, 360. * deg);
    logicalFoil =
        new G4LogicalVolume(solidFoil, mylarMaterial, "Foil");
    physicalFoil = new G4PVPlacement(
        0,
        G4ThreeVector(pos.x(), pos.y(),
                      pos.z() + (carrierThickness + mylarThickness) / 2.0),
        logicalFoil, "Foil", motherVolume, false, 0);

    //Al Layer on top of the Back Foil
    solidAlumB = new G4Tubs("Foil", 0., foilRadius,
                               AlumThickness / 2.0, 0, 360. * deg);
    logicalAlumB =
        new G4LogicalVolume(solidAlumB, aluminiumMaterial, "Foil");
    physicalAlumB = new G4PVPlacement(
        0,
        G4ThreeVector(pos.x(), pos.y(),
                      pos.z() - mylarThickness - (carrierThickness + AlumThickness) / 2.0),
        logicalAlumB, "Foil", motherVolume, false, 0);

    //Back Foil
    solidbackFoil = new G4Tubs("Foil", 0., foilRadius,
                               mylarThickness / 2.0, 0, 360. * deg);
    logicalbackFoil =
        new G4LogicalVolume(solidbackFoil, mylarMaterial, "Foil");
    physicalbackFoil = new G4PVPlacement(
        0,
        G4ThreeVector(pos.x(), pos.y(),
                      pos.z() - (carrierThickness + mylarThickness) / 2.0),
        logicalbackFoil, "Foil", motherVolume, false, 0);

    // solidWestFoil = new G4Tubs("WestFoil", 0., foilRadius,
    //                            westFoilThickness / 2.0, 0, 360. * deg);
    // logicalWestFoil =
    //     new G4LogicalVolume(solidWestFoil, mylarMaterial, "WestFoil");
    // physicalWestFoil = new G4PVPlacement(
    //     0, G4ThreeVector(pos.x(), pos.y(), pos.z() - (carrierThickness +
    //                                                   westFoilThickness) / 2.0),
    //     logicalWestFoil, "WestFoil", motherVolume, false, 0);

    solidCarrier = new G4Tubs("Carrier", 0.0, carrierRadius,
                              carrierThickness / 2.0, 0, 360. * deg);
    logicalCarrier =
        new G4LogicalVolume(solidCarrier, carrierMaterial, "Carrier");
    physicalCarrier =
        new G4PVPlacement(0, G4ThreeVector(pos.x(), pos.y(), pos.z()),
                          logicalCarrier, "Carrier", motherVolume, false, 0);

    solidSourceHolder =
        new G4Tubs("SourceHolder", ringInnerRadius, ringOuterRadius,
                   ringThickness / 2.0, 0, 360. * deg);
    logicalSourceHolder = new G4LogicalVolume(
        solidSourceHolder, aluminiumMaterial, "SourceHolder");
    physicalSourceHolder = new G4PVPlacement(
        0, G4ThreeVector(pos.x(), pos.y(), pos.z()), logicalSourceHolder,
        "SourceHolder", motherVolume, false, 0);
  }
  else if (id == 6) {

    //Manitoba Source Holder
    AlBSide = 2.2606 * cm;
    AlBThickness = 1.2446 * cm;
    BoreRadius = 0.25*2.54/2.0 * cm;

    //Dimensions of the Slit in the Al Box
    AlSlitLength = AlBSide/2.0; //Slit length will get a bit longer to avoid invisible wall
    AlSlitHeight = 0.13*2.54 * cm;
    AlSlitDepth = 0.06*2.54 * cm; //Slit depth will get a bit longer to avoid invisible wall

    //CAL2702 Diameter
    AlSourceOD = 2.54 * cm;

    //Al Block of the Source holder
    AlBox = new G4Box("SourceHolder", AlBSide/2.0, AlBSide/2.0, AlBThickness/2.0 - AlSlitDepth); //There is a layer of "Slit" on either side of the Al Box
    // Cylindrical Bore
    AlBore = new G4Tubs("SourceHolder",0.0, BoreRadius,AlBSide / 2., 0., 360. * deg); //Even though Box & Bore have same length, due to the offset, there shouldn't be any invisible wall
    //Rotation Matrix
    yRot->rotateY(90.*deg);
    zRot90->rotateZ(90.*deg);
    zRot180->rotateZ(180.*deg);
    zRot270->rotateZ(270.*deg);
    //subtraction solid
    AlBlock = new G4SubtractionSolid("SourceHolder", AlBox, AlBore, yRot, G4ThreeVector( -1.0/4.0*AlBSide,0.0,0.0));
    logicalAlBlock = new G4LogicalVolume(AlBlock, aluminiumMaterial, "SourceHolder");
    physicalAlBlock = new G4PVPlacement(
        0, G4ThreeVector(pos.x(), pos.y(), pos.z()),
        logicalAlBlock, "SourceHolder", motherVolume, false, 0);

    //Al Block of the Source holder
    AlPad = new G4Box("SourceHolder", AlBSide/4.0, (AlBSide-AlSlitHeight)/4.0, AlSlitDepth/2.0);
    logicalAlPad = new G4LogicalVolume(AlPad, aluminiumMaterial, "SourceHolder");
    physicalAlPadUF = new G4PVPlacement(
        0, G4ThreeVector(pos.x() + AlBSide/4.0, pos.y() + (AlBSide+AlSlitHeight)/4.0, pos.z() + (AlBThickness-AlSlitDepth)/2.0 ),
        logicalAlPad, "SourceHolder", motherVolume, false, 0);
    physicalAlPadLF = new G4PVPlacement(
        0, G4ThreeVector(pos.x() + AlBSide/4.0, pos.y() - (AlBSide+AlSlitHeight)/4.0, pos.z() + (AlBThickness-AlSlitDepth)/2.0 ),
        logicalAlPad, "SourceHolder", motherVolume, false, 0);
    physicalAlPadUB = new G4PVPlacement(
        0, G4ThreeVector(pos.x() + AlBSide/4.0, pos.y() + (AlBSide+AlSlitHeight)/4.0, pos.z() - (AlBThickness-AlSlitDepth)/2.0 ),
        logicalAlPad, "SourceHolder", motherVolume, false, 0);
    physicalAlPadLB = new G4PVPlacement(
        0, G4ThreeVector(pos.x() + AlBSide/4.0, pos.y() - (AlBSide+AlSlitHeight)/4.0, pos.z() - (AlBThickness-AlSlitDepth)/2.0 ),
        logicalAlPad, "SourceHolder", motherVolume, false, 0);


    //Al Block of the Source holder
    AlTopLayer = new G4Box("SourceHolder", AlBSide/4.0, AlBSide/2.0, AlSlitDepth/2.0);
    //Circular cut
    DiskSlit = new G4Tubs("SourceHolder",0.0, AlSlitHeight/ 2., AlSlitDepth / 2. + 1.0*mm, 0., 360. * deg);
    // Left Al Box
    AlSlit = new G4SubtractionSolid("SourceHolder", AlTopLayer, DiskSlit, 0, G4ThreeVector( AlBSide/4.0, 0.0 , 0.0 ));
    logicalAlSlit = new G4LogicalVolume(AlSlit, aluminiumMaterial, "SourceHolder");
    physicalAlSlitF = new G4PVPlacement(
        0, G4ThreeVector(pos.x() - AlBSide/4.0, pos.y(), pos.z() + (AlBThickness-AlSlitDepth)/2.0 ),
        logicalAlSlit, "SourceHolder", motherVolume, false, 0);

    physicalAlSlitB = new G4PVPlacement(
        0, G4ThreeVector(pos.x() - AlBSide/4.0, pos.y(), pos.z() - (AlBThickness-AlSlitDepth)/2.0 ),
        logicalAlSlit, "SourceHolder", motherVolume, false, 0);

    //Al Block of the Source holder
    AlEarBox = new G4Box("SourceHolder", AlBSide/4.0, AlBSide/4.0, AlSlitHeight/2.0);
    //Circular cut
    DiskCut = new G4Tubs("SourceHolder",0.0, AlSourceOD/ 2., AlSlitHeight / 2. + 1.0*mm, 0., 360. * deg);
    AlEar = new G4SubtractionSolid("SourceHolder", AlEarBox, DiskCut, 0, G4ThreeVector( -1.0/4.0*AlBSide, -1.0/4.0*AlBSide , 0.0 ));
    logicalAlEar = new G4LogicalVolume(AlEar, aluminiumMaterial, "SourceHolder");
    physicalAlEarF1 = new G4PVPlacement(
        0, G4ThreeVector(pos.x() + AlBSide/4.0, pos.y() + AlBSide/4.0, pos.z() + ((AlBThickness+AlSlitHeight)/2.0 ) ),
        logicalAlEar, "SourceHolder", motherVolume, false, 0);
    physicalAlEarF2 = new G4PVPlacement(
        zRot90, G4ThreeVector(pos.x() + AlBSide/4.0, pos.y() - AlBSide/4.0, pos.z() + ((AlBThickness+AlSlitHeight)/2.0 ) ),
        logicalAlEar, "SourceHolder", motherVolume, false, 0);
    physicalAlEarF3 = new G4PVPlacement(
        zRot180, G4ThreeVector(pos.x() - AlBSide/4.0, pos.y() - AlBSide/4.0, pos.z() + ((AlBThickness+AlSlitHeight)/2.0 ) ),
        logicalAlEar, "SourceHolder", motherVolume, false, 0);
    physicalAlEarF4 = new G4PVPlacement(
        zRot270, G4ThreeVector(pos.x() - AlBSide/4.0, pos.y() + AlBSide/4.0, pos.z() + ((AlBThickness+AlSlitHeight)/2.0 ) ),
        logicalAlEar, "SourceHolder", motherVolume, false, 0);
    physicalAlEarB1 = new G4PVPlacement(
        0, G4ThreeVector(pos.x() + AlBSide/4.0, pos.y() + AlBSide/4.0, pos.z() - ((AlBThickness+AlSlitHeight)/2.0 ) ),
        logicalAlEar, "SourceHolder", motherVolume, false, 0);
    physicalAlEarB2 = new G4PVPlacement(
        zRot90, G4ThreeVector(pos.x() + AlBSide/4.0, pos.y() - AlBSide/4.0, pos.z() - ((AlBThickness+AlSlitHeight)/2.0 ) ),
        logicalAlEar, "SourceHolder", motherVolume, false, 0);
    physicalAlEarB3 = new G4PVPlacement(
        zRot180, G4ThreeVector(pos.x() - AlBSide/4.0, pos.y() - AlBSide/4.0, pos.z() - ((AlBThickness+AlSlitHeight)/2.0 ) ),
        logicalAlEar, "SourceHolder", motherVolume, false, 0);
    physicalAlEarB4 = new G4PVPlacement(
        zRot270, G4ThreeVector(pos.x() - AlBSide/4.0, pos.y() + AlBSide/4.0, pos.z() - ((AlBThickness+AlSlitHeight)/2.0 ) ),
        logicalAlEar, "SourceHolder", motherVolume, false, 0);

    //Spacer between the hex nut and the Al block
    SpacerThickness = 2.0*mm;
    Spacer = new G4Tubs("SourceHolder",0.0, AlBThickness/2.0, SpacerThickness/2.0, 0., 360. * deg);
    logicalSpacer =
        new G4LogicalVolume(Spacer, stainlessSteelMaterial, "SourceHolder");
    physicalSpacer = new G4PVPlacement(
        yRot, G4ThreeVector(pos.x() + (AlBSide + SpacerThickness)/2.0, pos.y(), pos.z()),
        logicalSpacer, "SourceHolder", motherVolume, false, 0);

    // Hex Nut
    HexNut = new G4Polyhedra("SourceHolder", 15.0*deg, 360. * deg, 6, 2, HNZplane, HNrInner, HNrOuter);
    logicalHexNut =
        new G4LogicalVolume(HexNut, stainlessSteelMaterial, "SourceHolder");
    physicalBacking = new G4PVPlacement(
        yRot, G4ThreeVector(pos.x() + AlBSide/2.0 + SpacerThickness + HNHeight/2.0, pos.y(), pos.z()),
        logicalHexNut, "SourceHolder", motherVolume, false, 0);

    // Holder Arm
    HolderArm = new G4Tubs("SourceHolder",0.0, 5.0/16.0*2.54/2.0*cm, 5.0*cm, 0., 360. * deg);
    logicalHolderArm =
        new G4LogicalVolume(HolderArm, stainlessSteelMaterial, "SourceHolder");
    physicalHolderArm = new G4PVPlacement(
        yRot, G4ThreeVector(pos.x() +  AlBSide/2.0 + SpacerThickness + HNHeight + 5.0*cm, pos.y(), pos.z()),
        logicalHolderArm, "SourceHolder", motherVolume, false, 0);

  }
  else if (id == 7) {
    // 241Am source tests at LANL 2019
    // General dimensions are estimated, unfortunately
    G4double AlBridgeThickness = 2*mm;
    G4double AlBridgeWidth = 23.5*mm;
    G4double AlBridgeDepth = 11*mm;

    G4double AlBackingThickness = 2*mm;
    G4double AlBackingWidth = 40*mm;
    G4double AlBackingDepth = 150*mm;
    G4double AlBackingDistance = 5*mm;

    G4Box* AlBridge = new G4Box("SourceHolder", AlBridgeWidth/2.0, AlBridgeDepth/2.0, AlBridgeThickness/2.0);
    G4LogicalVolume* logicalAlBridge = new G4LogicalVolume(AlBridge, aluminiumMaterial, "logicalSourceHolder");
    G4PVPlacement* physicalAlBridge = new G4PVPlacement(0, G4ThreeVector(pos.x(), pos.y(), pos.z()), logicalAlBridge, "physicalSourceHolder", motherVolume, false, 0);

    G4Box* AlBacking = new G4Box("SourceBacking", AlBackingWidth/2.0, AlBackingDepth/2.0, AlBackingThickness/2.0);
    G4LogicalVolume* logicalAlBacking = new G4LogicalVolume(AlBacking, aluminiumMaterial, "logicalSourceBacking");
    G4PVPlacement* physicalAlBacking = new G4PVPlacement(0, G4ThreeVector(pos.x(), pos.y(), pos.z()-AlBackingDistance), logicalAlBacking, "physicalSourceBacking", motherVolume, false, 0);
  }
}

void NDDDetectorConstruction::BuildVisualisation() {
  // Visualization attributes

  logicalWorld->SetVisAttributes(G4VisAttributes::Invisible);

  G4VisAttributes* simpleBoxVisAttGreen =
      new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  simpleBoxVisAttGreen->SetVisibility(true);
  simpleBoxVisAttGreen->SetForceSolid(false);

  logicalSilicon->SetVisAttributes(simpleBoxVisAttGreen);

  G4VisAttributes* simpleBoxVisAttRed =
      new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
  simpleBoxVisAttRed->SetVisibility(true);
  simpleBoxVisAttRed->SetForceSolid(false);

  logicalCarrier->SetVisAttributes(simpleBoxVisAttRed);
  logicalSourceHolder->SetVisAttributes(simpleBoxVisAttRed);

  G4VisAttributes* simpleBoxVisAttYellow =
      new G4VisAttributes(G4Colour(0.988, 0.812, 0.01));
  simpleBoxVisAttYellow->SetVisibility(true);
  simpleBoxVisAttYellow->SetForceSolid(false);

  logicalbArmor->SetVisAttributes(simpleBoxVisAttYellow);
  logicalCM1->SetVisAttributes(simpleBoxVisAttYellow);
  logicalCM2->SetVisAttributes(simpleBoxVisAttYellow);
  logicalCArmor->SetVisAttributes(simpleBoxVisAttYellow);

}

void NDDDetectorConstruction::ConstructSDandField() {}

void NDDDetectorConstruction::SetStepLimits() {
  //G4double maxStepDL = stepSize * deadLayerThickness;  //Dead layer is gone
  G4double maxStepDL = 5. * nm;
  /*G4double maxStepWL = stepSize * waterThickness;
  G4UserLimits* stepLimitWater = new G4UserLimits(maxStepWL);
  logicalWater->SetUserLimits(stepLimitWater);

  G4double maxStepCar = stepSize * carrierThickness;
  stepLimitCar = new G4UserLimits(maxStepCar);
  logicalCarrier->SetUserLimits(stepLimitCar);

  G4double maxStepMyl = stepSize * mylarThickness;

  stepLimitMyl = new G4UserLimits(maxStepMyl);
  logicalFoil->SetUserLimits(stepLimitMyl);
  logicalWestFoil->SetUserLimits(stepLimitMyl);*/

  /*logicalWorld->SetUserLimits(
      new G4UserLimits(1e-3 * m, 1e2 * m, 1e3 * s, 1e-3 * keV));*/
}
