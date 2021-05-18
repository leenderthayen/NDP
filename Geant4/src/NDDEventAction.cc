//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "NDDEventAction.hh"
#include "NDDRunAction.hh"
#include "NDDSiPixelHit.hh"
#include "NDDAnalysisManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4SDManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDEventAction::NDDEventAction(NDDAnalysisManager* anMan)
: G4UserEventAction(), analysisManager(anMan) {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDEventAction::~NDDEventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDEventAction::BeginOfEventAction(const G4Event* evt) {
  Clear();

  auto sdMan = G4SDManager::GetSDMpointer();

  SiHCiD = sdMan->GetCollectionID("SiPixelHitCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDEventAction::EndOfEventAction(const G4Event* evt) {
  classification = ClassifyEvent();

  G4double enPrimary = 0;
  for (auto & pe : primaryEvents) {
    enPrimary += pe.energy;
  }

  G4HCofThisEvent* hce = evt->GetHCofThisEvent();
  NDDSiPixelHitsCollection* SiPixelHC = 0;

  SiPixelHC = (NDDSiPixelHitsCollection*)(hce->GetHC(SiHCiD));

  G4int nrHits = SiPixelHC->entries();

  std::vector<G4double> pixelEnDep;
  pixelEnDep.resize(128);

  for (int iHit = 0; iHit < nrHits; iHit++) {
    NDDSiPixelHit* hit = (*SiPixelHC)[iHit];
    G4ThreeVector pos = hit->GetPos();
    G4ThreeVector mom = hit->GetMomentum();
    G4String particleName = hit->GetParticleName();
    G4int trackID = hit->GetTrackID();
    G4int pixelNumber = hit->GetPixelNumber();
    analysisManager->FillHitsTuple(evt->GetEventID(), trackID, classification, enPrimary,
                            hit->GetEnDep(), pos.x(), pos.y(), pos.z(), mom.x(),
                            mom.y(), mom.z(), hit->GetTime(), pixelNumber, particleName);

    enDepSi += hit->GetEnDep();
    if (iHit == 0) {
      timeSi = hit->GetTime();
      poeXSi = pos.x();
      poeYSi = pos.y();
    }
    analysisManager->FillH2Hist(1, std::abs(pos.z()), pos.x());

    pixelEnDep[hit->GetPixelNumber() - 1] += hit->GetEnDep();
  }

  analysisManager->FillSpacetimeTuple(evt->GetEventID(), classification,
                    angleSourceOut, angleSiOut, timeSi, poeXSi, poeYSi);

  analysisManager->FillH2Hist(0, poeXSi, poeYSi);

  if (enDepSi > 0) analysisManager->FillH1Hist(1, enDepSi);
  if (enDepDead > 0) analysisManager->FillH1Hist(2, enDepDead);
  if (enDepFoil > 0) analysisManager->FillH1Hist(3, enDepFoil);
  if (enDepCarrier > 0) analysisManager->FillH1Hist(4, enDepCarrier);
  if (enDepSourceHolder > 0) analysisManager->FillH1Hist(5, enDepSourceHolder);
  if (bremsstrahlungLoss > 0) analysisManager->FillH1Hist(6, bremsstrahlungLoss);
  if (timeSi > 0) analysisManager->FillH1Hist(7, timeSi);

  analysisManager->FillEnergyTuple(
      evt->GetEventID(), classification, enPrimary, enDepSi,
      enDepDead, enDepFoil, enDepCarrier, enDepSourceHolder,
      bremsstrahlungLoss);

  for (int i = 0; i < pixelEnDep.size(); i++) {
    if (pixelEnDep[i] > 0) {
      analysisManager->FillH1Hist(8 + i, pixelEnDep[i]);
    }
  }

  analysisManager->FillPixelTuple(evt->GetEventID(), classification, enPrimary,
                           pixelEnDep);

  for (int i = 0; i < visitedVolumes.size(); i++) {
    VolumeVisit v = visitedVolumes[i];
    analysisManager->FillVolumesTuple(evt->GetEventID(), classification, enPrimary,
                               v.currentEn, v.time, v.volume, v.particleName);
  }

  for (int i = 0; i < primaryEvents.size(); i++) {
    PrimaryEvent pe = primaryEvents[i];
    analysisManager->FillPrimaryTuple(evt->GetEventID(), pe.trackID, classification,
                     pe.energy, pe.creationTime, pe.creationPos.x(),
                     pe.creationPos.y(), pe.creationPos.z(), pe.creationProcess,
                     pe.particleName);
  }
}

void NDDEventAction::AddVisitedVolume(G4double currentEn, G4double time,
                                   G4String volume, G4String particleName) {
  if (visitedVolumes.size() == 0 ||
      visitedVolumes[visitedVolumes.size() - 1].volume != volume) {
    VolumeVisit vv = {currentEn, time, volume, particleName};
    visitedVolumes.push_back(vv);
  }
}

void NDDEventAction::Clear() {
  enDepSi = enDepDead = enDepFoil = enDepCarrier =
          enDepSourceHolder = bremsstrahlungLoss = 0;
  poeXSi = poeYSi = timeSi = 0;
  angleSourceOut = angleSiOut= 0;
  visitedVolumes.clear();
  primaryEvents.clear();
}

void NDDEventAction::AddPrimaryEvent(G4int trackID, G4double energy, G4double time,
  G4ThreeVector creationPos, G4String process, G4String particleName) {
  PrimaryEvent pe = {trackID, energy, time, creationPos, process, particleName};
  primaryEvents.push_back(pe);
}

G4int NDDEventAction::ClassifyEvent() {
  /* Required digits
     * Determine whether east or west was hit first: 1 east 2 west 3 neither
     * Number of separate detector hits for east and west
     * Number of magnetic reflections on east and west
     * Number of dead layer hits on east and west
     * Number of times passing through the foil
     * Number of backscatters east and west

     Total: 10
  */
  G4int SiHits = 0;
  G4int backscatters = 0;
  G4int deadHits = 0;
  G4int foilHits = 0;

  for (int i = 0; i < visitedVolumes.size(); i++) {
    VolumeVisit v = visitedVolumes[i];
    if (v.volume.find("Foil") != std::string::npos) {
      foilHits++;
    } else if (v.volume == "Dead") {
      deadHits++;
    } else if (v.volume == "physicalSilicon") {
      SiHits++;
    }
  }
  G4int classification = 1e3 * SiHits +
                         1e2 * backscatters +
                         1e1 * deadHits  +
                         1e0 * foilHits;

  return classification;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
