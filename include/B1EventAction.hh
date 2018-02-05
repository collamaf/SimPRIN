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
// $Id: B1EventAction.hh 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file B1EventAction.hh
/// \brief Definition of the B1EventAction class

#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

class B1RunAction;

/// Event action class
///

class B1EventAction : public G4UserEventAction
{
public:
	B1EventAction(B1RunAction* runAction, std::ofstream &);
	virtual ~B1EventAction();
	
	virtual void BeginOfEventAction(const G4Event* event);
	virtual void EndOfEventAction(const G4Event* event);
	
	void AddEdep(G4double edep) { fEdep += edep; }
	void AddEdkin(G4double edkin) { fEdkin += edkin; }
	void AddNo(G4int no) { fno += no; }
	
	void AddNoPre(G4int PreNo) { fPreNo += PreNo; }
	
	void AddEdepSr(G4double edepSr) { fEdepSr += edepSr; }
//	void AddEdepEl(G4double edepEl) { fEdepEl += edepEl; }
	void AddEdepY(G4double edepY) { fEdepY += edepY; }
	G4double GetSourceX() {return fSourceX;}
	void SetSourceX(G4double sx) {fSourceX=sx;}
	void SetSourceY(G4double sy) {fSourceY=sy;}
	void SetSourceZ(G4double sz) {fSourceZ=sz;}

//	void SetExitProcess(G4String proc) {fExitProcess=proc;}
	
	void SetSourceCosX(G4double scx) {fSourceCosX=scx;}
	void SetSourceCosY(G4double scy) {fSourceCosY=scy;}
	void SetSourceCosZ(G4double scz) {fSourceCosZ=scz;}
	void SetSourceEne(G4double sene) {fSourceEne=sene;}
	void SetSourceIsotope(G4double siso) {fSourceIsotope=siso;}
	
	void AddNSourceExit(G4double snsexit) {fNSourceExit+=snsexit;}
	G4double GetNSourceExit(void) {return fNSourceExit;}
	
	void SetStoreTrackID(G4int trid) {fStoreTrackID=trid;}
	G4int GetStoreTrackID(void) {return fStoreTrackID;}
	
	void AddPassCounter(G4int num) {fPassCounter+=num;}
    void ResetPassCounter() {fPassCounter=0;}
    G4int GetPassCounter(void) {return fPassCounter;}

	//  void SetStopPosition (G4double x,G4double y, G4double z){
	//  xstop=x;
	//  ystop=y;
	// zstop=z;
	
	//ma}
	
	
private:
	B1RunAction* fRunAction;
	G4double fEdep;
	G4double fEdkin;
	// G4double xstop;
	// G4double ystop;
	// G4double zstop;
	G4int fno;
	G4int fPreNo;
	
	G4double     fEdepSr;
//	G4double     fEdepEl;
	G4double     fEdepY;

	
	G4double fSourceX;
	G4double fSourceY;
	G4double fSourceZ;
	G4double fSourceEne;
	G4int fSourceIsotope;
	G4double fSourceCosX;
	G4double fSourceCosY;
	G4double fSourceCosZ;
	
	G4int fPassCounter;
	
	G4double fNSourceExit;
	
	/*
	G4double fSourceXExit;
	G4double fSourceYExit;
	G4double fSourceZExit;
	G4double fSourceEneExit;
	G4double fSourceCosXExit;
	G4double fSourceCosYExit;
	G4double fSourceCosZExit;
	*/
	G4String fExitProcess;
	G4int fStoreTrackID;
	std::ofstream &FilePrimaries;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


