#ifndef NDDAnalysisManager_h
#define NDDAnalysisManager_h

#include "g4root.hh"
//#include "g4xml.hh"
//#include "g4csv.hh"
//#include "g4hdf5.hh"

#include "globals.hh"

class NDDAnalysisManager {
public:
  NDDAnalysisManager();
 ~NDDAnalysisManager();

  void Book();
  void Save();

  void FillPrimaryTuple(G4int, G4int, G4int, G4double, G4double, G4double, G4double, G4double, G4String, G4String);
  void FillEnergyTuple(G4int, G4int, G4double, G4double, G4double,
      G4double, G4double, G4double, G4double);
  void FillSpacetimeTuple(G4int, G4int, G4double, G4double, G4double, G4double, G4double);
  void FillHitsTuple(G4int, G4int, G4int, G4double, G4double, G4double, G4double, G4double,
      G4double, G4double, G4double, G4double, G4int, G4String);
  void FillPixelTuple(G4int, G4int, G4double, std::vector<G4double>&);
  void FillVolumesTuple(G4int, G4int, G4double, G4double, G4double, G4String, G4String);
  void FillH1Hist(G4int ih, G4double xbin, G4double weight = 1.);
  void FillH2Hist(G4int ih, G4double xbin, G4double ybin, G4double weight = 1.);

};

#endif
