#define AnaPRIN_cxx
#include "AnaPRIN.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <map>

#define NBINE 1000
#define MINE 0
#define MAXE 20

#define NBINX 60
#define MINX -3
#define MAXX 3

#define NBINY 60
#define MINY -3
#define MAXY 3

#define NBINZ 120
#define MINZ -10
#define MAXZ 110


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
	
	TH1F* TuttiGammaEne=new TH1F("AllEne","AllEne", NBINE, MINE, MAXE);
	TH1F* TuttiGammaX=new TH1F("AllX","AllX", NBINX, MINX, MAXX);
	TH1F* TuttiGammaY=new TH1F("AllY","AllY", NBINY, MINY, MAXY);
	TH1F* TuttiGammaZ=new TH1F("AllZ","AllZ", NBINZ, MINZ, MAXZ);

	THStack *StackEne = new THStack("StackEne","");
	THStack *StackZ = new THStack("StackZ","");

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
			TuttiGammaEne->Fill(ExitGammasEne->at(jj));
			TuttiGammaX->Fill(AllX->at(jj));
			TuttiGammaY->Fill(AllY->at(jj));
			TuttiGammaZ->Fill(AllZ->at(jj));

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
	TH1F* HistoEne[AllMothers.size()];
	TH1F* HistoZ[AllMothers.size()];
	cout<<"Numero elementi "<<AllMothers.size()<<endl;
	for (jj=0; jj<AllMothers.size(); jj++) {
		HistoEne[jj]=new TH1F(Form("HistoEne%d",AllMothers.at(jj)%100000),Form("HistoEne%d",AllMothers.at(jj)%100000), NBINE, MINE, MAXE);
		cout<<"Creato istogramma Ene num= "<< jj<<", nome= "<<Form("HistoEne%d",AllMothers.at(jj)%100000)<<endl;
		HistoEne[jj]->SetLineColor(kBlack);

		HistoZ[jj]=new TH1F(Form("HistoZ%d",AllMothers.at(jj)%100000),Form("HistoZ%d",AllMothers.at(jj)%100000), NBINZ, MINZ, MAXZ);
		cout<<"Creato istogramma Z num= "<< jj<<", nome= "<<Form("HistoEne%d",AllMothers.at(jj)%100000)<<endl;
		
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
			
			HistoEne[iter->second]->Fill(ExitGammasEne->at(jj)); //Riempio l'istogramma di E corrispondente
			HistoZ[iter->second]->Fill(AllZ->at(jj)); //Riempio l'istogramma di Z corrispondente

			
		}
		
	}
	
	if (debug) cout<<"DEBUG Carbonio12 sta alla posizione "<<mappa[1000060120]<<endl;
	

	TuttiGammaEne->SetLineColor(kBlack);
	TuttiGammaEne->Draw();
	
	TuttiGammaEne->Write();
	TuttiGammaX->Write();
	TuttiGammaY->Write();
	TuttiGammaZ->Write();

	auto C11 = mappa.find(1000060110);
	cout<<"Trovato C11 in posizone: "<<C11->second<<endl;
	auto C12 = mappa.find(1000060120);
	cout<<"Trovato C12 in posizone: "<<C12->second<<endl;
	auto C13 = mappa.find(1000060130);
	cout<<"Trovato C13 in posizone: "<<C13->second<<endl;
	auto O15 = mappa.find(1000080150);
	cout<<"Trovato O15 in posizone: "<<O15->second<<endl;
	auto O16 = mappa.find(1000080160);
	cout<<"Trovato O16 in posizone: "<<O16->second<<endl;
	auto F17 = mappa.find(1000090170);
	cout<<"Trovato F17 in posizone: "<<F17->second<<endl;
	auto He4 = mappa.find(1000020040);
	cout<<"Trovato He4 in posizone: "<<He4->second<<endl;
	auto N14 = mappa.find(1000070140);
	cout<<"Trovato N14 in posizone: "<<N14->second<<endl;
	auto N15 = mappa.find(1000070150);
	cout<<"Trovato N15 in posizone: "<<N15->second<<endl;
	auto B10 = mappa.find(1000050100);
	cout<<"Trovato N15 in posizone: "<<B10->second<<endl;
	auto B11 = mappa.find(1000050110);
	cout<<"Trovato B11 in posizone: "<<B11->second<<endl;
	
	
	HistoEne[C11->second]->SetFillColor(kRed); //C11
	HistoEne[C12->second]->SetFillColor(kBlue); //C12
	HistoEne[C13->second]->SetFillColor(kCyan+2); //C13
	HistoEne[O15->second]->SetFillColor(kMagenta); //O15
	HistoEne[O16->second]->SetFillColor(kGreen); //O16
	HistoEne[F17->second]->SetFillColor(kOrange); //F17
	HistoEne[He4->second]->SetFillColor(kCyan); //He4
	HistoEne[N14->second]->SetFillColor(kYellow); //N14
	HistoEne[N15->second]->SetFillColor(kViolet+6); //N15
	HistoEne[B10->second]->SetFillColor(kGray+1); //B10
	HistoEne[B11->second]->SetFillColor(kOrange-6); //B11
	
	for (jj=0; jj<AllMothers.size(); jj++) {
//		cout<<AllMothers.at(jj)<<endl;
		HistoEne[jj]->Write();
		HistoZ[jj]->Write();
		StackEne->Add(HistoEne[jj]);
		StackZ->Add(HistoZ[jj]);

	}
	
//	StackEne[jj].Write();
//	StackZ[jj].Write();
	
	TCanvas* canv1=new TCanvas();
	TuttiGammaX->Draw();
	
	TCanvas* canv2=new TCanvas();
	TuttiGammaY->Draw();
	
	TCanvas* canv3=new TCanvas();
	TuttiGammaZ->Draw();
	
	TCanvas* canvasStackEne=new TCanvas();
	StackEne->Draw();
	
	TLegend* legendEne=new TLegend();
	legendEne->AddEntry(HistoEne[C11->second],HistoEne[C11->second]->GetTitle());
	legendEne->Draw();
	
}

