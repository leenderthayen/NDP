#include "NDDAnalysisManager.hh"

#include "G4SystemOfUnits.hh"

NDDAnalysisManager::NDDAnalysisManager() {
  // analysis manager only gets created first time this function is called,
  // need it to happen before we start a run so that the analysis commands
  // are available
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);
}

NDDAnalysisManager::~NDDAnalysisManager() {}

void NDDAnalysisManager::Book() {
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);
  #ifdef G4MULTITHREADED
    analysisManager->SetNtupleMerging(true);
  #endif

  analysisManager->SetHistoDirectoryName("hist");
  analysisManager->SetNtupleDirectoryName("ntuple");
  //analysisManager->SetActivation(true);

  const G4String& filenamec = analysisManager->GetFileName();  //Output name must be specified in the Macro

  analysisManager->OpenFile(filenamec);

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
  analysisManager->CreateNtupleIColumn("eventID");
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
  analysisManager->CreateNtupleIColumn("eventID");
  analysisManager->CreateNtupleIColumn("classification");
  analysisManager->CreateNtupleDColumn("angleSourceOut");
  analysisManager->CreateNtupleDColumn("angleOut");
  analysisManager->CreateNtupleDColumn("timeSi");
  analysisManager->CreateNtupleDColumn("poeXSi");
  analysisManager->CreateNtupleDColumn("poeYSi");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("hits", "Detector hits");
  analysisManager->CreateNtupleIColumn("eventID");
  analysisManager->CreateNtupleIColumn("trackID");
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
  analysisManager->CreateNtupleIColumn("pixelNumber");
  analysisManager->CreateNtupleSColumn("particle");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("pixelEnergies", "Pixel hits");
  analysisManager->CreateNtupleIColumn("eventID");
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
  analysisManager->CreateNtupleIColumn("eventID");
  analysisManager->CreateNtupleIColumn("classification");
  analysisManager->CreateNtupleDColumn("primary");
  analysisManager->CreateNtupleDColumn("currentEn");
  analysisManager->CreateNtupleDColumn("time");
  analysisManager->CreateNtupleSColumn("volume");
  analysisManager->CreateNtupleSColumn("particle");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("PrimaryEnergies", "Particles created and their initial properties");
  analysisManager->CreateNtupleIColumn("eventID");
  analysisManager->CreateNtupleIColumn("trackID");
  analysisManager->CreateNtupleIColumn("classification");
  analysisManager->CreateNtupleDColumn("energy");
  analysisManager->CreateNtupleDColumn("time");
  analysisManager->CreateNtupleDColumn("x");
  analysisManager->CreateNtupleDColumn("y");
  analysisManager->CreateNtupleDColumn("z");
  analysisManager->CreateNtupleSColumn("process");
  analysisManager->CreateNtupleSColumn("name");
  analysisManager->FinishNtuple();
}

void NDDAnalysisManager::Save() {
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}

void NDDAnalysisManager::FillEnergyTuple(
    G4int iD, G4int classification, G4double enPrimary, G4double enSi,
    G4double enDead, G4double enFoil, G4double enCarrier, G4double enSourceHolder,
    G4double bremsstrahlungLoss) {

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
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

void NDDAnalysisManager::FillSpacetimeTuple(
    G4int iD, G4int classification, G4double angleSourceOut,
    G4double angleSiOut, G4double timeSi, G4double poeXSi, G4double poeYSi) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillNtupleIColumn(1, 0, iD);
  analysisManager->FillNtupleIColumn(1, 1, classification);
  analysisManager->FillNtupleDColumn(1, 2, angleSourceOut);
  analysisManager->FillNtupleDColumn(1, 3, angleSiOut);
  analysisManager->FillNtupleDColumn(1, 4, timeSi / ns);
  analysisManager->FillNtupleDColumn(1, 5, poeXSi / mm);
  analysisManager->FillNtupleDColumn(1, 6, poeYSi / mm);
  analysisManager->AddNtupleRow(1);
}

void NDDAnalysisManager::FillHitsTuple(G4int eventID, G4int trackID, G4int classification,
                                   G4double enPrimary, G4double eDep,
                                   G4double x, G4double y, G4double z,
                                   G4double px, G4double py, G4double pz,
                                   G4double time, G4int pixelNumber, G4String particle) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillNtupleIColumn(2, 0, eventID);
  analysisManager->FillNtupleIColumn(2, 1, trackID);
  analysisManager->FillNtupleIColumn(2, 2, classification);
  analysisManager->FillNtupleDColumn(2, 3, enPrimary / keV);
  analysisManager->FillNtupleDColumn(2, 4, eDep / keV);
  analysisManager->FillNtupleDColumn(2, 5, x / mm);
  analysisManager->FillNtupleDColumn(2, 6, y / mm);
  analysisManager->FillNtupleDColumn(2, 7, z / mm);
  analysisManager->FillNtupleDColumn(2, 8, px);
  analysisManager->FillNtupleDColumn(2, 9, py);
  analysisManager->FillNtupleDColumn(2, 10, pz);
  analysisManager->FillNtupleDColumn(2, 11, time / ns);
  analysisManager->FillNtupleIColumn(2, 12, pixelNumber);
  analysisManager->FillNtupleSColumn(2, 13, particle);
  analysisManager->AddNtupleRow(2);
}

void NDDAnalysisManager::FillPixelTuple(G4int iD, G4int classification,
                                    G4double enPrimary,
                                    std::vector<G4double>& eDep) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillNtupleIColumn(3, 0, iD);
  analysisManager->FillNtupleIColumn(3, 1, classification);
  analysisManager->FillNtupleDColumn(3, 2, enPrimary);
  for (unsigned long i = 0; i < eDep.size(); i++) {
    analysisManager->FillNtupleDColumn(3, i + 3, eDep[i] / keV);
  }
  analysisManager->AddNtupleRow(3);
}

void NDDAnalysisManager::FillVolumesTuple(G4int iD, G4int classification,
                                      G4double primaryEn, G4double currentEn,
                                      G4double time, G4String volume, G4String particle) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillNtupleIColumn(4, 0, iD);
  analysisManager->FillNtupleIColumn(4, 1, classification);
  analysisManager->FillNtupleDColumn(4, 2, primaryEn / keV);
  analysisManager->FillNtupleDColumn(4, 3, currentEn / keV);
  analysisManager->FillNtupleDColumn(4, 4, time / ns);
  analysisManager->FillNtupleSColumn(4, 5, volume);
  analysisManager->FillNtupleSColumn(4, 6, particle);
  analysisManager->AddNtupleRow(4);
}

void NDDAnalysisManager::FillPrimaryTuple(G4int eventID, G4int trackID, G4int classification,
                                      G4double energy, G4double time, G4double x, G4double y,
                                      G4double z, G4String process, G4String name) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillNtupleIColumn(5, 0, eventID);
  analysisManager->FillNtupleIColumn(5, 1, trackID);
  analysisManager->FillNtupleIColumn(5, 2, classification);
  analysisManager->FillNtupleDColumn(5, 3, energy / keV);
  analysisManager->FillNtupleDColumn(5, 4, time / ns);
  analysisManager->FillNtupleDColumn(5, 5, x / mm);
  analysisManager->FillNtupleDColumn(5, 6, y / mm);
  analysisManager->FillNtupleDColumn(5, 7, z / mm);
  analysisManager->FillNtupleSColumn(5, 8, process);
  analysisManager->FillNtupleSColumn(5, 9, name);
  analysisManager->AddNtupleRow(5);
}

void NDDAnalysisManager::FillH1Hist(G4int ih, G4double xbin, G4double weight) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillH1(ih, xbin, weight);
}

void NDDAnalysisManager::FillH2Hist(G4int ih, G4double xbin, G4double ybin,
                                G4double weight) {
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillH2(ih, xbin, ybin, weight);
}
