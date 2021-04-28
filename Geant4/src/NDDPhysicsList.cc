#include "NDDPhysicsList.hh"
#include "NDDPhysicsListMessenger.hh"

#include "PhysListEmStandard.hh"
#include "PhysListEmStandardSS.hh"   //single scattering model
#include "PhysListEmStandardGS.hh"   //Goudsmit-Saunderson
#include "PhysListEmStandardWVI.hh"  //Wentzel-VI MSC model

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmParameters.hh"
#include "G4IonPhysics.hh"
#include "G4IonElasticPhysics.hh"

#include "G4IonConstructor.hh"
#include "G4NuclideTable.hh"
#include "G4LeptonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"

#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Geantino.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Neutron.hh"

#include "G4StepLimiter.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDPhysicsList::NDDPhysicsList() : G4VModularPhysicsList() {
  defaultCutValue = 100. * um;
  cutForGamma = defaultCutValue;
  cutForElectron = defaultCutValue;
  cutForPositron = defaultCutValue;

  pMessenger = new NDDPhysicsListMessenger(this);

  G4int verb = 1;
  SetVerboseLevel(verb);

  // EM physics
  RegisterPhysics(new G4EmLivermorePhysics(1));
  G4EmParameters* param = G4EmParameters::Instance();
  param->SetAugerCascade(true);
  param->SetStepFunction(1., 1*CLHEP::mm);
  param->SetStepFunctionMuHad(1., 1*CLHEP::mm);

  // Ion Elastic scattering
  RegisterPhysics( new G4IonElasticPhysics(verb));

  // Ion Inelastic physics
  RegisterPhysics( new G4IonPhysics(verb));
  ////RegisterPhysics( new G4IonINCLXXPhysics(verb));

  // Decay physics
  RegisterPhysics(new G4DecayPhysics());
  RegisterPhysics(new G4RadioactiveDecayPhysics());
  // // Radioactive decay
  // RegisterPhysics(new BiasedRDPhysics());

  // Gamma-Nuclear Physics
  G4EmExtraPhysics* gnuc = new G4EmExtraPhysics(verb);
  gnuc->ElectroNuclear(false);
  gnuc->MuonNuclear(false);
  RegisterPhysics(gnuc);

  //add new units for radioActive decays
  //
  new G4UnitDefinition( "millielectronVolt", "meV", "Energy", 1.e-3*eV);
  // Mandatory for G4NuclideTable
  // Half-life threshold must be set small or many short-lived isomers
  // will not be assigned life times (default to 0)
  G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(0.1*picosecond);
  G4NuclideTable::GetInstance()->SetLevelTolerance(1.0*eV);

  // add new units for radioActive decays
  //
  const G4double minute = 60 * second;
  const G4double hour = 60 * minute;
  const G4double day = 24 * hour;
  const G4double year = 365 * day;
  new G4UnitDefinition("minute", "min", "Time", minute);
  new G4UnitDefinition("hour", "h", "Time", hour);
  new G4UnitDefinition("day", "d", "Time", day);
  new G4UnitDefinition("year", "y", "Time", year);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NDDPhysicsList::~NDDPhysicsList() {
  delete pMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDPhysicsList::ConstructParticle() {
  // pseudo-particles
  G4Geantino::GeantinoDefinition();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BosonConstructor pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDPhysicsList::ReplaceEMPhysicsList(const G4String& name) {
  if (verboseLevel > 1) {
    G4cout << "NDDPhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
  }

  if (name == "local") {
    ReplacePhysics(new PhysListEmStandard(name));
  } else if (name == "emstandard_opt0") {
    ReplacePhysics(new G4EmStandardPhysics(1));
  } else if (name == "emstandard_opt1") {
    ReplacePhysics(new G4EmStandardPhysics_option1());
  } else if (name == "emstandard_opt2") {
    ReplacePhysics(new G4EmStandardPhysics_option2());
  } else if (name == "emstandard_opt3") {
    ReplacePhysics(new G4EmStandardPhysics_option3());
  } else if (name == "standardSS") {
    ReplacePhysics(new PhysListEmStandardSS(name));
  } else if (name == "standardWVI") {
    ReplacePhysics(new PhysListEmStandardWVI(name));
  } else if (name == "standardGS") {
    ReplacePhysics(new PhysListEmStandardGS(name));
  } else if (name == "emlivermore") {
    ReplacePhysics(new G4EmLivermorePhysics());
  } else if (name == "empenelope") {
    ReplacePhysics(new G4EmPenelopePhysics());
  } else {
    G4cout << "NDDPhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined" << G4endl;
    ReplacePhysics(new G4EmStandardPhysics(1));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDPhysicsList::SetCuts() {
  SetCutsWithDefault();
  if (verboseLevel > 0) {
    G4cout << "NDDPhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue, "Length") << G4endl;
  }
  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");

  if (verboseLevel > 0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDPhysicsList::SetCutForGamma(G4double cut) {
  cutForGamma = cut;
  G4cout << "Setting cut for gamma " << G4BestUnit(cut, "Length") << G4endl;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDPhysicsList::SetCutForElectron(G4double cut) {
  cutForElectron = cut;
  G4cout << "Setting cut for electron " << G4BestUnit(cut, "Length") << G4endl;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NDDPhysicsList::SetCutForPositron(G4double cut) {
  cutForPositron = cut;
  G4cout << "Setting cut for positron " << G4BestUnit(cut, "Length") << G4endl;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
