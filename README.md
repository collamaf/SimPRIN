# SimPRIN
For PRIN


Source->Draw("ExitGammasEne","");
Source->SetLineColor(kBlue);
Source->Draw("ExitGammasEne","ExitGammasMother==1000060120","same");
Source->SetLineColor(kRed);
Source->Draw("ExitGammasEne","ExitGammasMother==1000060110","same")
Source->SetLineColor(kGreen);
Source->Draw("ExitGammasEne","ExitGammasMother==1000070120","same")
Source->SetLineColor(kMagenta);
Source->Draw("ExitGammasEne","ExitGammasMother==1000080160","same")




C11: 1000060110
C12: 1000060120
N12: 1000070120
O16: 1000080160



TChain * chain = new TChain("Source")
chain->Add("PRINmcYT_AllF_t*.root")
TFile *file = TFile::Open("Prova5T.root","RECREATE");
chain->CloneTree(-1,"fast");
file->Write();


TChain * chain = new TChain("Source")
chain->Add("PRINmc_t*.root")

Multi Thread non sembra essere molto efficiente
1T-> 1m40s
8T-> 1m2s
con 1000000 primari con tumor di solo fluoro




TFile *f0 = TFile::Open("PRINmcYT_AllF_out.root")
AllEne->SetLineColor(kRed);
AllEne->Draw();
TFile *f1 = TFile::Open("PRINmcYT_AllCaF_out.root")
AllEne->SetLineColor(kRed-7);
AllEne->Draw("same");
TFile *f2 = TFile::Open("PRINmcNT_out.root")
AllEne->SetLineColor(kBlack);
AllEne->Draw("same");
TFile *f3 = TFile::Open("PRINmcYT_Real_1000pF_out.root")
AllEne->SetLineColor(kBlue);
AllEne->Draw("same");
TFile *f4 = TFile::Open("PRINmcYT_Real_100pF_out.root")
AllEne->SetLineColor(kBlue-6);
AllEne->Draw("same");
TFile *f5 = TFile::Open("PRINmcYT_Real_10pF_out.root")
AllEne->SetLineColor(kBlue-10);
AllEne->Draw("same");


.L AnaPRIN.C
 c=new AnaPRIN("RootBuoni/PRINmcYT_AllF")
  c->Loop()
  .q
  .L AnaPRIN.C
  c=new AnaPRIN("RootBuoni/PRINmcNT")
  c->Loop()
  .q
  
  root  -l
  .L AnaPRIN.C
  c=new AnaPRIN("RootBuoni/PRINmcYT_AllCaF")
  c->Loop()
  .q
  
  
  .L AnaPRIN.C
  c=new AnaPRIN("RootBuoni/PRINmcYT_Real_10pF")
  c->Loop()
  .q
  
  
.L AnaPRIN.C
c=new AnaPRIN("RootBuoni/PRINmcYT_Real_100pF")
c->Loop()
.q
  
  
.L AnaPRIN.C
c=new AnaPRIN("RootBuoni/PRINmcYT_Real_1000pF")
c->Loop()
.q

