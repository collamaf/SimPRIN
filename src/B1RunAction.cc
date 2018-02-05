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
// $Id: B1RunAction.cc 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file B1RunAction.cc
/// \brief Implementation of the B1RunAction class

#include "B1RunAction.hh"
#include "B1PrimaryGeneratorAction.hh"
#include "B1DetectorConstruction.hh"
#include "B1SteppingAction.hh"


#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "B1Analysis.hh"

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1RunAction::B1RunAction(G4double x0, G4double ZValue, G4double CuDiam, G4int FilterFlag, G4double TBR, G4int SourceSelect, G4int SensorChoice)
: G4UserRunAction(),
fEdep("Edep", 0.),
fEdep2("Edep2", 0.),
fEdkin("Edkin", 0.)
, fX0Scan(x0)
, fZValue(ZValue)
, fCuDiam(CuDiam)
, fFilterFlag(FilterFlag)
, fTBR(TBR)
, fSourceSelect(SourceSelect)
, fSensorChoice(SensorChoice)

{
	// Register accumulable to the accumulable manager
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->RegisterAccumulable(fEdep);
	accumulableManager->RegisterAccumulable(fEdep2);
	accumulableManager->RegisterAccumulable(fEdkin);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1RunAction::~B1RunAction()
{
	delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1RunAction::BeginOfRunAction(const G4Run* run)
{
	// inform the runManager to save random number seed
	G4RunManager::GetRunManager()->SetRandomNumberStore(false);
	
	// reset accumulable to their initial values
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Reset();
	
	
	CreateHistogram();
	
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	
	nbEventInRun = run->GetNumberOfEventToBeProcessed();
	analysisManager->FillNtupleIColumn(0,22, nbEventInRun);

	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1RunAction::EndOfRunAction(const G4Run* run)
{
	G4int nofEvents = run->GetNumberOfEvent();
	if (nofEvents == 0) return;
	
	// Merge accumulable
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Merge();
	
	
	// Compute dose = total energy deposit in a run and its variance
	//
	G4double edep  = fEdep.GetValue();
	G4double edep2 = fEdep2.GetValue();
	
	//G4double edkin  = fEdkin.GetValue();
	
	G4double rms = edep2 - edep*edep/nofEvents;
	if (rms > 0.) rms = std::sqrt(rms); else rms = 0.;
	
	const B1DetectorConstruction* detectorConstruction
	= static_cast<const B1DetectorConstruction*>
	(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	G4double mass = detectorConstruction->GetScoringVolume()->GetMass();
	G4double dose = edep/mass;
	G4double rmsDose = rms/mass;
	
	// Run conditions
	//  note: There is no primary generator action object for "master"
	//        run manager for multi-threaded mode.
	const B1PrimaryGeneratorAction* generatorAction
	= static_cast<const B1PrimaryGeneratorAction*>
	(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	G4String runCondition;
	if (generatorAction)
	{
		const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
		runCondition += particleGun->GetParticleDefinition()->GetParticleName();
		runCondition += " of ";
		G4double particleEnergy = particleGun->GetParticleEnergy();
		runCondition += G4BestUnit(particleEnergy,"Energy");
	}
	
	// Print
	//
	if (IsMaster()) {
		G4cout
		<< G4endl
		<< "--------------------End of Global Run-----------------------";
	}
	else {
		G4cout
		<< G4endl
		<< "--------------------End of Local Run------------------------";
	}
	
	G4cout
	<< G4endl
	<< " The run consists of " << nofEvents << " "<< runCondition
	<< G4endl
	<< G4endl
	// << " N. of hits in scoring volume :" << fnofHits
	// << G4endl
	<< " Cumulated dose per run, in scoring volume : "
	<< G4BestUnit(dose,"Dose") << " rms = " << G4BestUnit(rmsDose,"Dose")
	<< G4endl
	<< "------------------------------------------------------------"
	<< G4endl
	<< G4endl;
	
	///////////////
	// Write Histo
	//
	WriteHistogram();
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1RunAction::AddEdep(G4double edep)
{
	fEdep  += edep;
	fEdep2 += edep*edep;
}

void B1RunAction::AddEdkin(G4double edkin)
{
	fEdkin  += edkin;
}


void B1RunAction::CreateHistogram()
{
	// Book histograms, ntuple
	//	G4cout << "##### Create analysis manager " << "  " << this << G4endl;
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	
	G4cout << "Using " << analysisManager->GetType() << " analysis manager" << G4endl;
	
	// Create directories
	analysisManager->SetVerboseLevel(1);
	
	// Open an output file
	G4String fileNameBase = "CMOSmc";
	G4String fileName;
	
	if (fCuDiam>=0){
		fileName= fileNameBase + "X"+  std::to_string((G4int)fX0Scan) + "_Z" + std::to_string((G4int)fZValue) + "_CuD" + std::to_string((G4int)fCuDiam) + "_Fil" + std::to_string((G4int)fFilterFlag) + "_TBR" + std::to_string((G4int)(10*fTBR));
	}
	else {
		fileName= fileNameBase + "X"+  std::to_string((G4int)fX0Scan) + "_Z" + std::to_string((G4int)fZValue) + "_NOCuD" + "_Fil" + std::to_string((G4int)fFilterFlag) + "_TBR" + std::to_string((G4int)(10*fTBR));
	}
	
	if (fSourceSelect==1) fileName.append("_PSr");
	if (fSourceSelect==2) fileName.append("_ExtSr");
	if (fSourceSelect==3) fileName.append("_ExtY");
	if (fSensorChoice==1) fileName.append("_011");
	if (fSensorChoice==2) fileName.append("_115");

	analysisManager->OpenFile(fileName);

	// Creating ntuple
	
	analysisManager->CreateNtuple("B1", "physics");
	analysisManager->CreateNtuple("Source", "SourceNtuple");
	
	analysisManager->CreateNtupleDColumn(0,"Eabs");                           //0
	analysisManager->CreateNtupleDColumn(0,"EabsComp", RunVectorEAbsComp); //1
	analysisManager->CreateNtupleDColumn(0,"PreCmosTrackN");                  //2
	analysisManager->CreateNtupleDColumn(0,"PreCmosPart", RunVectorPart); //3
	analysisManager->CreateNtupleDColumn(0,"PreCmosEn", RunVectorEnPre); //4
	analysisManager->CreateNtupleDColumn(0,"InCmosTrackN");                   //5
	analysisManager->CreateNtupleDColumn(0,"InCmosPart", RunVectorPartCmos); //6
	analysisManager->CreateNtupleDColumn(0,"InCmosEn", RunVectorEnCmos); //7
	analysisManager->CreateNtupleDColumn(0,"InCmosX", RunVectorXCmos); //8
	analysisManager->CreateNtupleDColumn(0,"InCmosY", RunVectorYCmos); //9
	analysisManager->CreateNtupleDColumn(0,"InCmosZ", RunVectorZCmos); //10
//	analysisManager->CreateNtupleDColumn(0,"InCmosEnSr");
//	analysisManager->CreateNtupleDColumn(0,"InCmosEnY");
	analysisManager->CreateNtupleDColumn(0,"PixelID", RunVectorPixNo); //11
//	analysisManager->CreateNtupleDColumn(0,"EDepInPixel", RunVectorPixEneDep); //12
	analysisManager->CreateNtupleDColumn(0,"PixXPos", RunVectorPixXpos); //13
	analysisManager->CreateNtupleDColumn(0,"PixYPos", RunVectorPixYpos); //14
	analysisManager->CreateNtupleDColumn(0,"SourceX");                           //14
	analysisManager->CreateNtupleDColumn(0,"SourceY");                           //15
	analysisManager->CreateNtupleDColumn(0,"SourceZ");                           //16

	analysisManager->CreateNtupleDColumn(0,"SourceCosX", RunVectorCosX); //17
	analysisManager->CreateNtupleDColumn(0,"SourceCosY", RunVectorCosY); //18
	analysisManager->CreateNtupleDColumn(0,"SourceCosZ", RunVectorCosZ); //19

	analysisManager->CreateNtupleDColumn(0,"SourceEne", RunVectorEnGen); //20
	analysisManager->CreateNtupleDColumn(0,"SourceIsotope", RunVectorIsotopeGen); //21
	analysisManager->CreateNtupleIColumn(0,"Nev");							//22


	/*
	analysisManager->CreateNtupleDColumn(0,"SourceCosX");                           //19
	analysisManager->CreateNtupleDColumn(0,"SourceCosY");                           //20
	analysisManager->CreateNtupleDColumn(0,"SourceCosZ");                           //21
	analysisManager->CreateNtupleDColumn(0,"SourceEne");                           //22
	analysisManager->CreateNtupleDColumn(0,"SourceIsotope");                           //23
	*/
	
	
	analysisManager->CreateNtupleDColumn(1,"AllX");                           //0
	analysisManager->CreateNtupleDColumn(1,"AllY");                           //1
	analysisManager->CreateNtupleDColumn(1,"AllZ");                           //2
//	analysisManager->CreateNtupleDColumn(1,"AllCosX");                           //3
//	analysisManager->CreateNtupleDColumn(1,"AllCosY");                           //4
//	analysisManager->CreateNtupleDColumn(1,"AllCosZ");                           //5
	analysisManager->CreateNtupleDColumn(1,"AllCosX", RunVectorCosX);                           //3
	analysisManager->CreateNtupleDColumn(1,"AllCosY", RunVectorCosY);                           //4
	analysisManager->CreateNtupleDColumn(1,"AllCosZ", RunVectorCosZ);                           //5
	
	//	analysisManager->CreateNtupleDColumn(1,"AllEne");                           //6
	analysisManager->CreateNtupleDColumn(1,"AllEne", RunVectorEnGen);                           //6
//	analysisManager->CreateNtupleDColumn(1,"AllIsotope");                           //7
	analysisManager->CreateNtupleDColumn(1,"AllIsotope", RunVectorIsotopeGen);                           //7
	analysisManager->CreateNtupleDColumn(1,"ExitX", RunVectorXExit);                           //8
	analysisManager->CreateNtupleDColumn(1,"ExitY", RunVectorYExit);                           //9
	analysisManager->CreateNtupleDColumn(1,"ExitZ", RunVectorZExit);                           //10
	analysisManager->CreateNtupleDColumn(1,"ExitCosX", RunVectorCosXExit);                           //11
	analysisManager->CreateNtupleDColumn(1,"ExitCosY", RunVectorCosYExit);                           //12
	analysisManager->CreateNtupleDColumn(1,"ExitCosZ", RunVectorCosZExit);                           //13
	
	analysisManager->CreateNtupleDColumn(1,"ExitEne", RunVectorEnExit);                           //14
	analysisManager->CreateNtupleDColumn(1,"ExitPart", RunVectorPartExit);                           //15
	analysisManager->CreateNtupleDColumn(1,"ExitParentID", RunVectorParentIDExit);                           //16
	analysisManager->CreateNtupleIColumn(1,"ExitProcess", RunExitProcess); //17
	analysisManager->CreateNtupleDColumn(1,"ExitTrackN"); //18
	//	analysisManager->CreateNtupleDColumn(1,"ExitProcess", RunVectorParentIDExit); //16

	analysisManager->FinishNtuple(0);
	analysisManager->FinishNtuple(1);
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void B1RunAction::WriteHistogram()
{
	
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	
	// save histograms
	//
	analysisManager->Write();
	analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

