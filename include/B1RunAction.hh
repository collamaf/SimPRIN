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
// $Id: B1RunAction.hh 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file B1RunAction.hh
/// \brief Definition of the B1RunAction class

#ifndef B1RunAction_h
#define B1RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4AccumulableManager.hh"
#include "globals.hh"
#include <vector>

#include <iostream>

class G4Run;

/// Run action class
///
/// In EndOfRunAction(), it calculates the dose in the selected volume
/// from the energy deposit accumulated via stepping and event actions.
/// The computed dose is then printed on the screen.

class B1RunAction : public G4UserRunAction
{
public:
	B1RunAction(G4double, G4double, G4double, G4int, G4double, G4int, G4int);
	virtual ~B1RunAction();
	
	// virtual G4Run* GenerateRun();
	virtual void BeginOfRunAction(const G4Run*);
	virtual void   EndOfRunAction(const G4Run*);
	
	void AddEdep (G4double edep);
	void AddEdkin (G4double edkin);
	
	// void AddEdepPhot (G4double edepPhot);
	// void AddEdepEl (G4double edepEl);
	
	std::vector<G4double>& GetRunEnCmos() {return RunVectorEnCmos; }
	std::vector<G4double>& GetRunXCmos() {return RunVectorXCmos; }
	std::vector<G4double>& GetRunYCmos() {return RunVectorYCmos; }
	std::vector<G4double>& GetRunZCmos() {return RunVectorZCmos; }
	std::vector<G4double>& GetRunPartCmos() {return RunVectorPartCmos; }
	
	std::vector<G4double>& GetRunEnExit() {return RunVectorEnExit; }
	std::vector<G4double>& GetRunXExit() {return RunVectorXExit; }
	std::vector<G4double>& GetRunYExit() {return RunVectorYExit; }
	std::vector<G4double>& GetRunZExit() {return RunVectorZExit; }
	std::vector<G4double>& GetRunCosXExit() {return RunVectorCosXExit; }
	std::vector<G4double>& GetRunCosYExit() {return RunVectorCosYExit; }
	std::vector<G4double>& GetRunCosZExit() {return RunVectorCosZExit; }
	std::vector<G4double>& GetRunPartExit() {return RunVectorPartExit; }
	std::vector<G4double>& GetRunParentIDExit() {return RunVectorParentIDExit; }
	
	std::vector<G4int>& GetRunExitProcess() {return RunExitProcess; }
	
	std::vector<G4double>& GetRunCosX() {return RunVectorCosX; }
	std::vector<G4double>& GetRunCosY() {return RunVectorCosY; }
	std::vector<G4double>& GetRunCosZ() {return RunVectorCosZ; }
	std::vector<G4double>& GetRunEnGen() {return RunVectorEnGen; }
	std::vector<G4double>& GetRunIsotopeGen() {return RunVectorIsotopeGen; }
	
	std::vector<G4double>& GetRunEAbsComp() {return RunVectorEAbsComp; }

	std::vector<G4double>& GetRunEnPre() {return RunVectorEnPre; }
	std::vector<G4double>& GetRunPart() {return RunVectorPart; }
	
	std::vector<G4double>& GetRunPixNo() {return RunVectorPixNo; }
//	std::vector<G4double>& GetRunPixEneDep() {return RunVectorPixEneDep; }
	std::vector<G4double>& GetRunPixXpos() {return RunVectorPixXpos; }
	std::vector<G4double>& GetRunPixYpos() {return RunVectorPixYpos; }
	
	G4int GetEventNumber() {return nbEventInRun;}
	
	void SetMotherIsotope(G4double miso) {fMotherIsotope=miso;}
	G4int GetMotherIsotope() {return fMotherIsotope;}

	/*
	 std::vector<G4double>& GetSourceX() {return RunVectorSourceX; }
	 std::vector<G4double>& GetSourceY() {return RunVectorSourceY; }
	 std::vector<G4double>& GetSourceZ() {return RunVectorSourceZ; }
	 std::vector<G4double>& GetSourceEne() {return RunVectorSourceEne; }
	 */
	
	
private:
	G4Accumulable<G4double> fEdep;
	G4Accumulable<G4double> fEdep2;
	G4Accumulable <G4double> fEdkin;
	
	G4double fX0Scan;
	G4double fZValue;
	G4double fCuDiam;
	G4int fFilterFlag;
	G4int nbEventInRun;
	G4double fTBR;
	G4int fSourceSelect;
	
	G4int fMotherIsotope=-10;
	G4int fSensorChoice;

	
	//G4Accumulable <G4double> fEdepPhot;
	//G4Accumulable<G4double> fEdepEl;
	
	/////////////////
	// Histogramming
	//
	void CreateHistogram();
	void WriteHistogram();
	
	std::vector<G4double> RunVectorEnCmos;
	std::vector<G4double> RunVectorXCmos;
	std::vector<G4double> RunVectorYCmos;
	std::vector<G4double> RunVectorZCmos;
	std::vector<G4double> RunVectorPartCmos;
	
	std::vector<G4double> RunVectorEnPre;
	std::vector<G4double> RunVectorPart;
	
	std::vector<G4double> RunVectorPixNo;
//	std::vector<G4double> RunVectorPixEneDep;
	std::vector<G4double> RunVectorPixXpos;
	std::vector<G4double> RunVectorPixYpos;
	
	std::vector<G4double> RunVectorCosX;
	std::vector<G4double> RunVectorCosY;
	std::vector<G4double> RunVectorCosZ;
	
	std::vector<G4double> RunVectorEnGen;
	std::vector<G4double> RunVectorIsotopeGen;
	
	
	std::vector<G4double> RunVectorEnExit;
	std::vector<G4double> RunVectorXExit;
	std::vector<G4double> RunVectorYExit;
	std::vector<G4double> RunVectorZExit;
	std::vector<G4double> RunVectorCosXExit;
	std::vector<G4double> RunVectorCosYExit;
	std::vector<G4double> RunVectorCosZExit;
	
	std::vector<G4double> RunVectorPartExit;
	std::vector<G4double> RunVectorParentIDExit;
	
	std::vector<G4int> RunExitProcess;
	
	std::vector<G4double> RunVectorEAbsComp;
	/*
	 std::vector<G4double> RunVectorSourceX;
	 std::vector<G4double> RunVectorSourceY;
	 std::vector<G4double> RunVectorSourceZ;
	 std::vector<G4double> RunVectorSourceEne;
	 */
	
	
};

#endif

