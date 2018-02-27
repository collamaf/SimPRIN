#define AnaPRIN_cxx
#include "AnaPRIN.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <map>

#define NBINE 1000
#define MINE 0
#define MAXE 20
#define NBINEPROT 250

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

#define NBINMOTHER 30000
#define MINMOTHER 1000000000
#define MAXMOTHER 1000300000

#define NBINMOTHERold 30000
#define MINMOTHERold 0
#define MAXMOTHERold 30000

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

	TH1F* MotherFromTraw=new TH1F("MotherFromTraw","Mother isotope of g from T RAW", NBINMOTHER, MINMOTHER-5, MAXMOTHER-5);
	TH1F* MotherFromNTraw=new TH1F("MotherFromNTraw","Mother isotope of g NOT from T RAW", NBINMOTHER, MINMOTHER-5, MAXMOTHER-5);

	
	
	TH1F* GammaXECut=new TH1F("GammaXECut",Form("X prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINX, MINX, MAXX);
	TH1F* GammaYECut=new TH1F("GammaYECut",Form("Y prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINY, MINY, MAXY);
	TH1F* GammaZECut=new TH1F("GammaZECut",Form("Z prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINZ, MINZ, MAXZ);

	TH2F* ShapeXY=new TH2F("ShapeXY","X-Y prod point of photons", NBINX, MINX, MAXX, NBINY, MINY, MAXY);
	TH2F* ShapeZY=new TH2F("ShapeZY","Z-Y prod point of photons", NBINZ, MINZ, MAXZ, NBINY, MINY, MAXY);

	TH2F* ShapeXYECut=new TH2F("ShapeXYECut",Form("X-Y prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINX, MINX, MAXX, NBINY, MINY, MAXY);
	TH2F* ShapeZYECut=new TH2F("ShapeZYECut",Form("Z-Y prod point of photons in energy window: %lf-%lf MeV", EneThrMin, EneThrMax), NBINZ, MINZ, MAXZ, NBINY, MINY, MAXY);



	THStack *StackEne = new THStack("StackEne","");
	THStack *StackZ = new THStack("StackZ","");
	THStack *StackEneProt = new THStack("StackEneProt","Energy of the proton that generated each element");
	THStack *StackMotherVolume = new THStack("StackMotherVolume","Mother isotopes from/not from Tumor volume");

	THStack *StackEneVolume = new THStack("StackEneVolume","Photon energies from/not from Tumor volume");


	int temp=-10, jj, debug=1, tt=0;
	vector<int>     AllMothers;
	std::vector<int>::iterator it;
	
	int AppoggioZ=-1, AppoggioA=-1;
	
	
	Long64_t nbytes = 0, nb = 0;
	map<int,int> mappa;
	nentries/=(100/fEventFraction); //reduce number of events to process for faster tests
	for (Long64_t jentry=0; jentry<nentries;jentry++) { // PRIMO LOOP SU TUTTI GLI EVENTI: salvo tutti gli isotopi madre in un vettore di appoggio e creo la mappa
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		temp=0;
		
		for (jj=0; jj<GammaEne->size();jj++) { //Loop su tutti i fotoni uscenti dal cilindro in quell'evento
			AllEne->Fill(GammaEne->at(jj));
			AllEneZoom->Fill(GammaEne->at(jj));
			TuttiGammaX->Fill(AllX->at(jj));
			TuttiGammaY->Fill(AllY->at(jj));
			TuttiGammaZ->Fill(AllZ->at(jj));
			ShapeXY->Fill(AllX->at(jj),AllY->at(jj));
			ShapeZY->Fill(AllZ->at(jj),AllY->at(jj));

			if (GammaEne->at(jj)>EneThrMin && GammaEne->at(jj)<EneThrMax) {
				GammaXECut->Fill(AllX->at(jj));
				GammaYECut->Fill(AllY->at(jj));
				GammaZECut->Fill(AllZ->at(jj));
				ShapeXYECut->Fill(AllX->at(jj),AllY->at(jj));
				ShapeZYECut->Fill(AllZ->at(jj),AllY->at(jj));
			}
			
			SpacchettaIsotopo(GammaMother->at(jj), &AppoggioZ, &AppoggioA);
			if (debug>1) cout<<"Ev "<< jentry<<", jj "<< jj<<", temp "<< temp<<", Mother "<<GammaMother->at(jj)<<" nome spacchettato: Z= "<<AppoggioZ<<", A= "<<AppoggioA   <<endl;
			
			if (GammaVolume->at(jj)==0) {
				EneFromNT->Fill(GammaEne->at(jj));
//				MotherFromNTraw->Fill(GammaMother->at(jj)%10000000/10);
				MotherFromNTraw->Fill(GammaMother->at(jj));
//				MotherFromNT->Fill(Form("Z%dA%d", AppoggioZ, AppoggioA),1);
			}
			if (GammaVolume->at(jj)==1) {
				EneFromT->Fill(GammaEne->at(jj));
//				MotherFromTraw->Fill(GammaMother->at(jj)%10000000/10);
				MotherFromTraw->Fill(GammaMother->at(jj));
//				MotherFromT->Fill(Form("Z%dA%d", AppoggioZ, AppoggioA),1);

			}


			if(GammaMother->at(jj)>100000) {  //se il fotone è figlio di un isotopo (dovrebbe essere sempre vero in questo caso)
				temp=GammaMother->at(jj); // Appoggio l'attuale isotopo madre in un contenitore temporaneo
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
	TH1F* HistoProtEne[AllMothers.size()];
	TH1F* HistoProtEneBis[AllMothers.size()];
	cout<<"Numero elementi "<<AllMothers.size()<<endl;
	for (jj=0; jj<AllMothers.size(); jj++) {
		SpacchettaIsotopo(AllMothers.at(jj), &AppoggioZ, &AppoggioA);
		HistoEne[jj]=new TH1F(Form("Ene%s%d",PeriodicTable[AppoggioZ-1], AppoggioA),Form("Ene%s%d",PeriodicTable[AppoggioZ-1], AppoggioA), NBINE, MINE, MAXE);
//		cout<<"Creato istogramma Ene num= "<< jj<<", nome= "<<Form("E_%d",AllMothers.at(jj)%100000)<<endl;
		if (debug>1) cout<<"Creato istogramma Ene num= "<< jj<<", nome= "<<Form("Ene%s%d",PeriodicTable[AppoggioZ-1], AppoggioA)<<endl;

		HistoZ[jj]=new TH1F(Form("Z%s%d",PeriodicTable[AppoggioZ-1], AppoggioA),Form("Z%s%d",PeriodicTable[AppoggioZ-1], AppoggioA), NBINZ, MINZ, MAXZ);
		if (debug>1)  cout<<"## Creato istogramma Z num= "<< jj<<", nome= "<<Form("Z%s%d",PeriodicTable[AppoggioZ-1], AppoggioA)<<endl;
		
		HistoProtEne[jj]=new TH1F(Form("ProtEne%s%d",PeriodicTable[AppoggioZ-1], AppoggioA),Form("ProtEne%s%d",PeriodicTable[AppoggioZ-1], AppoggioA), NBINEPROT, MINE, MAXE);
		HistoProtEneBis[jj]=new TH1F(Form("ProtEneBis%s%d",PeriodicTable[AppoggioZ-1], AppoggioA),Form("ProtEneBis%s%d",PeriodicTable[AppoggioZ-1], AppoggioA), NBINEPROT, MINE, MAXE);
//		cout<<"MERDA "<<Form("ProtEneBis%s%d",PeriodicTable[AppoggioZ-1], AppoggioA)<<endl;

		
		
		HistoEne[jj]->SetLineColor(kBlack);
		HistoEne[jj]->SetLineStyle(3);
		HistoEne[jj]->SetFillColor(kYellow-9);
		
		HistoZ[jj]->SetLineColor(kBlack);
		HistoZ[jj]->SetLineStyle(3);
		HistoZ[jj]->SetFillColor(kYellow-9);
		
		HistoProtEne[jj]->SetLineColor(0);
		HistoProtEne[jj]->SetLineStyle(3);
		HistoProtEne[jj]->SetFillColor(0);

		HistoProtEneBis[jj]->SetLineColor(0);
		HistoProtEneBis[jj]->SetLineStyle(0);
		HistoProtEneBis[jj]->SetFillColor(0);
	}
	
	for (Long64_t jentry=0; jentry<nentries;jentry++) { // SECONDO LOOP SU TUTTI GLI EVENTI: Riscorro tutti gli eventi per riempire gli istogrammi dei vari elementi
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		temp=0;
		for (jj=0; jj<GammaEne->size();jj++) {
			if (debug>1) cout<<"Cerco elemento= "<< GammaMother->at(jj)<<endl;
			auto iter = mappa.find(GammaMother->at(jj)); //cerco nella mappa la posizone dell'isotopo che sto considerando
			
			if (iter != mappa.end()) {
			if (debug>1) cout<<"DEBUG Mother= "<<GammaMother->at(jj)<<", sua mappa "<< iter->second <<endl;
			}
			else {
				cout<<"DEBUG ERRORE! Isotopo non trovato"<<endl; //non dovrebbe succedere...
			}
			HistoEne[iter->second]->Fill(GammaEne->at(jj)); //Riempio l'istogramma di E corrispondente
			HistoZ[iter->second]->Fill(AllZ->at(jj)); //Riempio l'istogramma di Z corrispondente
			HistoProtEne[iter->second]->Fill(ProtEne->at(jj)); //Riempio l'istogramma di Z corrispondente
			
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
			HistoProtEne[ElementPos[jj]]->SetFillColor(ColorMap[jj]);
			//		HistoZ[ElementPos[jj]]->SetLineColor(ColorMap[jj]);
		}
	}

	//Output: salvo prima gli HistoEne e poi gli HistoZ
	for (jj=0; jj<AllMothers.size(); jj++) {
		//		cout<<AllMothers.at(jj)<<endl;
		DirHistoEne->cd();
		HistoEne[jj]->Write();
		StackEne->Add(HistoEne[jj]);
		StackZ->Add(HistoZ[jj]);
//	}
//	for (jj=0; jj<AllMothers.size(); jj++) { //doppio loop per scrivere nel file prima tutti gli HistoEne e poi tutti gli HistoZ
		DirHistoZ->cd();
		HistoZ[jj]->Write();
//	}
//	for (jj=0; jj<AllMothers.size(); jj++) { //triplo loop per scrivere nel file prima tutti gli HistoEne e poi tutti gli HistoZ e gli HistoProtEne
//		HistoProtEne[jj]->Scale(1/HistoProtEne[jj]->Integral());
		DirHistoProtEne->cd();
		HistoProtEne[jj]->Scale(1/HistoProtEne[jj]->GetMaximum());
		HistoProtEne[jj]->Write();
		StackEneProt->Add(HistoProtEne[jj],"HISTO");
	}
	outfile->cd();
	

	AllEne->Write();
	AllEneZoom->Write();
	TuttiGammaX->Write();
	TuttiGammaY->Write();
	TuttiGammaZ->Write();

	
	TCanvas* canvX=new TCanvas("canvX","canvX");
	TuttiGammaX->Draw();
	GammaXECut->SetLineColor(kRed);
	GammaXECut->Draw("same");
	
	TCanvas* canvY=new TCanvas("canvY","canvY");
	TuttiGammaY->Draw();
	GammaYECut->SetLineColor(kRed);
	GammaYECut->Draw("same");
	
	TCanvas* canvZ=new TCanvas("canvZ","canvZ");
	TuttiGammaZ->Draw();
	GammaZECut->SetLineColor(kRed);
	GammaZECut->Draw("same");
	
	TCanvas* canvEneVolume=new TCanvas("canvEneVolume","canvEneVolume");
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
	
	
	TH1F* MotherFromT=new TH1F("MotherFromT","Mother isotope of g from T", AllMothers.size(), -0.5, AllMothers.size()-0.5);
	TH1F* MotherFromNT=new TH1F("MotherFromNT","Mother isotope of g NOT from T", AllMothers.size(), -0.5, AllMothers.size()-0.5);

	
	
	for (jj=1; jj<=MotherFromNTraw->GetNbinsX(); jj++) {
		if (MotherFromNTraw->GetBinContent(jj)>0 || MotherFromTraw->GetBinContent(jj)>0 ) {
			SpacchettaIsotopo(MotherFromNTraw->GetBinCenter(jj), &AppoggioZ, &AppoggioA);
//			MotherFromNT->Fill(Form("X_{%d}^{%d}", AppoggioZ, AppoggioA), MotherFromNTraw->GetBinContent(jj));
			MotherFromNT->Fill(Form("%s^{%d}", PeriodicTable[AppoggioZ-1], AppoggioA), MotherFromNTraw->GetBinContent(jj));
			if (debug>1)  cout<<"NT BinCenter= "<<(int)MotherFromNTraw->GetBinCenter(jj)<<", Metto in posizione "<< Form("%s%d", PeriodicTable[AppoggioZ-1], AppoggioA) << " il valore "<<MotherFromNTraw->GetBinContent(jj)<<" jj= "<<jj<< endl;
			if (debug>1)  cout<<"T BinCenter= "<<(int)MotherFromTraw->GetBinCenter(jj)<<", Metto in posizione "<< Form("%s%d", PeriodicTable[AppoggioZ-1], AppoggioA) << " il valore "<<MotherFromTraw->GetBinContent(jj)<<" jj= "<<jj<< endl<<endl;
			MotherFromT->Fill(Form("%s^{%d}", PeriodicTable[AppoggioZ-1], AppoggioA), MotherFromTraw->GetBinContent(jj));

		}
	}


	TCanvas* canvMotherVolume=new TCanvas("canvMotherVolume","canvMotherVolume");
	MotherFromT->SetLineColor(kRed);
	MotherFromT->SetFillColor(kRed);
	StackMotherVolume->Add(MotherFromT);
	MotherFromNT->SetLineColor(kBlue);
	MotherFromNT->SetFillColor(kBlue);
	StackMotherVolume->Add(MotherFromNT);
	StackMotherVolume->Draw("HIST");
	
	TLegend* legendMotherVolume=new TLegend(0.55, 0.65, .9, .9);
	legendMotherVolume->SetName("legendMotherVolume");
	legendMotherVolume->AddEntry(MotherFromNT,MotherFromNT->GetTitle());
	legendMotherVolume->AddEntry(MotherFromT,MotherFromT->GetTitle());
	legendMotherVolume->Draw();
	legendMotherVolume->Write();
	canvMotherVolume->Write();
	
	
	TCanvas* canvStackEne=new TCanvas("canvStackEne","canvStackEne");
	StackEne->Draw();
	TLegend* legendEne=new TLegend(0.55, 0.55, .9, .9);
	legendEne->SetName("legendEne");
	legendEne->SetNColumns(2);
	for (jj=0; jj<NELEMENTS; jj++) {
		if (ElementPos[jj]>=0) legendEne->AddEntry(HistoEne[ElementPos[jj]],HistoEne[ElementPos[jj]]->GetTitle());
		
	}
	legendEne->Draw();
	legendEne->Write();
	

	TCanvas* canvStackEneProt=new TCanvas("canvStackEneProt","canvStackEneProt");
	StackEneProt->Draw("HISTO");
	StackEneProt->Write();

	TLegend* legendEneProt=new TLegend(0.65, 0.6, .9, .9);
	legendEneProt->SetName("legendZ");
	legendEneProt->SetNColumns(2);
	for (jj=0; jj<NELEMENTS; jj++) {
		if (ElementPos[jj]>=0) legendEneProt->AddEntry(HistoProtEne[ElementPos[jj]],HistoProtEne[ElementPos[jj]]->GetTitle());
	}
	
	legendEneProt->Draw();
	legendEneProt->Write();
	canvStackEneProt->Write();

	TCanvas* canvEneProt=new TCanvas("canvEneProt","canvEneProt");
		for (jj=0; 1&&jj<NELEMENTS; jj++) {
			if (ElementPos[jj]>=0) {
				SpacchettaIsotopo(ElementName[jj], &AppoggioZ, &AppoggioA);
//				HistoProtEne[ElementPos[jj]]->Clone(Form("ProtEneBis%s%d",PeriodicTable[AppoggioZ-1], AppoggioA));
//				cout<<"MERDA "<<Form("ProtEneBis%s%d",PeriodicTable[AppoggioZ-1], AppoggioA)<<endl;
//				cout<<"FANCULO jj= "<<jj<<", Elemento "<<ElementName[jj]<<", Posizione "<<ElementPos[jj]<<endl;
				
				HistoProtEneBis[ElementPos[jj]]=HistoProtEne[ElementPos[jj]];
				HistoProtEneBis[ElementPos[jj]]->SetName(Form("ProtEneBis%s%d",PeriodicTable[AppoggioZ-1], AppoggioA));
				HistoProtEneBis[ElementPos[jj]]->SetTitle(Form("ProtEneBis%s%d",PeriodicTable[AppoggioZ-1], AppoggioA));
				HistoProtEneBis[ElementPos[jj]]->SetLineColor(HistoProtEneBis[ElementPos[jj]]->GetFillColor());
				HistoProtEneBis[ElementPos[jj]]->SetFillColor(0);
				HistoProtEneBis[ElementPos[jj]]->SetLineStyle(0);
				if (jj==0) HistoProtEneBis[ElementPos[jj]]->Draw("HISTO");
				else HistoProtEneBis[ElementPos[jj]]->Draw("HISTOsame");
				DirHistoProtEne->cd();
				HistoProtEneBis[ElementPos[jj]]->Write();
			}
			legendEneProt->Draw();
		}
	outfile->cd();

	TCanvas* canvStackZ=new TCanvas("canvStackZ","canvStackZ");
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
	MotherFromTraw->Write();
	MotherFromNTraw->Write();
	EneFromT->Write();
	EneFromNT->Write();
	MotherFromT->Write();
	MotherFromNT->Write();
	StackMotherVolume->Write();

	TCanvas* canvStackBoth=new TCanvas("canvStackBoth");
	canvStackBoth->Divide(2,1);
	canvStackBoth->cd(1);
	StackEne->Draw();
	legendEne->Draw();
	
	canvStackBoth->cd(2);
	StackZ->Draw();
	legendZ->Draw();

	canvStackBoth->Write();
	canvEneProt->Write();

	GammaXECut->Write();
	GammaYECut->Write();
	GammaZECut->Write();
	StackEneVolume->Write();
	ShapeXY->Write();
	ShapeZY->Write();
	ShapeXYECut->Write();
	ShapeZYECut->Write();

}


