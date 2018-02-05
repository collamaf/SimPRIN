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
// $Id: B1ActionInitialization.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file B1ActionInitialization.cc
/// \brief Implementation of the B1ActionInitialization class

#include "B1ActionInitialization.hh"
#include "B1PrimaryGeneratorAction.hh"
#include "B1RunAction.hh"
#include "B1EventAction.hh"
#include "B1SteppingAction.hh"
#include "B1StackingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1ActionInitialization::B1ActionInitialization(G4double x0, G4double ZValue, G4double CuDiam, G4int FilterFlag, std::ofstream& file, G4double TBR/*, G4bool SrSourceFlag*/, G4int SourceSelect, G4int SensorChoice)
  : G4VUserActionInitialization(), fX0Scan(x0), fZValue(ZValue), fCuDiam(CuDiam), fFilterFlag(FilterFlag), FilePrimaries(file), fTBR(TBR), /*fSrSourceFlag(SrSourceFlag),*/ 	fSourceSelect(SourceSelect), fSensorChoice(SensorChoice)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1ActionInitialization::~B1ActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1ActionInitialization::BuildForMaster() const
{
  B1RunAction* runAction = new B1RunAction(fX0Scan, fZValue, fCuDiam, fFilterFlag, fTBR, fSourceSelect, fSensorChoice);
  SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1ActionInitialization::Build() const
{
//  SetUserAction(new B1PrimaryGeneratorAction(runAction));
	G4cout<<"PROVA Action Init "<<fX0Scan<<G4endl;

  B1RunAction* runAction = new B1RunAction(fX0Scan, fZValue, fCuDiam, fFilterFlag, fTBR, fSourceSelect, fSensorChoice);
  SetUserAction(runAction);
  
  B1EventAction* eventAction = new B1EventAction(runAction, FilePrimaries);
  SetUserAction(eventAction);
	
  SetUserAction(new B1SteppingAction(eventAction, runAction, fCuDiam));
	
//	B1PrimaryGeneratorAction* primAction= new B1PrimaryGeneratorAction(eventAction, TRUE, fSrSourceFlag, TRUE, fTBR, fSrSourceFlag); // Y, Sr, PrintDist, TBR sorge
	B1PrimaryGeneratorAction* primAction= new B1PrimaryGeneratorAction(eventAction,  fTBR, fSourceSelect);
	SetUserAction(primAction);
	SetUserAction(new B1StackingAction(runAction, eventAction));
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
