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

B1SteppingAction::B1SteppingAction(B1EventAction* eventAction, B1RunAction* runAction, G4double CuDiam)
: G4UserSteppingAction(),
fEventAction(eventAction),
fScoringVolume(0),
runStepAction(runAction),
fCuDiam(CuDiam)
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
	
	//	if((NextVol && ThisVol->GetName()=="Resin" && NextVol->GetName()=="CMOS")|| (NextVol && ThisVol->GetName()=="World" && NextVol->GetName()=="CMOS")) { //what enters CMOS
	if((NextVol && ThisVol->GetName()=="Resin" && NextVol->GetName()=="CMOS")|| (NextVol && ThisVol->GetName()!="CMOS" && NextVol->GetName()=="CMOS")) { //what enters CMOS
		

		G4double edepKin = step->GetPostStepPoint()->GetKineticEnergy();
		//Fill vector
		(runStepAction->GetRunEnPre()).push_back(edepKin/keV);
		fEventAction->AddNoPre(1);
		(runStepAction->GetRunPart()).push_back(step->GetTrack()->GetDynamicParticle() ->GetPDGcode());
		fEventAction->AddEdkin(edepKin);
	}
	

	//Modified on 2017-11-17 by collamaf: now the condition works for both cases: with or without Cu collimator.
	//If there is not collimator save what goes from source to dummy. If there is a collimator save what goes from world (the hole) into dummy

	if( NextVol && ( (fCuDiam<0 &&  ( (ThisVol->GetName()=="SourceSR" && NextVol->GetName()=="Dummy") || (ThisVol->GetName()=="SourceDOTA" && NextVol->GetName()=="Dummy"))) || ( (fCuDiam>=0 &&   (ThisVol->GetName()=="World" && NextVol->GetName()=="Dummy") ) )) ) { //what actually exits the source
		
        //collamaf: to avoid double counting same track going back and forth, check if I already counted it
		if (fEventAction->GetStoreTrackID()==step->GetTrack()->GetTrackID()) { //if I already saw this track exiting the source...
			fEventAction->AddPassCounter(1);  //increase the counter
            
//			G4cout<<"CMOSDEBUG CONTROLLA "<<fEventAction->GetStoreTrackID()<<", PassCounter= "<<fEventAction->GetPassCounter()<<G4endl;
        }else {
            fEventAction->SetStoreTrackID(step->GetTrack()->GetTrackID());
//            G4cout<<"CMOSDEBUG PRIMO PASSAGGIO!! "<<fEventAction->GetStoreTrackID()<<", PassCounter= "<<fEventAction->GetPassCounter()<<G4endl;
//            if (fEventAction->GetPassCounter()!=0) G4cout<<"MERDAAAAA Primo passaggio di"<<fEventAction->GetStoreTrackID()<<" ma con PassCounter= "<<fEventAction->GetPassCounter()<<G4endl;
        }
		
		//		if (fEventAction->GetNSourceExit()>1) 	G4cout<<"CMOSDEBUG CONTROLLA"<<fEventAction->GetNSourceExit()<<G4endl;
		if (fEventAction->GetPassCounter()==0) {
			fEventAction->AddNSourceExit(1);
			(runStepAction->GetRunEnExit()).push_back(step->GetPostStepPoint()->GetKineticEnergy()/keV);
			(runStepAction->GetRunXExit()).push_back(step->GetPostStepPoint()->GetPosition().x()/mm);
			(runStepAction->GetRunYExit()).push_back(step->GetPostStepPoint()->GetPosition().y()/mm);
			(runStepAction->GetRunZExit()).push_back(step->GetPostStepPoint()->GetPosition().z()/mm);
			(runStepAction->GetRunCosXExit()).push_back(step->GetPreStepPoint()->GetMomentumDirection().x());
			(runStepAction->GetRunCosYExit()).push_back(step->GetPreStepPoint()->GetMomentumDirection().y());
			(runStepAction->GetRunCosZExit()).push_back(step->GetPreStepPoint()->GetMomentumDirection().z());
			(runStepAction->GetRunPartExit()).push_back(step->GetTrack()->GetDynamicParticle() ->GetPDGcode());
			(runStepAction->GetRunParentIDExit()).push_back(step->GetTrack()->GetParentID());
			(runStepAction->GetRunExitProcess().push_back((step->GetTrack()->GetCreatorProcess()->GetProcessType())));
		}
		
		/*
		 We have to use PreStepPoint to save the exit cosines, otherwise we already have particles flipped..
		 */
		
//		if (0&&step->GetPostStepPoint()->GetMomentumDirection().z()<0) G4cout<<"CMOSDEBUG COSZ NEGATIVO! pre "<< step->GetPreStepPoint()->GetMomentumDirection().z()<<", post "<< step->GetPostStepPoint()->GetMomentumDirection().z()<<G4endl;
		//		G4cout<<"debug! Nome: "<<step->GetTrack()->GetCreatorProcess()->GetProcessName()<<
		//	", Type= "<<  step->GetTrack()->GetCreatorProcess()->GetProcessType() <<", Sub Type= "<<  step->GetTrack()->GetCreatorProcess()->GetProcessSubType() <<G4endl;
	}
	
	if (!fScoringVolume) {
		const B1DetectorConstruction* detectorConstruction
		= static_cast<const B1DetectorConstruction*>
		(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
		fScoringVolume = detectorConstruction->GetScoringVolume();
	}
	
	if (0 && runStepAction->GetMotherIsotope() != 0 && runStepAction->GetMotherIsotope() !=1) G4cout<<"CMOSDEBUG PROVA STEPPING  MotherIsotope Val= "<< runStepAction->GetMotherIsotope()
	<<G4endl;
	
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
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

