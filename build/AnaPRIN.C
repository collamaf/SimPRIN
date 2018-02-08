#define AnaPRIN_cxx
#include "AnaPRIN.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <map>

#define NBIN 1000
#define MIN 0
#define MAX 30

void AnaPRIN::Loop()
{
	//  Macro to Analyze the root file output of SimPRIN
	//      root> .L AnaPRIN.C
	//      root> t=new AnaPRIN("Nomefile[without.root]");
	//      root> t.Loop();
	//
	
	//    The macro loops an all entries to save contributions to gamma yield for several isotopes
	//    It automatically reads all the registered isotopes, creates one histo for each and saves them on an output root file
	
	// Last Edit on 2018-02-08 by collamaf
	
	if (fChain == 0) return;
	
	Long64_t nentries = fChain->GetEntriesFast();
	
	TH1F* TuttiGamma=new TH1F("All","All", NBIN, MIN, MAX);

	int temp=-10, jj, debug=0, tt=0;
	vector<int>     AllMothers;
	std::vector<int>::iterator it;
	
	
	Long64_t nbytes = 0, nb = 0;
	map<int,int> mappa;
	
	for (Long64_t jentry=0; jentry<nentries;jentry++) { // PRIMO LOOP SU TUTTI GLI EVENTI: salvo tutti gli isotopi madre in un vettore di appoggio e creo la mappa
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		temp=0;
		
		for (jj=0; jj<ExitGammasEne->size();jj++) { //Loop su tutti i fotoni uscenti dal cilindro in quell'evento
			TuttiGamma->Fill(ExitGammasEne->at(jj));

			if (debug) cout<<"Ev "<< jentry<<", jj "<< jj<<", temp "<< temp<<", Mother "<<ExitGammasMother->at(jj)<<endl;

			if(ExitGammasMother->at(jj)>100000) {  //se il fotone è figlio di un isotopo (dovrebbe essere sempre vero in questo caso)
				temp=ExitGammasMother->at(jj); // Appoggio l'attuale isotopo madre in un contenitore temporaneo
			if (debug)	cout<<"Setto Temp"<<endl;
			} else cout<<"Attento controlla!!"<<endl;  // per essere sicuri..
			
			it = find(AllMothers.begin(), AllMothers.end(),temp); //cerco l'attuale isotopo nella lista di quelli già visti
			if (it != AllMothers.end()) { //se l'isotopo c'era già
				if (debug) cout<<"già visto grazie"<<endl;
			} else { //se è la prima volta che lo vedo ...
				if (debug) cout<<"Nuovo! aggiungo "<<temp <<endl;
				AllMothers.push_back(temp); // lo aggiungo al vettore di quelli già visti
				mappa[temp]=tt; // e anche alla mappa per ricordarmi la posizione di ciascun isotopo
				cout<<"Metto nella mappa! L'isotopo (chiave) "<< temp<<" è la voce della mappa con valore "<<mappa[temp]<<endl;

				tt++; //incremento il contatore degli isotopi trovati
			}
		}
		
	} //FINE PRIMO LOOP SU TUTTI GLI EVENTI
	// Finito il primo loop ho riempito un vettore (AllMothers) con tutti gli isotopi trovati e una mappa (mappa) con la loro posizione
	
	// Creo l'array di istogrammi per i vari isotopi
	TH1F* Histo[AllMothers.size()];
	cout<<"Numero elementi "<<AllMothers.size()<<endl;
	for (jj=0; jj<AllMothers.size(); jj++) {
		Histo[jj]=new TH1F(Form("Histo%d",AllMothers.at(jj)%100000),Form("Histo%d",AllMothers.at(jj)%100000), NBIN, MIN, MAX);
		cout<<"Creato istogramma num= "<< jj<<", nome= "<<Form("Histo%d",AllMothers.at(jj)%100000)<<endl;
	}
	
	for (Long64_t jentry=0; jentry<nentries;jentry++) { // SECONDO LOOP SU TUTTI GLI EVENTI: Riscorro tutti gli eventi per riempire gli istogrammi dei vari elementi
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		temp=0;
		for (jj=0; jj<ExitGammasEne->size();jj++) {
			if (debug) cout<<"Cerco elemento= "<< ExitGammasMother->at(jj)<<endl;
			auto iter = mappa.find(ExitGammasMother->at(jj)); //cerco nella mappa la posizone dell'isotopo che sto considerando
			
			if (iter != mappa.end()) {
			if (debug) cout<<"DEBUG Mother= "<<ExitGammasMother->at(jj)<<", sua mappa "<< iter->second <<endl;
			}
			else {
				cout<<"DEBUG ERRORE! Isotopo non trovato"<<endl; //non dovrebbe succedere...
			}
			
			Histo[iter->second]->Fill(ExitGammasEne->at(jj)); //Riempio l'istogramma corrispondente
			
			
		}
		
	}
	
	if (debug) cout<<"DEBUG Carbonio12 sta alla posizione "<<mappa[1000060120]<<endl;
	

	TuttiGamma->SetLineColor(kBlack);
	TuttiGamma->Draw();

	
	TuttiGamma->Write();
	
	
	for (jj=0; jj<AllMothers.size(); jj++) {
//		cout<<AllMothers.at(jj)<<endl;
		Histo[jj]->Write();
		
	}
	
	
	
}

