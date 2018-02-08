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
// $Id: B1SteppingAction.cc 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class

#include "B1SteppingAction.hh"
#include "B1EventAction.hh"
#include "B1RunAction.hh"
#include "B1DetectorConstruction.hh"


#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

#include "B1Analysis.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::B1SteppingAction(B1EventAction* eventAction, B1RunAction* runAction)
: G4UserSteppingAction(),
fEventAction(eventAction),
fScoringVolume(0),
runStepAction(runAction)
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::~B1SteppingAction()
{}

//std::ofstream pixelOut("PixelTest.dat", std::ios::out);

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1SteppingAction::UserSteppingAction(const G4Step* step)
{
	

	G4VPhysicalVolume* ThisVol = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
	G4VPhysicalVolume* NextVol = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
	
	if (0 &&step->GetTrack()->GetCreatorProcess()) G4cout<<"MERDA "<<step->GetTrack()->GetCreatorProcess()->GetProcessType()<<G4endl;
	
	if( NextVol && ThisVol->GetName()=="Absorber" && NextVol->GetName()=="World" ) { //what actually exits the Absorber
			fEventAction->AddNSourceExit(1);
		
			(runStepAction->GetRunEnExit()).push_back(step->GetPostStepPoint()->GetKineticEnergy()/MeV);
			(runStepAction->GetRunXExit()).push_back(step->GetPostStepPoint()->GetPosition().x()/mm);
			(runStepAction->GetRunYExit()).push_back(step->GetPostStepPoint()->GetPosition().y()/mm);
			(runStepAction->GetRunZExit()).push_back(step->GetPostStepPoint()->GetPosition().z()/mm);
			(runStepAction->GetRunCosXExit()).push_back(step->GetPreStepPoint()->GetMomentumDirection().x());
			(runStepAction->GetRunCosYExit()).push_back(step->GetPreStepPoint()->GetMomentumDirection().y());
		
			(runStepAction->GetRunCosZExit()).push_back(step->GetPreStepPoint()->GetMomentumDirection().z());
			(runStepAction->GetRunPartExit()).push_back(step->GetTrack()->GetDynamicParticle() ->GetPDGcode());
		
			(runStepAction->GetRunParentIDExit()).push_back(step->GetTrack()->GetParentID());
		
//			(runStepAction->GetRunExitProcess().push_back((step->GetTrack()->GetCreatorProcess()->GetProcessType())));
		/**/
		
	}
#if 0
	if (!fScoringVolume) {
		const B1DetectorConstruction* detectorConstruction
		= static_cast<const B1DetectorConstruction*>
		(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
		fScoringVolume = detectorConstruction->GetScoringVolume();
	}
	
	// get volume of the current step
	G4LogicalVolume* volume
	= step->GetPreStepPoint()->GetTouchableHandle()
	->GetVolume()->GetLogicalVolume();

	// check if we are in scoring volume
	if (volume== fScoringVolume) {
		//pixel information collection
		G4int CopyNB=step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
		fEventAction->AddNo(1);
		
		G4ThreeVector pixCenter;
		G4TouchableHandle touchHandle =step->GetPreStepPoint()->GetTouchableHandle();
		G4ThreeVector vec_origin(0.,0.,0.);
		G4ThreeVector globalPos = touchHandle->GetHistory()-> GetTopTransform().Inverse().TransformPoint(vec_origin);
		pixCenter = globalPos;
		
		if (CopyNB>0) {
			//fill vectors
			(runStepAction->GetRunPixNo()).push_back(CopyNB);
//			(runStepAction->GetRunPixEneDep()).push_back(step->GetTotalEnergyDeposit()/keV);
			(runStepAction->GetRunPixXpos()).push_back(pixCenter.getX()/mm);
			(runStepAction->GetRunPixYpos()).push_back(pixCenter.getY()/mm);
		}
		
		// collect energy deposited in this step
		G4StepPoint* postPoint = step->GetPostStepPoint();
		G4double edepStep = step->GetTotalEnergyDeposit();
		G4ThreeVector post=postPoint->GetPosition();

		//Fill vector
		(runStepAction->GetRunEnCmos()).push_back(step->GetTotalEnergyDeposit()/keV);
		(runStepAction->GetRunXCmos()).push_back(step->GetPreStepPoint()->GetPosition().x()/mm);
		(runStepAction->GetRunYCmos()).push_back(step->GetPreStepPoint()->GetPosition().y()/mm);
		(runStepAction->GetRunZCmos()).push_back(step->GetPreStepPoint()->GetPosition().z()/mm);
		(runStepAction->GetRunPartCmos()).push_back(step->GetTrack()->GetDynamicParticle() ->GetPDGcode());
		
		//Collect deposited energy in CMOS  due to Sr electons
		if (runStepAction->GetMotherIsotope() == 0 ) {  //if son of Sr
			fEventAction->AddEdepSr(step->GetTotalEnergyDeposit());
		}

		//Collect deposited energy in CMOS due to Y electons
		if (runStepAction->GetMotherIsotope() == 1 ) {  //if son of Y
			fEventAction->AddEdepY(step->GetTotalEnergyDeposit());
		}
		
		fEventAction->AddEdep(edepStep);
	}
#endif
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

