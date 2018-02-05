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
	G4int debug=0;
	if (debug) G4cout<<"CMOSDEBUG PROVA STACKING creata nuova traccia tipo= "<< track->GetDynamicParticle() ->GetPDGcode()<<", MotherIsotope Val= "<< runStackAction->GetMotherIsotope()
	<<G4endl;
	//keep primary particle
	//	if (track->GetParentID() == 0) return fUrgent;
	//	if (track->GetParentID() == 1 && track->GetDynamicParticle() ->GetPDGcode()==11)		  G4cout<<"CMOSDEBUG PROVA STACKING elettrone! en= "<< track->GetKineticEnergy()/CLHEP::keV  <<G4endl;
//	runStackAction->SetMotherIsotope(-10); //I have a new particle, so a initialise the flag
    
    fEventAction->ResetPassCounter(); //collamaf: at each new track we reset the pass counter
    
	if (track->GetDynamicParticle() ->GetPDGcode()==11) { //if I generated an electron
		if (debug) G4cout<<"CMOSDEBUG PROVA STACKING nuovo elettrone! en= "<< track->GetKineticEnergy()/CLHEP::keV  <<G4endl;
		if (track->GetParentID() == 1) { //figlio di Sr
			if (debug) G4cout<<"CMOSDEBUG Sr Setto il MotherIsotope a 0"<<G4endl;
			runStackAction->SetMotherIsotope(0);
			(runStackAction->GetRunEnGen()).push_back(track->GetKineticEnergy()/CLHEP::keV);
			(runStackAction->GetRunIsotopeGen()).push_back(0);
			(runStackAction->GetRunCosX()).push_back(track->GetMomentumDirection().x());
			(runStackAction->GetRunCosY()).push_back(track->GetMomentumDirection().y());
			(runStackAction->GetRunCosZ()).push_back(track->GetMomentumDirection().z());
		} else if (track->GetParentID() == 2) {  //figlio di Y
			if (debug) G4cout<<"CMOSDEBUG Y Setto il MotherIsotope a 1"<<G4endl;
			runStackAction->SetMotherIsotope(1);
			(runStackAction->GetRunEnGen()).push_back(track->GetKineticEnergy()/CLHEP::keV);
			(runStackAction->GetRunIsotopeGen()).push_back(1);
			(runStackAction->GetRunCosX()).push_back(track->GetMomentumDirection().x());
			(runStackAction->GetRunCosY()).push_back(track->GetMomentumDirection().y());
			(runStackAction->GetRunCosZ()).push_back(track->GetMomentumDirection().z());
		}
	}
	
	
	
	return fUrgent;
}
//kill secondary neutrino
//  if (track->GetDefinition() == G4NeutrinoE::NeutrinoE()) return fKill;
//  else return fUrgent;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
