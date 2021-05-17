#include "MicroElecPhysics.hh"

#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4LossTableManager.hh"
#include "G4EmParameters.hh"
#include "G4EmBuilder.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"
#include "G4PEEffectFluoModel.hh"
#include "G4KleinNishinaModel.hh"
#include "G4LivermorePhotoElectricModel.hh"

#include "G4eMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#include "G4MscStepLimitType.hh"
#include "G4UrbanMscModel.hh"
#include "G4DummyModel.hh"
#include "G4WentzelVIModel.hh"
#include "G4CoulombScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4Generator2BS.hh"
#include "G4SeltzerBergerModel.hh"

#include "G4eplusAnnihilation.hh"
#include "G4UAtomicDeexcitation.hh"

#include "G4ePairProduction.hh"
#include "G4ionIonisation.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4NuclearStopping.hh"

#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4GenericIon.hh"

#include "G4PhysicsListHelper.hh"
#include "G4BuilderType.hh"
#include "G4EmModelActivator.hh"
#include "G4GammaGeneralProcess.hh"

// Geant4-MicroElec MODELS

#include "G4MicroElecElasticModel_new.hh"
#include "G4MicroElecInelasticModel_new.hh"
//

#include "G4LossTableManager.hh"
#include "G4EmConfigurator.hh"
#include "G4VEmModel.hh"
#include "G4DummyModel.hh"
#include "G4eIonisation.hh"
#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4eMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#include "G4BraggModel.hh"
#include "G4BraggIonModel.hh"
#include "G4BetheBlochModel.hh"
#include "G4UrbanMscModel.hh"
#include "G4MollerBhabhaModel.hh"
#include "G4IonFluctuations.hh"
#include "G4UniversalFluctuation.hh"

#include "G4UAtomicDeexcitation.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(MicroElecPhysics);

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MicroElecPhysics::MicroElecPhysics(G4int ver,
							 const G4String&)
  : G4VPhysicsConstructor("MicroElecPhysics"), verbose(ver)
{
  G4EmParameters* param = G4EmParameters::Instance();
  param->SetDefaults();
  param->SetVerbose(verbose);
  param->SetMinEnergy(10*CLHEP::eV);
  param->SetLowestElectronEnergy(100*CLHEP::eV);
  param->SetNumberOfBinsPerDecade(20);
  param->ActivateAngularGeneratorForIonisation(true);
  param->SetUseMottCorrection(true);
  param->SetStepFunction(0.2, 100*CLHEP::um);
  param->SetStepFunctionMuHad(0.2, 50*CLHEP::um);
  param->SetStepFunctionLightIons(0.1, 20*CLHEP::um);
  param->SetStepFunctionIons(0.1, 1*CLHEP::um);
  param->SetMscStepLimitType(fUseDistanceToBoundary);
  param->SetMuHadLateralDisplacement(true);
  param->SetLateralDisplacementAlg96(true);
  param->SetUseICRU90Data(true);
  param->SetFluo(true);
  param->SetMaxNIELEnergy(1*CLHEP::MeV);
  SetPhysicsType(bElectromagnetic);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MicroElecPhysics::~MicroElecPhysics()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MicroElecPhysics::ConstructParticle()
{
  // minimal set of particles for EM physics
  G4EmBuilder::ConstructMinimalEmSet();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MicroElecPhysics::ConstructProcess()
{
	if(verbose > 1) {
    G4cout << "### " << GetPhysicsName() << " Construct Processes " << G4endl;
  }
  G4EmBuilder::PrepareEMPhysics();

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // processes used by several particles
  G4ePairProduction* ee = new G4ePairProduction();
  G4hMultipleScattering* hmsc = new G4hMultipleScattering("ionmsc");

  // nuclear stopping
  G4double nielEnergyLimit = G4EmParameters::Instance()->MaxNIELEnergy();
  G4NuclearStopping* pnuc = new G4NuclearStopping();
  pnuc->SetMaxKinEnergy(nielEnergyLimit);

  // Add gamma EM Processes
  G4ParticleDefinition* particle = G4Gamma::Gamma();

  G4PhotoElectricEffect* pee = new G4PhotoElectricEffect();
  pee->SetEmModel(new G4LivermorePhotoElectricModel());

  G4ComptonScattering* cs = new G4ComptonScattering();
  cs->SetEmModel(new G4KleinNishinaModel());

  if(G4EmParameters::Instance()->GeneralProcessActive()) {
    G4GammaGeneralProcess* sp = new G4GammaGeneralProcess();
    sp->AddEmProcess(pee);
    sp->AddEmProcess(cs);
    sp->AddEmProcess(new G4GammaConversion());
    sp->AddEmProcess(new G4RayleighScattering());
    G4LossTableManager::Instance()->SetGammaGeneralProcess(sp);
    ph->RegisterProcess(sp, particle);
  } else {
    ph->RegisterProcess(pee,particle);
    ph->RegisterProcess(cs, particle);
    ph->RegisterProcess(new G4GammaConversion(), particle);
    ph->RegisterProcess(new G4RayleighScattering(), particle);
  }

  // e-
  particle = G4Electron::Electron();

  G4eMultipleScattering* msc = new G4eMultipleScattering();
  G4eIonisation* eIoni = new G4eIonisation();

  G4eBremsstrahlung* brem = new G4eBremsstrahlung();
  G4SeltzerBergerModel* br1 = new G4SeltzerBergerModel();
  G4eBremsstrahlungRelModel* br2 = new G4eBremsstrahlungRelModel();
  br1->SetAngularDistribution(new G4Generator2BS());
  br2->SetAngularDistribution(new G4Generator2BS());
  brem->SetEmModel(br1);
  brem->SetEmModel(br2);
  br2->SetLowEnergyLimit(CLHEP::GeV);

  ph->RegisterProcess(msc, particle);
  ph->RegisterProcess(eIoni, particle);
  ph->RegisterProcess(brem, particle);
  ph->RegisterProcess(ee, particle);

  // e+
  particle = G4Positron::Positron();

  msc = new G4eMultipleScattering();
  eIoni = new G4eIonisation();

  brem = new G4eBremsstrahlung();
  br1 = new G4SeltzerBergerModel();
  br2 = new G4eBremsstrahlungRelModel();
  br1->SetAngularDistribution(new G4Generator2BS());
  br2->SetAngularDistribution(new G4Generator2BS());
  brem->SetEmModel(br1);
  brem->SetEmModel(br2);
  br2->SetLowEnergyLimit(CLHEP::GeV);

  ph->RegisterProcess(msc, particle);
  ph->RegisterProcess(eIoni, particle);
  ph->RegisterProcess(brem, particle);
  ph->RegisterProcess(ee, particle);
  ph->RegisterProcess(new G4eplusAnnihilation(), particle);

  // generic ion
  particle = G4GenericIon::GenericIon();
  G4ionIonisation* ionIoni = new G4ionIonisation();
  ionIoni->SetEmModel(new G4IonParametrisedLossModel());
  ph->RegisterProcess(hmsc, particle);
  ph->RegisterProcess(ionIoni, particle);
  ph->RegisterProcess(pnuc, particle);

  // muons, hadrons, ions
  G4EmBuilder::ConstructCharged(hmsc, pnuc, false);

	// **************************************
	// 2) Define processes for Target region
	// **************************************

	// STANDARD EM processes should be inactivated when corresponding MicroElec processes are used
	// - STANDARD EM e- processes are inactivated below 100 MeV
	// - STANDARD EM proton & ion processes are inactivated below standEnergyLimit
	//
	G4EmConfigurator* em_config = G4LossTableManager::Instance()->EmConfigurator();

	G4VEmModel* mod;
	// *** e-

	// ---> STANDARD EM processes are inactivated below 100 MeV

	G4UrbanMscModel* msc =  new G4UrbanMscModel();
	msc->SetActivationLowEnergyLimit(100*MeV);
	em_config->SetExtraEmModel("e-","msc",msc,"SiliconCrystal");

	mod = new G4MollerBhabhaModel();
	mod->SetActivationLowEnergyLimit(100*MeV);
	em_config->SetExtraEmModel("e-","eIoni",mod,"SiliconCrystal",0.0,10*TeV, new G4UniversalFluctuation());

	// ---> MicroElec processes activated

	mod = new G4MicroElecElasticModel_new();
	em_config->SetExtraEmModel("e-","e-_G4MicroElecElastic",mod,"SiliconCrystal",0.0,100*MeV);

	mod = new G4MicroElecInelasticModel_new();
	em_config->SetExtraEmModel("e-","e-_G4MicroElecInelastic",mod,"SiliconCrystal",16.7*eV,100*MeV);

	// *** proton

	// ---> STANDARD EM processes inactivated below standEnergyLimit

	// STANDARD msc is still active
	// Inactivate following STANDARD processes

	mod = new G4BraggModel();
	mod->SetActivationHighEnergyLimit(1*keV);
	em_config->SetExtraEmModel("proton","hIoni",mod,"Target",0.0,2*MeV, new G4IonFluctuations());

	mod = new G4BetheBlochModel();
	mod->SetActivationLowEnergyLimit(10*GeV);
	em_config->SetExtraEmModel("proton","hIoni",mod,"Target",2*MeV,10*TeV, new G4IonFluctuations());

	// ---> MicroElec processes activated
	mod = new G4MicroElecInelasticModel_new();
	mod->SetActivationLowEnergyLimit(1*keV);
	em_config->SetExtraEmModel("proton","p_G4MicroElecInelastic",mod,"SiliconCrystal",0.0,10*GeV);
	em_config->SetExtraEmModel("proton","p_G4MicroElecInelastic",new G4DummyModel,
					 "SiliconCrystal",10*GeV,10*TeV);

 // *** ion

	// ---> STANDARD EM processes inactivated below standEnergyLimit

	// STANDARD msc is still active
	// Inactivate following STANDARD processes

	mod = new G4BraggIonModel();
	mod->SetActivationHighEnergyLimit(1*keV);
	em_config->SetExtraEmModel("GenericIon","ionIoni",mod,"SiliconCrystal",0.0,2*MeV, new G4IonFluctuations());

	mod = new G4BetheBlochModel();
	mod->SetActivationLowEnergyLimit(10*GeV);
	em_config->SetExtraEmModel("GenericIon","ionIoni",mod,"SiliconCrystal",2*MeV,10*TeV, new G4IonFluctuations());

	// ---> MicroElec processes activated
	mod = new G4MicroElecInelasticModel_new();
	mod->SetActivationLowEnergyLimit(1*keV);
	em_config->SetExtraEmModel("GenericIon","ion_G4MicroElecInelastic",mod,
					 "SiliconCrystal",0.0,10*GeV);
	em_config->SetExtraEmModel("GenericIon","ion_G4MicroElecInelastic",new G4DummyModel,
					 "SiliconCrystal",10*GeV,10*TeV);

	// Deexcitation
	//
	G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
	G4LossTableManager::Instance()->SetAtomDeexcitation(de);
	de->SetFluo(true);
	de->SetAuger(true);
	de->SetPIXE(true);
	de->InitialiseForNewRun();

  // extra configuration
  G4EmModelActivator mact(GetPhysicsName());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
