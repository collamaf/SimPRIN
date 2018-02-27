//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: exampleB1.cc 86065 2014-11-07 08:51:15Z gcosmo $
//
/// \file exampleB1.cc
/// \brief Main program of the B1 example

#include "B1DetectorConstruction.hh"
#include "B1ActionInitialization.hh"


#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "QBBC.hh"
#include "B1PhysicsList.hh"

#include "G4SystemOfUnits.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"
#include "G4StepLimiter.hh"
#include "G4UserLimits.hh"
#include "G4StepLimiterPhysics.hh"
//#include "G4ScoringManager.hh"

#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
	
	
	// Detect interactive mode (if no arguments) and define UI session
	G4UIExecutive* ui = 0;
	if ( argc == 3) {
		ui = new G4UIExecutive(argc, argv);
	}
	
	
	// Fluor concentration: to be red: one F atom every XX PMMA molecules
	// -1 means CaF
	// -2 means pure F19
	// -10015 means simple MC with Eproton=1.5MeV
	// -10020 means simple MC with Eproton=2MeV
	// -10025 means simple MC with Eproton=2.5MeV
	// -10030 means simple MC with Eproton=3MeV
	// -10035 means simple MC with Eproton=3.5MeV
	// -10040 means simple MC with Eproton=4MeV
	
	// ./exampleB1 1 -1 ../run1.mac
	// ./exampleB1 {TumorFlag} {FluorFraction} ../run1.mac

	//	G4double Fluorfracion=6.7e13;
	//	G4double FluorFracion=.1; //Now is the percentage of F19 in PMMA-F! e.g. 10 means 10%
	G4double FluorFracion=2; //Now is the number of F19 atoms per each PMMA molecule
	G4bool RealBeamFlag=false;
	G4int TumorFlag=true;
	
	
	if ( argc >1 ) {
		//    x0Scan=(*argv[2]-48)*mm;
		TumorFlag=strtod (argv[1], NULL);
		FluorFracion=strtod (argv[2], NULL);
		
		G4cout<<"DEBUG Initial parameter check TumorFlag= "<<TumorFlag<<G4endl;
		G4cout<<"DEBUG Initial parameter check FluorFracion= "<<FluorFracion<<G4endl;
	
	}
	


	G4String FileNamePrim;
	FileNamePrim="PrimariesPRIN";
	FileNamePrim.append(+ ".dat");
	std::ofstream primFile(FileNamePrim, std::ios::out);
	
	G4cout<<"#### DEBUG ##### "<<G4endl;
	G4cout<<"FluorFraction= "<<FluorFracion<<G4endl;
	G4cout<<"RealBeamFlag= "<<RealBeamFlag<<G4endl;
	G4cout<<"TumorFlag= "<<TumorFlag<<G4endl;

	G4String FileNameOut;
	FileNameOut="PRINmc";
	
	if (argc==3) FileNameOut="VIS";
	
	if (TumorFlag) {
		G4cout<<"Required tumor volume"<<G4endl;
		FileNameOut.append("YT");
		if (FluorFracion==-2) FileNameOut.append(+ "_AllF");
		else if (FluorFracion==-1) FileNameOut.append(+ "_AllCaF");
		else FileNameOut.append(std::to_string((G4int)(FluorFracion)) +"_pF");
	}
	else {
		FileNameOut.append("NT");
		G4cout<<"NOT Required tumor volume"<<G4endl;
	}
	if (FluorFracion<=-10000) {
		FileNameOut="PRINmcSimple_";
		FileNameOut.append(std::to_string((G4int)(fabs(FluorFracion))%100*100) +"keV");
	}
	if (argc==3) FileNameOut="VIS";
	FileNameOut.append(+ ".root");
	
	
	// Choose the Random engine
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	
	// Construct the default run manager
	//
	//#ifdef G4MULTITHREAD
	//  G4MTRunManager* runManager = new G4MTRunManager;
	//#else
	//G4RunManager* runManager = new G4RunManager;
	//#endif
	
	
	//#ifdef G4MULTITHREADED
#if 0
	G4MTRunManager* runManager = new G4MTRunManager;
//	runManager->SetNumberOfThreads( G4Threading::G4GetNumberOfCores() );
	runManager->SetNumberOfThreads(5 );
#else
	G4RunManager* runManager = new G4RunManager;
#endif
//	G4ScoringManager::GetScoringManager();
	
	
	// Physics list
	G4VModularPhysicsList* physicsList = new QBBC;
	physicsList->SetVerboseLevel(1);
	runManager->SetUserInitialization(physicsList);
	// Set mandatory initialization classes
	// Detector construction
	runManager->SetUserInitialization(new B1DetectorConstruction(TumorFlag, FluorFracion)); //DetectorConstruction needs to know if it is a SrSource to place the right geometry
	
	
	
	//  runManager->SetUserInitialization(new B1PhysicsList);
	
	//B1PhysicsList* physicsList=new B1PhysicsList;
	//	physicsList->RegisterPhysics(new G4StepLimiterPhysics());
	//runManager->SetUserInitialization(physicsList);
	
	// User action initialization
	//	runManager->SetUserInitialization(new B1ActionInitialization(x0Scan, ZValue, CuDiam, FilterFlag, primFile, TBRvalue,SourceSelect, SourceSelect));
	runManager->SetUserInitialization(new B1ActionInitialization(primFile, FluorFracion, RealBeamFlag, FileNameOut));
	
	// Initialize visualization
	//
	G4VisManager* visManager = new G4VisExecutive;
	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
	// G4VisManager* visManager = new G4VisExecutive("Quiet");
	visManager->Initialize();
	
	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	
	// Process macro or start UI session
	//
	
	if ( ! ui ) {
		// batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[3];
		UImanager->ApplyCommand(command+fileName);
	}
	else {
		// interactive mode
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();
		delete ui;
	}
	
	delete visManager;
	delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
