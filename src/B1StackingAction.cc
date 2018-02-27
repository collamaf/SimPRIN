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
// $Id: B3StackingAction.cc 66536 2012-12-19 14:32:36Z ihrivnac $
// 
/// \file B3StackingAction.cc
/// \brief Implementation of the B3StackingAction class

#include "B1StackingAction.hh"
#include "B1RunAction.hh"
#include "B1EventAction.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"

#include "G4Track.hh"
#include "G4NeutrinoE.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1StackingAction::B1StackingAction(B1RunAction* runAction, B1EventAction* EventAction)
:runStackAction(runAction), fEventAction(EventAction)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1StackingAction::~B1StackingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack
B1StackingAction::ClassifyNewTrack(const G4Track* track)
{
	G4int debug=1;
	
	/*
	 
	 C11: 1000060110
	 C12: 1000060120
	 N12: 1000070120
	 O16: 1000080160
	 
	 
	 */
	
	const G4VProcess* creator=track->GetCreatorProcess();
	std::string CreatorProcname="undefined";
	if(creator) CreatorProcname=creator->GetProcessName();
	
	if (debug) {
		G4cout<<"DEBUG PROVA STACKING creata nuova traccia tipo= "<< track->GetDynamicParticle() ->GetPDGcode()<<", MotherIsotope Val= "<< runStackAction->GetMotherIsotope();
		if (creator) G4cout<<", MerdaProcess= "<<track->GetCreatorProcess()->GetProcessType();
			G4cout<<G4endl;
	}
	//keep primary particle
	//	if (track->GetParentID() == 0) return fUrgent;
	//	if (track->GetParentID() == 1 && track->GetDynamicParticle() ->GetPDGcode()==11)		  G4cout<<"CMOSDEBUG PROVA STACKING elettrone! en= "<< track->GetKineticEnergy()/CLHEP::keV  <<G4endl;
//	runStackAction->SetMotherIsotope(-10); //I have a new particle, so a initialise the flag
    
    fEventAction->ResetPassCounter(); //collamaf: at each new track we reset the pass counter
    
	if (track->GetDynamicParticle() ->GetPDGcode()==22) { //if I generated a gamma
		
		if (CreatorProcname=="protonInelastic" || CreatorProcname=="neutronInelastic") { //if the gamma was due to protonInelastic process dump all the wanted info in a temp set of vectors
			GammaStoreEne.push_back(track->GetKineticEnergy()/CLHEP::MeV);
			GammaStoreX.push_back(track->GetPosition().x()/mm);
			GammaStoreY.push_back(track->GetPosition().y()/mm);
			GammaStoreZ.push_back(track->GetPosition().z()/mm);
			GammaStoreCX.push_back(track->GetMomentumDirection().x());
			GammaStoreCY.push_back(track->GetMomentumDirection().y());
			GammaStoreCZ.push_back(track->GetMomentumDirection().z());
			if (track->GetTouchableHandle()->GetVolume()->GetName()=="Tumor") GammaStoreVolume.push_back(1);
			else GammaStoreVolume.push_back(0);
			
			if (debug)  G4cout<<"DEBUG butto nel vettore!"<<G4endl;
			if (debug) G4cout<<"DEBUG PROVA STACKING nuovo fotone! en= "<< track->GetKineticEnergy()/CLHEP::keV  <<G4endl;
			if (debug) G4cout<<"DEBUG PROVA STACKING Volume creazione fotone per protonInelastic= "<< track->GetTouchableHandle()->GetVolume()->GetName()  <<G4endl<<G4endl;

		}
		
		

	}
	
	if (track->GetDynamicParticle() ->GetPDGcode()>10000) {  //if I generated a Heavy Ion
		
		if (CreatorProcname=="protonInelastic" || CreatorProcname=="neutronInelastic" ) { //If the heavy ion was generated due to protonInelastic process, dump to root file all relevant informations about the gammas that were generated with it
			if (debug) G4cout<<"DEBUG  "<< track->GetDynamicParticle() ->GetPDGcode() <<G4endl;
			for (size_t aa=0; aa<GammaStoreEne.size(); aa++) {
				if (debug) G4cout<<GammaStoreEne.at(aa)<<G4endl;
				runStackAction->AddExitGammaEne(GammaStoreEne.at(aa));
				runStackAction->AddExitGammaMother(track->GetDynamicParticle() ->GetPDGcode() );
				runStackAction->AddRunX(GammaStoreX.at(aa));
				runStackAction->AddRunY(GammaStoreY.at(aa));
				runStackAction->AddRunZ(GammaStoreZ.at(aa));
				runStackAction->AddRunCosX(GammaStoreCX.at(aa));
				runStackAction->AddRunCosY(GammaStoreCY.at(aa));
				runStackAction->AddRunCosZ(GammaStoreCZ.at(aa));
				runStackAction->AddRunVolume(GammaStoreVolume.at(aa));
				runStackAction->AddExitProEne(track->GetKineticEnergy()/CLHEP::MeV);

				
				
			}
			//then free the vectors
			GammaStoreEne.clear();
			GammaStoreX.clear();
			GammaStoreY.clear();
			GammaStoreZ.clear();
			GammaStoreCX.clear();
			GammaStoreCY.clear();
			GammaStoreCZ.clear();
			GammaStoreVolume.clear();
//			GammaStoreEnePro.clear();
		
		}
	}
	
	return fUrgent;
}
//kill secondary neutrino
//  if (track->GetDefinition() == G4NeutrinoE::NeutrinoE()) return fKill;
//  else return fUrgent;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
