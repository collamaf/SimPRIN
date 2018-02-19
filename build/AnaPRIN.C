#define AnaPRIN_cxx
#include "AnaPRIN.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <map>

#define NBINE 1000
#define MINE 0
#define MAXE 20

#define NBINEZOOM 500
#define MINEZOOM 4.8
#define MAXEZOOM 7.8

#define NBINX 60
#define MINX -30
#define MAXX 30

#define NBINY 60
#define MINY -30
#define MAXY 30

#define NBINZ 120
#define MINZ -10
#define MAXZ 110

#define NELEMENTS 14

void AnaPRIN::Loop()
{
	//  Macro to Analyze the root file output of SimPRIN
	//      root> .L AnaPRIN.C
	//      root> t=new AnaPRIN("Nomefile[without.root]");
	//      root> t.Loop();
	//
	
	//    The macro loops an all entries to save contributions to gamma yield for several isotopes
	//    It automatically reads all the registered isotopes, creates one histo for each and saves them on an output root file
	
	// Last Edit on 2018-02-19 by collamaf - now more elastic, manages cases in which some isotopes of the list are not found
	
	if (fChain == 0) return;
	
	Long64_t nentries = fChain->GetEntriesFast();
	double EneThrMin=MINEZOOM;
	double EneThrMax=MAXEZOOM;
	
	TH1F* AllEne=new TH1F("AllEne","Energy spectrum of all photons exiting the absorber", NBINE, MINE, MAXE);
	TH1F* TuttiGammaX=new TH1F("AllX","AllX", NBINX, MINX, MAXX);
	TH1F* TuttiGammaY=new TH1F("AllY","AllY", NBINY, MINY, MAXY);
	TH1F* TuttiGammaZ=new TH1F("AllZ","AllZ", NBINZ, MINZ, MAXZ);
	
	TH1F* AllEneZoom=new TH1F("AllEneZoom",Form("Energy spectrum of all photons exiting the absorber in range %lf-%lf MeV", EneThrMin, EneThrMax), NBINEZOOM, MINEZOOM, MAXEZOOM);

	
	TH1F* EneFromT=new TH1F("EneFromT","Energy of g from T", NBINE, MINE, MAXE);
	TH1F* EneFromNT=new TH1F("EneFromNT","Energy of g NOT from T", NBINE, MINE, MAXE);


	
	TH1F* GammaXECut=new TH1F("GammaXECut",Form("X prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINX, MINX, MAXX);
	TH1F* GammaYECut=new TH1F("GammaYECut",Form("Y prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINY, MINY, MAXY);
	TH1F* GammaZECut=new TH1F("GammaZECut",Form("Z prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINZ, MINZ, MAXZ);

	TH2F* ShapeXY=new TH2F("ShapeXY","X-Y prod point of photons", NBINX, MINX, MAXX, NBINY, MINY, MAXY);
	TH2F* ShapeZY=new TH2F("ShapeZY","Z-Y prod point of photons", NBINZ, MINZ, MAXZ, NBINY, MINY, MAXY);

	TH2F* ShapeXYECut=new TH2F("ShapeXYECut",Form("X-Y prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINX, MINX, MAXX, NBINY, MINY, MAXY);
	TH2F* ShapeZYECut=new TH2F("ShapeZYECut",Form("Z-Y prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINZ, MINZ, MAXZ, NBINY, MINY, MAXY);



	THStack *StackEne = new THStack("StackEne","");
	THStack *StackZ = new THStack("StackZ","");
	
	THStack *StackEneVolume = new THStack("StackEneVolume","Photon energies from/not from Tumor volume");


	int temp=-10, jj, debug=1, tt=0;
	vector<int>     AllMothers;
	std::vector<int>::iterator it;
	
	int AppoggioZ=-1, AppoggioA=-1;
	
	
	Long64_t nbytes = 0, nb = 0;
	map<int,int> mappa;
//	nentries/=100; //for fast tests
	for (Long64_t jentry=0; jentry<nentries;jentry++) { // PRIMO LOOP SU TUTTI GLI EVENTI: salvo tutti gli isotopi madre in un vettore di appoggio e creo la mappa
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		temp=0;
		
		for (jj=0; jj<ExitGammasEne->size();jj++) { //Loop su tutti i fotoni uscenti dal cilindro in quell'evento
			AllEne->Fill(ExitGammasEne->at(jj));
			AllEneZoom->Fill(ExitGammasEne->at(jj));
			TuttiGammaX->Fill(AllX->at(jj));
			TuttiGammaY->Fill(AllY->at(jj));
			TuttiGammaZ->Fill(AllZ->at(jj));
			ShapeXY->Fill(AllX->at(jj),AllY->at(jj));
			ShapeZY->Fill(AllZ->at(jj),AllY->at(jj));

			if (ExitGammasEne->at(jj)>EneThrMin && ExitGammasEne->at(jj)<EneThrMax) {
				GammaXECut->Fill(AllX->at(jj));
				GammaYECut->Fill(AllY->at(jj));
				GammaZECut->Fill(AllZ->at(jj));
				ShapeXYECut->Fill(AllX->at(jj),AllY->at(jj));
				ShapeZYECut->Fill(AllZ->at(jj),AllY->at(jj));
			}
			if (ExitGammasOriginVolume->at(jj)==0) EneFromNT->Fill(ExitGammasEne->at(jj));
			if (ExitGammasOriginVolume->at(jj)==1) EneFromT->Fill(ExitGammasEne->at(jj));

			SpacchettaIsotopo(ExitGammasMother->at(jj), &AppoggioZ, &AppoggioA);
			if (debug>1) cout<<"Ev "<< jentry<<", jj "<< jj<<", temp "<< temp<<", Mother "<<ExitGammasMother->at(jj)<<" nome spacchettato: Z= "<<AppoggioZ<<", A= "<<AppoggioA   <<endl;

			if(ExitGammasMother->at(jj)>100000) {  //se il fotone è figlio di un isotopo (dovrebbe essere sempre vero in questo caso)
				temp=ExitGammasMother->at(jj); // Appoggio l'attuale isotopo madre in un contenitore temporaneo
			if (debug>1)	cout<<"Setto Temp"<<endl;
			} else cout<<"Attento controlla!!"<<endl;  // per essere sicuri..
			
			it = find(AllMothers.begin(), AllMothers.end(),temp); //cerco l'attuale isotopo nella lista di quelli già visti
			if (it != AllMothers.end()) { //se l'isotopo c'era già
				if (debug>1) cout<<"già visto grazie"<<endl;
			} else { //se è la prima volta che lo vedo ...
				if (debug>1) cout<<"Nuovo! aggiungo "<<temp <<endl;
				AllMothers.push_back(temp); // lo aggiungo al vettore di quelli già visti
				mappa[temp]=tt; // e anche alla mappa per ricordarmi la posizione di ciascun isotopo
				if (debug>0)  cout<<"Metto nella mappa! L'isotopo (chiave) "<< temp<<" è la voce della mappa con valore "<<mappa[temp]<<endl;

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
		SpacchettaIsotopo(AllMothers.at(jj), &AppoggioZ, &AppoggioA);
		HistoEne[jj]=new TH1F(Form("Ene_Z%d_A%d",AppoggioZ, AppoggioA),Form("Ene-X_{Z%d}^{A%d}",AppoggioZ, AppoggioA), NBINE, MINE, MAXE);
//		cout<<"Creato istogramma Ene num= "<< jj<<", nome= "<<Form("E_%d",AllMothers.at(jj)%100000)<<endl;
		if (debug>1) cout<<"Creato istogramma Ene num= "<< jj<<", nome= "<<Form("Ene_Z%d_A%d",AppoggioZ, AppoggioA)<<endl;

		HistoZ[jj]=new TH1F(Form("Z_Z%d_A%d",AppoggioZ, AppoggioA),Form("Z-X_{Z%d}^{A%d}",AppoggioZ, AppoggioA), NBINZ, MINZ, MAXZ);
		if (debug>1)  cout<<"## Creato istogramma Z num= "<< jj<<", nome= "<<Form("Z_Z%d_A%d",AppoggioZ, AppoggioA)<<endl;
		
		HistoEne[jj]->SetLineColor(kBlack);
		HistoEne[jj]->SetLineStyle(3);

		HistoEne[jj]->SetFillColor(kYellow-9);
		
		HistoZ[jj]->SetLineColor(kBlack);
		HistoZ[jj]->SetLineStyle(3);
		HistoZ[jj]->SetFillColor(kYellow-9);
		
		
	}
	
	for (Long64_t jentry=0; jentry<nentries;jentry++) { // SECONDO LOOP SU TUTTI GLI EVENTI: Riscorro tutti gli eventi per riempire gli istogrammi dei vari elementi
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		temp=0;
		for (jj=0; jj<ExitGammasEne->size();jj++) {
			if (debug>1) cout<<"Cerco elemento= "<< ExitGammasMother->at(jj)<<endl;
			auto iter = mappa.find(ExitGammasMother->at(jj)); //cerco nella mappa la posizone dell'isotopo che sto considerando
			
			if (iter != mappa.end()) {
			if (debug>1) cout<<"DEBUG Mother= "<<ExitGammasMother->at(jj)<<", sua mappa "<< iter->second <<endl;
			}
			else {
				cout<<"DEBUG ERRORE! Isotopo non trovato"<<endl; //non dovrebbe succedere...
			}
			HistoEne[iter->second]->Fill(ExitGammasEne->at(jj)); //Riempio l'istogramma di E corrispondente
			HistoZ[iter->second]->Fill(AllZ->at(jj)); //Riempio l'istogramma di Z corrispondente
		}
		
	}

	if (debug>1) cout<<"DEBUG Carbonio12 sta alla posizione "<<mappa[1000060120]<<endl;
	

	AllEne->SetLineColor(kBlack);
	AllEne->Draw();

	int ElementPos[NELEMENTS];
	int ElementName[NELEMENTS]= {
		1000060110,
		1000060120,
		1000060130,
		1000080150,
		1000080160,
		1000090170,
		1000020040,
		1000070120,
		1000070140,
		1000070150,
		1000050100,
		1000050110,
		1000090180,
		1000090190
	};
	
	Color_t ColorMap[NELEMENTS]={
		kRed, //C11
		kBlue, //C12
		kCyan+2, //C13
		kMagenta, //O15
		kGreen, //O16
		kOrange, //F17
		kCyan, //He4
		kRed+2, //N12
		kYellow, //N14
		kViolet+6, //N15
		kGray+1, //B10
		kOrange-6,
		kGreen+3,
		kOrange+1
	};

	for (jj=0; jj<NELEMENTS; jj++) {
		
		auto TempFind = mappa.find(ElementName[jj]);
		if (TempFind->second>=0 && TempFind->second<=AllMothers.size()) {
			ElementPos[jj]=TempFind->second;
			cout<<"Trovato elemento: "<< ElementName[jj] <<" in posizone: "<<ElementPos[jj]<<endl;
		} else {
			ElementPos[jj]=-10;
			cout<<"NON Trovato elemento: "<< ElementName[jj]<<endl;

		}
	}

	for (jj=0; jj<NELEMENTS; jj++) {
		if (ElementPos[jj]>=0) {
			HistoEne[ElementPos[jj]]->SetFillColor(ColorMap[jj]);
			//		HistoEne[ElementPos[jj]]->SetLineColor(ColorMap[jj]);
			HistoZ[ElementPos[jj]]->SetFillColor(ColorMap[jj]);
			//		HistoZ[ElementPos[jj]]->SetLineColor(ColorMap[jj]);
		}
	}

	//Output: salvo prima gli HistoEne e poi gli HistoZ
	for (jj=0; jj<AllMothers.size(); jj++) {
		//		cout<<AllMothers.at(jj)<<endl;
		HistoEne[jj]->Write();
		StackEne->Add(HistoEne[jj]);
		StackZ->Add(HistoZ[jj]);
	}
	for (jj=0; jj<AllMothers.size(); jj++) { //doppio loop per scrivere nel file prima tutti gli HistoEne e poi tutti gli HistoZ
		HistoZ[jj]->Write();
	}

	

	AllEne->Write();
	AllEneZoom->Write();
	TuttiGammaX->Write();
	TuttiGammaY->Write();
	TuttiGammaZ->Write();

	
	TCanvas* canvX=new TCanvas("canvX");
	TuttiGammaX->Draw();
	GammaXECut->SetLineColor(kRed);
	GammaXECut->Draw("same");
	
	TCanvas* canvY=new TCanvas("canvY");
	TuttiGammaY->Draw();
	GammaYECut->SetLineColor(kRed);
	GammaYECut->Draw("same");
	
	TCanvas* canvZ=new TCanvas("canvZ");
	TuttiGammaZ->Draw();
	GammaZECut->SetLineColor(kRed);
	GammaZECut->Draw("same");
	
	TCanvas* canvEneVolume=new TCanvas("canvEneVolume");
//	AllEne->Draw();
	EneFromT->SetLineColor(kRed);
	EneFromT->SetFillColor(kRed);
//	EneFromT->Draw("same");
	StackEneVolume->Add(EneFromT);
	EneFromNT->SetLineColor(kBlue);
	EneFromNT->SetFillColor(kBlue);
//	EneFromNT->Draw("same");
	StackEneVolume->Add(EneFromNT);
	StackEneVolume->Draw();
	
	TLegend* legendEneVolume=new TLegend(0.55, 0.55, .9, .9);
	legendEneVolume->SetName("legendEneVolume");
	legendEneVolume->AddEntry(EneFromNT,EneFromNT->GetTitle());
	legendEneVolume->AddEntry(EneFromT,EneFromT->GetTitle());
	legendEneVolume->Draw();
	legendEneVolume->Write();
	canvEneVolume->Write();
	

	TCanvas* canvStackEne=new TCanvas("canvStackEne");
	StackEne->Draw();
	
	TLegend* legendEne=new TLegend(0.55, 0.55, .9, .9);
	legendEne->SetName("legendEne");
	legendEne->SetNColumns(2);
	for (jj=0; jj<NELEMENTS; jj++) {
		if (ElementPos[jj]>=0) legendEne->AddEntry(HistoEne[ElementPos[jj]],HistoEne[ElementPos[jj]]->GetTitle());
		
	}
	legendEne->Draw();
	legendEne->Write();
	


	
	
	TCanvas* canvStackZ=new TCanvas("canvStackZ");
	StackZ->Draw();
	
	TLegend* legendZ=new TLegend(0.65, 0.6, .9, .9);
	legendZ->SetName("legendZ");
	legendZ->SetNColumns(2);
	for (jj=0; jj<NELEMENTS; jj++) {
		if (ElementPos[jj]>=0) legendZ->AddEntry(HistoEne[ElementPos[jj]],HistoEne[ElementPos[jj]]->GetTitle());
		
	}
	
	legendZ->Draw();
	legendZ->Write();


	StackEne->Write();
	StackZ->Write();
	EneFromT->Write();
	EneFromNT->Write();


	TCanvas* canvStackBoth=new TCanvas("canvStackBoth");
	canvStackBoth->Divide(2,1);
	canvStackBoth->cd(1);
	StackEne->Draw();
	legendEne->Draw();
	
	canvStackBoth->cd(2);
	StackZ->Draw();
	legendZ->Draw();

	canvStackBoth->Write();

	GammaXECut->Write();
	GammaYECut->Write();
	GammaZECut->Write();
	StackEneVolume->Write();
	ShapeXY->Write();
	ShapeZY->Write();
	ShapeXYECut->Write();
	ShapeZYECut->Write();
}


