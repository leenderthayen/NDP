//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "NDDEventAction.hh"
#include "NDDRunAction.hh"
#include "NDDSiPixelHit.hh"
#include "NDDAnalysis.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4SDManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDEventAction::NDDEventAction() : G4UserEventAction() {
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
  // save rndm status
  /*if (fRunAction->GetRndmFreq() == 2) {
    G4Random::saveEngineStatus("endOfEvent.rndm");

    G4int evtNb = evt->GetEventID();
    G4int printProgress = G4RunManager::GetRunManager()->GetPrintProgress();
    if (evtNb % printProgress == 0) {
      G4cout << "\n---> End of Event: " << evtNb << G4endl;
      G4Random::showEngineStatus();
    }
  }*/

  classification = ClassifyEvent();

  enPrimary = 0;
  for (auto & en : enPrimaries) {
    enPrimary += en;
  }

  G4HCofThisEvent* hce = evt->GetHCofThisEvent();
  NDDSiPixelHitsCollection* SiPixelHC = 0;

  SiPixelHC = (NDDSiPixelHitsCollection*)(hce->GetHC(SiHCiD));

  G4int nrHits = SiPixelHC->entries();

  std::vector<G4double> pixelEnDep;
  pixelEnDep.resize(128);

  for (G4int iHit = 0; iHit < nrHits; iHit++) {
    NDDSiPixelHit* hit = (*SiPixelHC)[iHit];
    G4ThreeVector pos = hit->GetPos();
    G4ThreeVector mom = hit->GetMomentum();
    G4String particleType = hit->GetParticleCode();
    FillHitsTuple(evt->GetEventID(), classification, enPrimary,
                            hit->GetEnDep(), pos.x(), pos.y(), pos.z(), mom.x(),
                            mom.y(), mom.z(), hit->GetTime(), 1, 0);

    enDepSi += hit->GetEnDep();
    if (iHit == 0) {
      timeSi = hit->GetTime();
      poeXSi = pos.x();
      poeYSi = pos.y();
    }
    FillH2Hist(1, std::abs(pos.z()), pos.x());

    pixelEnDep[hit->GetPixelNumber() - 1] += hit->GetEnDep();
  }

  FillSpacetimeTuple(evt->GetEventID(), classification,
                    angleSourceOut, angleSiOut, timeSi, poeXSi, poeYSi);

  FillH2Hist(0, poeXSi, poeYSi);

  if (enDepSi > 0) FillH1Hist(1, enDepSi);
  if (enDepDead > 0) FillH1Hist(2, enDepDead);
  if (enDepFoil > 0) FillH1Hist(3, enDepFoil);
  if (enDepCarrier > 0) FillH1Hist(4, enDepCarrier);
  if (enDepSourceHolder > 0) FillH1Hist(5, enDepSourceHolder);
  if (bremsstrahlungLoss > 0) FillH1Hist(6, bremsstrahlungLoss);
  if (timeSi > 0) FillH1Hist(7, timeSi);

  FillEnergyTuple(
      evt->GetEventID(), classification, enPrimary, enDepSi,
      enDepDead, enDepFoil, enDepCarrier, enDepSourceHolder,
      bremsstrahlungLoss);

  for (G4int i = 0; i < pixelEnDep.size(); i++) {
    if (pixelEnDep[i] > 0) {
      FillH1Hist(8 + i, pixelEnDep[i]);
    }
  }

  FillPixelTuple(evt->GetEventID(), classification, enPrimary,
                           pixelEnDep);

  for (G4int i = 0; i < visitedVolumes.size(); i++) {
    VolumeVisit* v = &(visitedVolumes[i]);
    FillVolumesTuple(evt->GetEventID(), classification, enPrimary,
                               v->currentEn, v->time, v->volume);
  }
}

void NDDEventAction::AddVisitedVolume(G4double currentEn, G4double time,
                                   G4String volume) {
  if (visitedVolumes.size() == 0 ||
      visitedVolumes[visitedVolumes.size() - 1].volume != volume) {
    VolumeVisit vv = {currentEn, time, volume};
    visitedVolumes.push_back(vv);
  }
}

void NDDEventAction::Clear() {
  enPrimary = enDepSi = enDepDead = enDepFoil = enDepCarrier =
          enDepSourceHolder = bremsstrahlungLoss = 0;
  poeXSi = poeYSi = timeSi = 0;
  angleSourceOut = angleSiOut= 0;
  visitedVolumes.clear();
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

  for (G4int i = 0; i < visitedVolumes.size(); i++) {
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

void NDDEventAction::FillEnergyTuple(
    G4int iD, G4int classification, G4double enPrimary, G4double enSi,
    G4double enDead, G4double enFoil, G4double enCarrier, G4double enSourceHolder,
    G4double bremsstrahlungLoss) {

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->GetNtupleActivation(0)) {
    analysisManager->FillNtupleIColumn(0, 0, iD);
    analysisManager->FillNtupleIColumn(0, 1, classification);
    analysisManager->FillNtupleDColumn(0, 2, enPrimary / keV);
    analysisManager->FillNtupleDColumn(0, 3, enSi / keV);
    analysisManager->FillNtupleDColumn(0, 4, enDead / keV);
    analysisManager->FillNtupleDColumn(0, 5, enFoil / keV);
    analysisManager->FillNtupleDColumn(0, 6, enCarrier / keV);
    analysisManager->FillNtupleDColumn(0, 7, enSourceHolder / keV);
    analysisManager->FillNtupleDColumn(0, 8, bremsstrahlungLoss / keV);
    analysisManager->AddNtupleRow(0);
  }
}

void NDDEventAction::FillSpacetimeTuple(
    G4int iD, G4int classification, G4double angleSourceOut,
    G4double angleSiOut, G4double timeSi, G4double poeXSi, G4double poeYSi) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->GetNtupleActivation(1)) {
    analysisManager->FillNtupleIColumn(1, 0, iD);
    analysisManager->FillNtupleIColumn(1, 1, classification);
    analysisManager->FillNtupleDColumn(1, 2, angleSourceOut);
    analysisManager->FillNtupleDColumn(1, 3, angleSiOut);
    analysisManager->FillNtupleDColumn(1, 4, timeSi / ns);
    analysisManager->FillNtupleDColumn(1, 5, poeXSi / mm);
    analysisManager->FillNtupleDColumn(1, 6, poeYSi / mm);
    analysisManager->AddNtupleRow(1);
  }
}

void NDDEventAction::FillHitsTuple(G4int iD, G4int classification,
                                   G4double enPrimary, G4double eDep,
                                   G4double x, G4double y, G4double z,
                                   G4double px, G4double py, G4double pz,
                                   G4double time, G4int volume, G4int particle) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->GetNtupleActivation(2)) {
    analysisManager->FillNtupleIColumn(2, 0, iD);
    analysisManager->FillNtupleIColumn(2, 1, classification);
    analysisManager->FillNtupleDColumn(2, 2, enPrimary / keV);
    analysisManager->FillNtupleDColumn(2, 3, eDep / keV);
    analysisManager->FillNtupleDColumn(2, 4, x / mm);
    analysisManager->FillNtupleDColumn(2, 5, y / mm);
    analysisManager->FillNtupleDColumn(2, 6, z / mm);
    analysisManager->FillNtupleDColumn(2, 7, px);
    analysisManager->FillNtupleDColumn(2, 8, py);
    analysisManager->FillNtupleDColumn(2, 9, pz);
    analysisManager->FillNtupleDColumn(2, 10, time / ns);
    analysisManager->FillNtupleIColumn(2, 11, volume);
    analysisManager->FillNtupleIColumn(2, 12, particle);
    analysisManager->AddNtupleRow(2);
  }
}

void NDDEventAction::FillPixelTuple(G4int iD, G4int classification,
                                    G4double enPrimary,
                                    std::vector<G4double>& eDep) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->GetNtupleActivation(3)) {
    analysisManager->FillNtupleIColumn(3, 0, iD);
    analysisManager->FillNtupleIColumn(3, 1, classification);
    analysisManager->FillNtupleDColumn(3, 2, enPrimary);
    for (G4int i = 0; i < eDep.size(); i++) {
      analysisManager->FillNtupleDColumn(3, i + 3, eDep[i] / keV);
    }
    analysisManager->AddNtupleRow(3);
  }
}

void NDDEventAction::FillVolumesTuple(G4int iD, G4int classification,
                                      G4double primaryEn, G4double currentEn,
                                      G4double time, G4String volume) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->GetNtupleActivation(4)) {
    analysisManager->FillNtupleIColumn(4, 0, iD);
    analysisManager->FillNtupleIColumn(4, 1, classification);
    analysisManager->FillNtupleDColumn(4, 2, primaryEn / keV);
    analysisManager->FillNtupleDColumn(4, 3, currentEn / keV);
    analysisManager->FillNtupleDColumn(4, 4, time / ns);
    analysisManager->FillNtupleSColumn(4, 5, volume);
    analysisManager->AddNtupleRow(4);
  }
}

void NDDEventAction::FillH1Hist(G4int ih, G4double xbin, G4double weight) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillH1(ih, xbin, weight);
}

void NDDEventAction::FillH2Hist(G4int ih, G4double xbin, G4double ybin,
                                G4double weight) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillH2(ih, xbin, ybin, weight);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
