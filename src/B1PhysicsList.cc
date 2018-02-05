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
// $Id: B3PhysicsList.cc 66536 2012-12-19 14:32:36Z ihrivnac $
//
/// \file B1PhysicsList.cc
/// \brief Implementation of the B1PhysicsList class

#include "B1PhysicsList.hh"

#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4SystemOfUnits.hh"
#include "G4RegionStore.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1PhysicsList::B1PhysicsList() 
: G4VModularPhysicsList(){
  SetVerboseLevel(1);

  // Default physics
  RegisterPhysics(new G4DecayPhysics());

  // Radioactive decay
  RegisterPhysics(new G4RadioactiveDecayPhysics());

  // EM physics
  RegisterPhysics(new G4EmStandardPhysics());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1PhysicsList::~B1PhysicsList()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1PhysicsList::SetCuts()
{
//  G4VUserPhysicsList::SetCuts();
	
	// default production thresholds for the world volume
	SetCutsWithDefault();
	
	// Production thresholds for detector regions
	/*
	 G4Region* region;
	G4String regName;
	
	
	regName = "CuReg";
	region = G4RegionStore::GetInstance()->GetRegion(regName);
	cuts = new G4ProductionCuts;
	cuts->SetProductionCut(0.01*mm); // same cuts for gamma, e- and e+
	region->SetProductionCuts(cuts);
	*/
	G4ProductionCuts* cuts = new G4ProductionCuts;
	cuts->SetProductionCut(0.01*mm);
	G4ProductionCuts* cutsCMOS = new G4ProductionCuts;
	cutsCMOS->SetProductionCut(0.001*mm);
	
	G4RegionStore::GetInstance()->GetRegion("ABSRegion")->SetProductionCuts(cuts);
	G4RegionStore::GetInstance()->GetRegion("CMOSReg")->SetProductionCuts(cutsCMOS);
	G4RegionStore::GetInstance()->GetRegion("SourceReg")->SetProductionCuts(cuts);
	G4RegionStore::GetInstance()->GetRegion("ResinReg")->SetProductionCuts(cuts);
	G4RegionStore::GetInstance()->GetRegion("CarrierReg")->SetProductionCuts(cuts);
	
/*
	
	regName = "CMOSReg";
	region = G4RegionStore::GetInstance()->GetRegion(regName);
	cuts = new G4ProductionCuts;
	cuts->SetProductionCut(0.01*mm,G4ProductionCuts::GetIndex("gamma"));
	cuts->SetProductionCut(0.1*mm,G4ProductionCuts::GetIndex("e-"));
	cuts->SetProductionCut(0.1*mm,G4ProductionCuts::GetIndex("e+"));
	region->SetProductionCuts(cuts);
*/
}
