//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Feb  6 19:10:22 2018 by ROOT version 6.11/02
// from TTree Source/SourceNtuple
// found on file: PRINmc.root
//////////////////////////////////////////////////////////

#ifndef AnaPRIN_h
#define AnaPRIN_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class AnaPRIN {
	public :
	TTree          *fChain;   //!pointer to the analyzed TTree or TChain
	Int_t           fCurrent; //!current Tree number in a TChain
	
	// Fixed size dimensions of array or collections stored in the TTree if any.
	
	// Declaration of leaf types
	vector<double>  *AllX;
	vector<double>  *AllY;
	vector<double>  *AllZ;
	vector<double>  *AllCosX;
	vector<double>  *AllCosY;
	vector<double>  *AllCosZ;
	vector<double>  *AllEne;
	vector<double>  *AllIsotope;
	vector<double>  *ExitX;
	vector<double>  *ExitY;
	vector<double>  *ExitZ;
	vector<double>  *ExitCosX;
	vector<double>  *ExitCosY;
	vector<double>  *ExitCosZ;
	vector<double>  *ExitEne;
	vector<double>  *ExitPart;
	vector<double>  *ExitParentID;
	vector<int>     *ExitProcess;
	Double_t        ExitTrackN;
	vector<double>  *ExitGammasEne;
	vector<int>     *ExitGammasMother;
	vector<int>     *ExitGammasOriginVolume;

	
	
	
	// List of branches
	TBranch        *b_AllX;   //!
	TBranch        *b_AllY;   //!
	TBranch        *b_AllZ;   //!
	TBranch        *b_AllCosX;   //!
	TBranch        *b_AllCosY;   //!
	TBranch        *b_AllCosZ;   //!
	TBranch        *b_AllEne;   //!
	TBranch        *b_AllIsotope;   //!
	TBranch        *b_ExitX;   //!
	TBranch        *b_ExitY;   //!
	TBranch        *b_ExitZ;   //!
	TBranch        *b_ExitCosX;   //!
	TBranch        *b_ExitCosY;   //!
	TBranch        *b_ExitCosZ;   //!
	TBranch        *b_ExitEne;   //!
	TBranch        *b_ExitPart;   //!
	TBranch        *b_ExitParentID;   //!
	TBranch        *b_ExitProcess;   //!
	TBranch        *b_ExitTrackN;   //!
	TBranch        *b_ExitGammasEne;   //!
	TBranch        *b_ExitGammasMother;   //!
	TBranch        *b_ExitGammasOriginVolume;   //!

	//   AnaPRIN(TTree *tree=0);
	AnaPRIN(TString filename);
	TString nomefile;
	
	virtual ~AnaPRIN();
	virtual Int_t    Cut(Long64_t entry);
	virtual Int_t    GetEntry(Long64_t entry);
	virtual Long64_t LoadTree(Long64_t entry);
	virtual void     Init(TTree *tree);
	virtual void     Loop();
	virtual Bool_t   Notify();
	virtual void     Show(Long64_t entry = -1);
	virtual void SpacchettaIsotopo (int isotope, int* Z , int*A);
	TFile* outfile;
};

#endif

#ifdef AnaPRIN_cxx
AnaPRIN::AnaPRIN(TString filename)
{
	nomefile=filename;
	
	TFile *f = new TFile(Form("%s.root",filename.Data()));
	TTree* tree = (TTree*)gDirectory->Get("Source");
	Init(tree);
	
	outfile=new TFile(Form("%s_out.root", filename.Data()), "RECREATE");
	outfile->cd();
	
	
}

AnaPRIN::~AnaPRIN()
{
	if (!fChain) return;
	delete fChain->GetCurrentFile();
}

Int_t AnaPRIN::GetEntry(Long64_t entry)
{
	// Read contents of entry.
	if (!fChain) return 0;
	return fChain->GetEntry(entry);
}
Long64_t AnaPRIN::LoadTree(Long64_t entry)
{
	// Set the environment to read one entry
	if (!fChain) return -5;
	Long64_t centry = fChain->LoadTree(entry);
	if (centry < 0) return centry;
	if (fChain->GetTreeNumber() != fCurrent) {
		fCurrent = fChain->GetTreeNumber();
		Notify();
	}
	return centry;
}

void AnaPRIN::Init(TTree *tree)
{
	// The Init() function is called when the selector needs to initialize
	// a new tree or chain. Typically here the branch addresses and branch
	// pointers of the tree will be set.
	// It is normally not necessary to make changes to the generated
	// code, but the routine can be extended by the user if needed.
	// Init() will be called many times when running on PROOF
	// (once per file to be processed).
	
	// Set object pointer
	AllX = 0;
	AllY = 0;
	AllZ = 0;
	AllCosX = 0;
	AllCosY = 0;
	AllCosZ = 0;
	AllEne = 0;
	AllIsotope = 0;
	ExitX = 0;
	ExitY = 0;
	ExitZ = 0;
	ExitCosX = 0;
	ExitCosY = 0;
	ExitCosZ = 0;
	ExitEne = 0;
	ExitPart = 0;
	ExitParentID = 0;
	ExitProcess = 0;
	ExitGammasEne = 0;
	ExitGammasMother = 0;
	ExitGammasOriginVolume = 0;

	// Set branch addresses and branch pointers
	if (!tree) return;
	fChain = tree;
	fCurrent = -1;
	fChain->SetMakeClass(1);
	
	fChain->SetBranchAddress("AllX", &AllX, &b_AllX);
	fChain->SetBranchAddress("AllY", &AllY, &b_AllY);
	fChain->SetBranchAddress("AllZ", &AllZ, &b_AllZ);
	fChain->SetBranchAddress("AllCosX", &AllCosX, &b_AllCosX);
	fChain->SetBranchAddress("AllCosY", &AllCosY, &b_AllCosY);
	fChain->SetBranchAddress("AllCosZ", &AllCosZ, &b_AllCosZ);
	fChain->SetBranchAddress("AllEne", &AllEne, &b_AllEne);
	fChain->SetBranchAddress("AllIsotope", &AllIsotope, &b_AllIsotope);
	fChain->SetBranchAddress("ExitX", &ExitX, &b_ExitX);
	fChain->SetBranchAddress("ExitY", &ExitY, &b_ExitY);
	fChain->SetBranchAddress("ExitZ", &ExitZ, &b_ExitZ);
	fChain->SetBranchAddress("ExitCosX", &ExitCosX, &b_ExitCosX);
	fChain->SetBranchAddress("ExitCosY", &ExitCosY, &b_ExitCosY);
	fChain->SetBranchAddress("ExitCosZ", &ExitCosZ, &b_ExitCosZ);
	fChain->SetBranchAddress("ExitEne", &ExitEne, &b_ExitEne);
	fChain->SetBranchAddress("ExitPart", &ExitPart, &b_ExitPart);
	fChain->SetBranchAddress("ExitParentID", &ExitParentID, &b_ExitParentID);
	fChain->SetBranchAddress("ExitProcess", &ExitProcess, &b_ExitProcess);
	fChain->SetBranchAddress("ExitTrackN", &ExitTrackN, &b_ExitTrackN);
	fChain->SetBranchAddress("ExitGammasEne", &ExitGammasEne, &b_ExitGammasEne);
	fChain->SetBranchAddress("ExitGammasMother", &ExitGammasMother, &b_ExitGammasMother);
	fChain->SetBranchAddress("ExitGammasOriginVolume", &ExitGammasOriginVolume, &b_ExitGammasOriginVolume);
	Notify();
}

Bool_t AnaPRIN::Notify()
{
	// The Notify() function is called when a new file is opened. This
	// can be either for a new TTree in a TChain or when when a new TTree
	// is started when using PROOF. It is normally not necessary to make changes
	// to the generated code, but the routine can be extended by the
	// user if needed. The return value is currently not used.
	
	return kTRUE;
}

void AnaPRIN::Show(Long64_t entry)
{
	// Print contents of entry.
	// If entry is not specified, print current entry
	if (!fChain) return;
	fChain->Show(entry);
}
Int_t AnaPRIN::Cut(Long64_t entry)
{
	// This function may be called from Loop.
	// returns  1 if entry is accepted.
	// returns -1 otherwise.
	return 1;
}



void AnaPRIN::SpacchettaIsotopo(int isotope, int* Z, int* A) {
	
	*Z=(isotope%10000000)/10000;
	*A=(isotope /10)%100;
	
	
	
	return;
}






#endif // #ifdef AnaPRIN_cxx
