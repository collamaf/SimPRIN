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
// $Id: B1DetectorConstruction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class
///
///
///

#include "B1DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SubtractionSolid.hh"
#include "G4StepLimiter.hh"
#include "G4UserLimits.hh"
#include "G4Region.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
: G4VUserDetectorConstruction(),
fScoringVolume(0)

{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{
	// Get nist material manager
	G4NistManager* nist = G4NistManager::Instance();
	
	// Option to switch on/off checking of volumes overlaps
	//
	G4bool checkOverlaps = false;
	
	//
	// World
	//
	G4double world_sizeXY = 0.2*m;
	G4double world_sizeZ  = 0.2*m;
	G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
	//	G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");
	
	G4Box* solidWorld =
	new G4Box("World",                       //its name
			  0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
	
	G4LogicalVolume* logicWorld =
	new G4LogicalVolume(solidWorld,          //its solid
						world_mat,           //its material
						"World");            //its name
	
	G4VPhysicalVolume* physWorld =
	new G4PVPlacement(0,                     //no rotation
					  G4ThreeVector(),       //at (0,0,0)
					  logicWorld,            //its logical volume
					  "World",               //its name
					  0,                     //its mother  volume
					  false,                 //no boolean operation
					  0,                     //copy number
					  checkOverlaps);        //overlaps checking
	
	
	//###################################################################
	//###################################################
	// Definitions of materials
	//##########################
	
	G4double z, a, density;
	G4String name, symbol;
	G4int ncomponents, natoms;
	
	a = 1.01*g/mole;
	G4Element* elH = new G4Element (name="Hydrogen", symbol="H", z=1.,a );
	a = 12.01*g/mole;
	G4Element* elC = new G4Element (name="Carbon", symbol="C", z=6.,a );
	a = 16.00*g/mole;
	G4Element* elO = new G4Element (name="Oxygen", symbol="O", z=8.,a );
	a = 14.00*g/mole;
	G4Element* elN = new G4Element (name="Nitrogen", symbol="N", z=7.,a );
	a = 19.00*g/mole;
	G4Element* elF19 = new G4Element (name="Fluorine19", symbol="F19", z=9.,a );
	
	density = 4.000*g/cm3; //4 for MT9V011, 2.43 for MT9V115
	G4Material* Resin = new G4Material (name="Resin", density, ncomponents=3);
	Resin->AddElement (elH, natoms=30);
	Resin->AddElement (elC, natoms=20);
	Resin->AddElement (elO, natoms=2);
	
	G4double densityAlu = 2.600*g/cm3;
	//	G4NistManager* man = G4NistManager::Instance();
	G4NistManager::Instance()->BuildMaterialWithNewDensity("MyAlu","G4_Al",densityAlu);
	
	/* ref from TestEm7
	 G4NistManager::Instance()->
	 BuildMaterialWithNewDensity("Water_1.05","G4_WATER",1.05*g/cm3);
	 */
	
	//###################################################
	// AGAR AGAR Source - AgarAgar should be C14 H24 O9
	//##########################
	
	G4double Agardensity = 1.030*g/cm3;
	G4Material* AgarAgar = new G4Material (name="AgarAgar", Agardensity, ncomponents=3);
	AgarAgar->AddElement (elH, natoms=24);
	AgarAgar->AddElement (elC, natoms=14);
	AgarAgar->AddElement (elO, natoms=9);
	
	
	//###################################################
	// ABS material - ABS should be C15 H17 N
	//##########################
	G4double ABSdensity = 0.7*g/cm3;
	G4Material* ABS = new G4Material (name="ABS", ABSdensity, ncomponents=3);
	ABS->AddElement (elH, natoms=17);
	ABS->AddElement (elC, natoms=15);
	ABS->AddElement (elN, natoms=1);
	
	//Define PMMA (C502H8)
	// NIST reference
	G4Material* PMMA = new G4Material("PMMA", 1.19*g/cm3, 3);
	PMMA -> AddElement(elC, 5);
	PMMA -> AddElement(elO, 2);
	PMMA -> AddElement(elH, 8);
	
	//Define Fluorine19 material
	G4Material* F19Mat = new G4Material("F19Mat", 1.19*g/cm3, 1);
	F19Mat->AddElement(elF19, 1);
	
	
	//Define PMMA (C502H8) Fluorato
	// NIST reference
//	G4double Ffracion=6.7e13; //to be red: one F atom every XX PMMA molecules
	G4double Ffracion=10; //to be red: one F atom every XX PMMA molecules
	G4Material* PMMAF = new G4Material("PMMAF", 1.19*g/cm3, 2);
	PMMAF -> AddMaterial(PMMA, 1-1/(1+Ffracion));
//	PMMAF -> AddMaterial(F19Mat, 1);
	PMMAF -> AddElement(elF19, 1/(1+Ffracion));
	
	//############ MATERIAL ASSIGNMENT
	G4Material* Absorber_mat = PMMAF;
	
	G4double AbsorberR=2.5*cm;
	G4double AbsorberH=10*cm;
	G4ThreeVector posAbsorber = G4ThreeVector(0, 0, AbsorberH*0.5);

	
	//###################################################
	// ABSORBER
	//##########################
	
	G4Tubs* solidAbsorber =
	new G4Tubs("Absorber",                       //its name
			   0,
			   AbsorberR,
			   0.5*AbsorberH,
			   0.*deg,
			   360.*deg);     //its size
	
	G4LogicalVolume* logicAbsorber =
	new G4LogicalVolume(solidAbsorber,          //its solid
						Absorber_mat,           //its material
						"Absorber");            //its name
		
		new G4PVPlacement(0,                     //no rotation
						  posAbsorber,       //at (0,0,0)
						  logicAbsorber,            //its logical volume
						  "Absorber",               //its name
						  logicWorld,            //its mother  volume
						  false,                 //no boolean operation
						  0,                     //copy number
						  checkOverlaps);        //overlaps checking
		

	//################################################### END DOTA SOURCE
	
	// Set scoring volume
	//Pixelated CMOS
	fScoringVolume = logicAbsorber;
	
	//Solid Si CMOS
	//fScoringVolume = logicCmos;

	return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
