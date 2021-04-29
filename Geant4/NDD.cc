#include "NDDDetectorConstruction.hh"
#include "NDDActionInitialization.hh"
#include "NDDPhysicsList.hh"
#include "NDDPixelReadOut.hh"

#include "G4ParallelWorldPhysics.hh"
#include "G4StepLimiterPhysics.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager * runManager = new G4MTRunManager;
  G4int nThreads = G4Threading::G4GetNumberOfCores();
  runManager->SetNumberOfThreads(nThreads);
#else
  G4RunManager * runManager = new G4RunManager;
#endif

  // Set mandatory initialization classes
  //
  // Detector construction
  G4String parallelWorldName = "ReadoutWorld";
  G4VUserDetectorConstruction* detector = new NDDDetectorConstruction;
  detector->RegisterParallelWorld(new NDDPixelReadOut(parallelWorldName, detector));
  runManager->SetUserInitialization(detector);
  // Physics list
  G4VModularPhysicsList* physicsList = new NDDPhysicsList;
  physicsList->RegisterPhysics(new G4ParallelWorldPhysics(parallelWorldName));
  runManager->SetUserInitialization(physicsList);
  // User action initialization
  runManager->SetUserInitialization(new NDDActionInitialization);

  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (argc!=1)   // batch mode
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
    }
  else
    {  // interactive mode : define UI session
     G4UIExecutive* ui = new G4UIExecutive(argc, argv);
     if (ui->IsGUI())
        UImanager->ApplyCommand("/control/execute init_vis.mac");
     ui->SessionStart();
     delete ui;
    }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

  delete visManager;
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
