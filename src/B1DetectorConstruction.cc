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
#include "G4Orb.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction(G4bool TumorFlag, G4double FluorFraction)
: G4VUserDetectorConstruction(),
fScoringVolume(0),
fTumorFlag(TumorFlag),
fFluorFraction(FluorFraction)

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
	
	G4bool SimpleMC=false;
	
	if (fFluorFraction<=-10000) {
		SimpleMC=true;
		G4cout<<"DEBUG: Simple simulation requested! Proton beam on CaF Cylinder"<<G4endl;
	}

	
	//
	// World
	//
	G4double world_sizeXY = 0.2*m;
	G4double world_sizeZ  = 0.2*m;
	G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
//	G4Material* piombo = nist->FindOrBuildMaterial("G4_Pb");
//	G4Material* vuoto = nist->FindOrBuildMaterial("G4_Galactic");

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
	a = 40.00*g/mole;
	G4Element* elCa = new G4Element (name="Calcium", symbol="Ca", z=20.,a );
	
	
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

	//Define Carbon12 material
	G4Material* C12Mat = new G4Material("C12Mat", 1.19*g/cm3, 1);
	C12Mat->AddElement(elC, 1);
	//Define Oxigen16 material
	G4Material* O16Mat = new G4Material("O16Mat", 1.19*g/cm3, 1);
	O16Mat->AddElement(elO, 1);
	//Define Hidrogen material
	G4Material* H1Mat = new G4Material("H1Mat", 1.19*g/cm3, 1);
	H1Mat->AddElement(elH, 1);
	
	G4double Ffracion=fFluorFraction;
	if (fFluorFraction<0) Ffracion=1;

	G4Material* PMMAF = new G4Material("PMMAF", 1.19*g/cm3, 2);
//	PMMAF -> AddMaterial(PMMA, fractionmass=(1-1/(1+Ffracion)));
//	PMMAF -> AddMaterial(F19Mat, fractionmass=(1/(1+Ffracion)));
	PMMAF -> AddMaterial(PMMA, (100-Ffracion)*perCent);
	PMMAF -> AddMaterial(F19Mat, (Ffracion)*perCent);
	
//	PMMAF -> AddElement(elF19, 1/(1+Ffracion));

	G4double AtomicWC12=5*12.01;
	G4double AtomicWO16=2*16;
	G4double AtomicWH1=8*1.008;
	G4double AtomicWF19=Ffracion*19;
	G4double AtomicWPMMA=AtomicWC12+AtomicWO16+AtomicWH1;
	G4double AtomicWPMMAFluorato=AtomicWC12+AtomicWO16+AtomicWH1+AtomicWF19;

	G4double MassFractionC12=AtomicWC12/AtomicWPMMA;
	G4double MassFractionO16=AtomicWO16/AtomicWPMMA;
	G4double MassFractionH1=AtomicWH1/AtomicWPMMA;
	
	G4double MassFractionC12Fluorato=AtomicWC12/AtomicWPMMAFluorato;
	G4double MassFractionO16Fluorato=AtomicWO16/AtomicWPMMAFluorato;
	G4double MassFractionH1Fluorato=AtomicWH1/AtomicWPMMAFluorato;
	G4double MassFractionF19Fluorato=AtomicWF19/AtomicWPMMAFluorato;
	
	G4double customPMMAFDensity=1.19*g/cm3*AtomicWPMMAFluorato/AtomicWPMMA;

	G4cout<<"STRONZO "<<MassFractionC12<<G4endl;
	G4cout<<"STRONZO "<<MassFractionC12Fluorato<<G4endl;

	//Define PMMERDA (C502H8)
	// NIST reference
	G4Material* customPMMA = new G4Material("customPMMA", 1.19*g/cm3, 3);
	customPMMA->AddMaterial(C12Mat, MassFractionC12);
	customPMMA->AddMaterial(O16Mat, MassFractionO16);
	customPMMA->AddMaterial(H1Mat, MassFractionH1);

	//Define PMMERDAF (C502H8)
	// NIST reference
	G4Material* customPMMAF = new G4Material("customPMMAF", customPMMAFDensity, 4); //fake density to compensate for PMMA atoms loss (with Michela)
	customPMMAF->AddMaterial(C12Mat, MassFractionC12Fluorato);
	customPMMAF->AddMaterial(O16Mat, MassFractionO16Fluorato);
	customPMMAF->AddMaterial(H1Mat, MassFractionH1Fluorato);
	customPMMAF->AddMaterial(F19Mat, MassFractionF19Fluorato);

#if 1
	

	//Define Calcium Fluoride
	// NIST reference: https://physics.nist.gov/cgi-bin/Star/compos.pl?ap130
	G4Material* CaF = new G4Material("CaF", 3.18*g/cm3, ncomponents=2);
//	CaF -> AddElement(elCa, natoms=1);
//	CaF -> AddElement(elF19, natoms=2);
	CaF -> AddElement(elCa, 51.3341*perCent);
	CaF -> AddElement(elF19, 48.6659*perCent);
#endif
	
	//############ MATERIAL ASSIGNMENT
	G4Material* Absorber_mat = customPMMA;
	G4Material* Tumor_mat = customPMMAF;
	if (fFluorFraction==-2) Tumor_mat=F19Mat;
	if (fFluorFraction==-1) Tumor_mat=CaF;
	
	if (!fTumorFlag)Tumor_mat = customPMMA; //se non voglio il tumore, lo faccio dello stesso materiale dell'assorbitore

	G4double AbsorberR=2.5*cm;
	G4double AbsorberH=10*cm;
	
	if (SimpleMC){
		AbsorberR=1.27*cm;
		AbsorberH=0.5*cm;
		Absorber_mat=CaF;
	}
	
	G4ThreeVector posAbsorber = G4ThreeVector(0, 0, AbsorberH*0.5);

	G4double TumorR=1*cm;
	G4double Buccia=2*mm;
//	G4double TumorZ=6.4*cm-2*cm;
	G4double TumorZ=1.5*cm; //was 1.4 - 0.7 in case of Tumor, 
	G4ThreeVector posTumorOut = G4ThreeVector(0, 0, TumorZ);

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
		
		 if (1) new G4PVPlacement(0,                     //no rotation
						  posAbsorber,       //at (0,0,0)
						  logicAbsorber,            //its logical volume
						  "Absorber",               //its name
						  logicWorld,            //its mother  volume
						  false,                 //no boolean operation
						  0,                     //copy number
						  checkOverlaps);        //overlaps checking
	
	
	G4cout<<"STRONZO Materiale assorbitore= "<<logicAbsorber->GetMaterial()<<G4endl;
	//###################################################
	// TUMOR
	//##########################
	
	G4Orb* solidTumorOut =
	new G4Orb("TumorOut",                       //its name
			  TumorR);
	
	G4LogicalVolume* logicTumorOut =
	new G4LogicalVolume(solidTumorOut,          //its solid
						Tumor_mat,           //its material
						"TumorOut");            //its name
	
//	if(1||fTumorFlag) new G4PVPlacement(0,                     //no rotation
	if(!SimpleMC) new G4PVPlacement(0,                     //no rotation
					  posTumorOut,       //at (0,0,0)
					  logicTumorOut,            //its logical volume
					  "TumorOut",               //its name
					  logicAbsorber,            //its mother  volume
					  false,                 //no boolean operation
					  0,                     //copy number
					  checkOverlaps);        //overlaps checking
	
	G4cout<<"STRONZO Materiale tumore OUT = "<<logicTumorOut->GetMaterial()<<G4endl;
	
	
	G4Orb* solidTumor =
	new G4Orb("Tumor",                       //its name
			  TumorR-Buccia);
	
	G4LogicalVolume* logicTumor =
	new G4LogicalVolume(solidTumor,          //its solid
						Tumor_mat,           //its material
						"Tumor");            //its name
	
	//	if(1||fTumorFlag) new G4PVPlacement(0,                     //no rotation
	if(!SimpleMC) new G4PVPlacement(0,                     //no rotation
									G4ThreeVector(0.,0.,0.),       //at (0,0,0)
									logicTumor,            //its logical volume
									"Tumor",               //its name
									logicTumorOut,            //its mother  volume
									false,                 //no boolean operation
									0,                     //copy number
									checkOverlaps);        //overlaps checking
	
	G4cout<<"STRONZO Materiale tumore= "<<logicTumor->GetMaterial()<<G4endl;

	
	// Set scoring volume
	//Pixelated CMOS
	fScoringVolume = logicAbsorber;
	
	//Solid Si CMOS
	//fScoringVolume = logicCmos;

	return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
