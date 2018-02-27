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
// $Id: B1PrimaryGeneratorAction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file B1PrimaryGeneratorAction.cc
/// \brief Implementation of the B1PrimaryGeneratorAction class

#include "B1PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4PhysicalConstants.hh"

#include "G4IonTable.hh"
#include "G4ChargedGeantino.hh"

#include "B1RunAction.hh"
#include "B1Analysis.hh"

#include "G4Event.hh"

#include <iostream>
#include <fstream>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using std::ofstream;
using std::ios;
using std::endl;


B1PrimaryGeneratorAction::B1PrimaryGeneratorAction(B1EventAction* eventAction, G4bool RealBeamFlag, G4double FluorFraction)
: G4VUserPrimaryGeneratorAction(),
fParticleGun(0) ,
evtPrimAction(eventAction),
fRealBeamFlag(RealBeamFlag),
fFluorFraction(FluorFraction)

{
	G4int n_particle = 1;
	fParticleGun  = new G4ParticleGun(n_particle);
	
	ofstream SourceFile;
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle("proton");
	
	fParticleGun->SetParticleDefinition(particle);
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1PrimaryGeneratorAction::~B1PrimaryGeneratorAction()
{
	delete fParticleGun;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1PrimaryGeneratorAction::GeneratePrimaries (G4Event* anEvent)
{

	G4double BeamEnergy=100*MeV;
	
	
	if (fFluorFraction<=-10000) {
		fRealBeamFlag=false;
		BeamEnergy=(((G4int)fabs(fFluorFraction))%100)/10.*MeV;
		if(anEvent->GetEventID()==0) G4cout<<"DEBUG: Simple simulation requested! Proton pointlike beam with energy= "<<BeamEnergy/MeV<<" MeV"<<G4endl;
	}
//	BeamEnergy=15*MeV;
	fParticleGun->SetParticleEnergy(BeamEnergy); //SetParticleEnergy uses kinetic energy

	G4ThreeVector momentumDirection = G4ThreeVector(0,0,1);
	
	
	
	G4double x0,y0;
	//  G4double cut = 2.*mm; // collimator dimension
	G4double sizeX = 10*mm/2.;
	G4double sizeY = 10*mm/2.;
	if (!fRealBeamFlag) {
		sizeX=0*mm;
		sizeY=0*mm;
	}
	//-- Gaussian shoot
	//   --------------
	//  while (1>0){ //
	x0 = G4RandGauss::shoot(0.,sizeX);
	y0 = G4RandGauss::shoot(0.,sizeY);
	//    if ( abs(x0)<cut && abs(y0)<cut ) break;
	//    }
	G4ThreeVector position = G4ThreeVector(x0,y0,-1e-5*cm);

	

	fParticleGun->SetParticleMomentumDirection(momentumDirection);
	fParticleGun->SetParticlePosition(position);

//	evtPrimAction->SetSourceEne(fParticleGun->GetParticleEnergy());
	evtPrimAction->SetSourceX((position.x())/mm);
	evtPrimAction->SetSourceY((position.y())/mm);
	evtPrimAction->SetSourceZ((position.z())/mm);
	
	
	fParticleGun->GeneratePrimaryVertex(anEvent);
	
}



G4double  B1PrimaryGeneratorAction::BetaDecaySpectrum(G4double Ek, G4double EndPoint)
{
	G4double ElMassMev = 0.510998928*MeV;
	
	G4double res=0.;
	
	G4double omega= Ek /ElMassMev  +1.;
	G4double omegamax= EndPoint /ElMassMev +1.;
	if(omega>1 && omega<omegamax)
	{
		res=(omegamax-omega)*(omegamax-omega) *Ek*sqrt(omega*omega-1.);
	}
	return res;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

