//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef NDDEventAction_h
#define NDDEventAction_h 1

#include "G4String.hh"
#include "G4Types.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "G4UserEventAction.hh"

#include <vector>

struct VolumeVisit {
  G4double currentEn;
  G4double time;
  G4String volume;
  G4String particleName;
};

struct PrimaryEvent {
  G4int trackID;
  G4double energy;
  G4double creationTime;
  G4ThreeVector creationPos;
  G4String creationProcess;
  G4String particleName;
};

class NDDRunAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NDDEventAction : public G4UserEventAction {
 public:
  NDDEventAction();
  virtual ~NDDEventAction();

 public:
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  inline void AddDepositedEnDead(const G4double& e) {
    enDepDead += e;
  };
  inline void AddDepositedEnFoil(const G4double& e) { enDepFoil += e; };
  inline void AddDepositedEnSourceHolder(const G4double& e) { enDepSourceHolder += e; };
  inline void AddBremsstrahlungLoss(const G4double& e) { bremsstrahlungLoss += e; };
  inline void SetSiPoE(const G4ThreeVector& v) {
    poeXSi = v.getX();
    poeYSi = v.getY();
  };
  inline void SetFoilPoE(const G4ThreeVector& v) {
    poeXF = v.getX();
    poeYF = v.getY();
  };
  inline void SetSiTime(const G4double& t) { timeSi = t; };
  inline void SetAngleOut(G4double a) { angleSiOut = a; };
  inline void SetAngleSourceOut(G4double a) { angleSourceOut = a; };
  inline void AddDepositedEnCarrier(const G4double& e) {
    enDepCarrier += e;
  };

  void AddVisitedVolume(G4double, G4double, G4String, G4String);
  void AddPrimaryEvent(G4int, G4double, G4double, G4ThreeVector, G4String, G4String);

  void ParseStepInfo(const std::string&, const G4double&, const G4ThreeVector&);

 private:
  G4int ClassifyEvent();

  G4double enDepSi;
  G4double enDepDead;
  G4double enDepFoil;
  G4double enDepCarrier;
  G4double enDepSourceHolder;
  G4double poeXSi, poeYSi;
  G4double poeXF, poeYF;
  G4double timeSi;
  G4double bremsstrahlungLoss;

  G4double angleSiOut, angleSourceOut;

  G4int SiHCiD;

  G4int classification;

  std::vector<VolumeVisit> visitedVolumes;
  std::vector<PrimaryEvent> primaryEvents;

  void Clear();
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
