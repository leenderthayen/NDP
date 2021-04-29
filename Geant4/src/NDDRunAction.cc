//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "NDDRunAction.hh"
#include "NDDRunMessenger.hh"

#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"

#include "Randomize.hh"

#include "NDDAnalysis.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDRunAction::NDDRunAction()
    : G4UserRunAction(), runMessenger(0), fSaveRndm(0) {
  filename = "test";
  runMessenger = new NDDRunMessenger(this);

  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true); // Only relevant when writing to ROOT

  analysisManager->SetHistoDirectoryName("hist");
  analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetActivation(true);

  G4int bins = 1500;
  G4int nrH1 = 8;

  // General histograms
  analysisManager->CreateH1("enPrimary", "Primary energy", bins, 0, 1.500,
                            "keV");
  analysisManager->CreateH1("enSi", "Energy deposited in Si", bins, 0,
                            0.500, "keV");
  analysisManager->CreateH1("enDead", "Energy deposited in dead layer",
                            bins, 0, 0.100, "keV");
  analysisManager->CreateH1("enFoil", "Energy deposited in source foils", bins,
                            0, 0.050, "keV");
  analysisManager->CreateH1("enCarrier", "Energy deposited in carrier material",
                            bins, 0, 0.20, "keV");
  analysisManager->CreateH1("enSourceHolder",
                            "Energy deposited in source holder", bins, 0, 0.500,
                            "keV");
  analysisManager->CreateH1("bremsstrahlungLoss",
                            "Energy losses due to Bremsstrahlung", bins, 0,
                            0.250, "keV");
  analysisManager->CreateH1("timeSi", "Time distribution for Si", bins,
                            0, 500, "ns");

  // Pixel histograms
  for (G4int i = 0; i < 128; i++) {
    std::ostringstream name;
    name << i + 1 << "E";

    analysisManager->CreateH1(name.str(),
                              "Energy distribution for " + name.str(), bins, 0,
                              1.500, "keV");
  }

  G4int nrH2 = 2;

  analysisManager->CreateH2("poeSi", "First hit distribution on Si",
                            100, -5, 5, 100, -5, 5, "cm", "cm");
  analysisManager->CreateH2("penetrationSi", "Penetration depth on Si",
                            100, 10., 10.5, 100, -5, 5, "mm", "cm");

  for (G4int i = 0; i < nrH1; i++) {
    analysisManager->SetH1Activation(i, false);
  }
  for (G4int i = 0; i < nrH2; i++) {
    analysisManager->SetH2Activation(i, true);
  }

  analysisManager->CreateNtuple("energy", "Energy variables");
  analysisManager->CreateNtupleIColumn("iD");
  analysisManager->CreateNtupleIColumn("classification");
  analysisManager->CreateNtupleDColumn("enPrimary");
  analysisManager->CreateNtupleDColumn("enSi");
  analysisManager->CreateNtupleDColumn("enDead");
  analysisManager->CreateNtupleDColumn("enFoil");
  analysisManager->CreateNtupleDColumn("enCarrier");
  analysisManager->CreateNtupleDColumn("enSourceHolder");
  analysisManager->CreateNtupleDColumn("bremsstrahlungLoss");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("spaceTime", "Position and timing variables");
  analysisManager->CreateNtupleIColumn("iD");
  analysisManager->CreateNtupleIColumn("classification");
  analysisManager->CreateNtupleDColumn("angleSourceOut");
  analysisManager->CreateNtupleDColumn("angleOut");
  analysisManager->CreateNtupleDColumn("timeSi");
  analysisManager->CreateNtupleDColumn("poeXSi");
  analysisManager->CreateNtupleDColumn("poeYSi");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("hits", "Detector hits");
  analysisManager->CreateNtupleIColumn("iD");
  analysisManager->CreateNtupleIColumn("classification");
  analysisManager->CreateNtupleDColumn("enPrimary");
  analysisManager->CreateNtupleDColumn("eDep");
  analysisManager->CreateNtupleDColumn("x");
  analysisManager->CreateNtupleDColumn("y");
  analysisManager->CreateNtupleDColumn("z");
  analysisManager->CreateNtupleDColumn("px");
  analysisManager->CreateNtupleDColumn("py");
  analysisManager->CreateNtupleDColumn("pz");
  analysisManager->CreateNtupleDColumn("time");
  analysisManager->CreateNtupleSColumn("particle");
  analysisManager->CreateNtupleIColumn("pixelNumber");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("pixelEnergies", "Pixel hits");
  analysisManager->CreateNtupleIColumn("iD");
  analysisManager->CreateNtupleIColumn("classification");
  analysisManager->CreateNtupleDColumn("enPrimary");
  for (G4int i = 1; i <= 128; i++) {
    std::ostringstream pixelName;
    if (i < 128) {
      pixelName << i << "E";
    } else {
      pixelName << i % 128 + 1 << "W";
    }
    analysisManager->CreateNtupleDColumn(pixelName.str());
  }
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("VisitedVolumes",
                                "Visited volumes and corresponding energies");
  analysisManager->CreateNtupleIColumn("iD");
  analysisManager->CreateNtupleIColumn("classification");
  analysisManager->CreateNtupleDColumn("primary");
  analysisManager->CreateNtupleDColumn("currentEn");
  analysisManager->CreateNtupleDColumn("time");
  analysisManager->CreateNtupleSColumn("volume");
  analysisManager->FinishNtuple();

  for (G4int i = 0; i < 5; i++) {
    analysisManager->SetNtupleActivation(i, true);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDRunAction::~NDDRunAction() {
  delete runMessenger;
  delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDRunAction::BeginOfRunAction(const G4Run*) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->IsActive()) {
    const G4String& filenamec = analysisManager->GetFileName();  //Output name must be specified in the Macro
    analysisManager->OpenFile(filenamec);
  }

  // save Rndm status
  if (fSaveRndm > 0) {
    G4Random::showEngineStatus();
    G4Random::saveEngineStatus("beginOfRun.rndm");
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDRunAction::EndOfRunAction(const G4Run*) {
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

  // save Rndm status
  if (fSaveRndm == 1) {
    G4Random::showEngineStatus();
    G4Random::saveEngineStatus("endOfRun.rndm");
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
