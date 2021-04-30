#ifndef NDDPixelReadOut_h
#define NDDPixelReadOut_h 1

#include "G4VUserParallelWorld.hh"

#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"

class G4VUserDetectorConstruction;
class NDDPixelReadOutMessenger;

class NDDPixelReadOut : public G4VUserParallelWorld {
public:
  NDDPixelReadOut(G4String&, G4VUserDetectorConstruction*);
  virtual ~NDDPixelReadOut();

  inline void SetPixelRings(G4int i) {pixelRings = i;}
  inline void SetPixelSize(G4double d) {pixelSize = d;}
  inline void SetDeadLayerThickness(G4double d) {deadLayerThickness = d;}
  inline void EnableDeadLayer(G4bool b) {enableDeadLayer = b;}

protected:
  virtual void Construct();
  virtual void ConstructSD();

private:
  NDDPixelReadOutMessenger* proMess;
  G4VUserDetectorConstruction* dc;
  G4int pixelRings;
  G4double pixelSize;
  G4double deadLayerThickness;
  G4bool enableDeadLayer;

  G4VSolid* solidPixel;
  G4LogicalVolume* logicalPixel;

};

#endif
