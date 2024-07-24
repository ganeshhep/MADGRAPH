/*
Simple macro showing how to access branches from the delphes output root file,
loop over events, and plot simple quantities such as the jet pt and the di-electron invariant
mass.

root -l examples/Example1.C'("delphes_output.root")'
*/

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

//------------------------------------------------------------------------------

void Analyzer2(const char *inputFile)
{
  gSystem->Load("libDelphes");

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();

  // Get pointers to branches used in this analysis
  TClonesArray *branchJet = treeReader->UseBranch("Jet");
  TClonesArray *branchEvent = treeReader->UseBranch("Event");

  // Book histograms
  TH1 *histJetN = new TH1F("number of jets", "Jet multiplicity", 7, 1, 8);
  TH1 *histJetInvM = new TH1F("invaiant mass", "M_{inv}(jet_{1}, jet_{2})", 130, 10.0, 200.0);

  // Loop over all events
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {
    double ref_pt_1 = 0, ref_pt_2 = 0, j1_pt, j2_pt, jet_pt_1;
    Int_t k = 0;
    // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);

    Jet *jet_b, *j1, *j2, *B_j1, *B_j2;

    // Number of b-tagged jets
    for(Int_t i = 0; i < branchJet->GetEntries(); i++){
      jet_b = (Jet *) branchJet->At(i);
      if(jet_b->BTag == 1){
        k++;
      }
    }

    if(k > 1) // If event contains at least 2 b-tagged jets
    {
      for(Int_t j = 0; j < branchJet->GetEntries(); j++){
        j1 = (Jet *) branchJet->At(j);
        j1_pt = j1->PT;
	  if(j1->BTag == 1){
            if(j1_pt > ref_pt_1){
              ref_pt_1 = j1_pt;
	      //jet_mass_1 = (jet->P4()).M();
              jet_pt_1 = j1_pt;
              B_j1 = j1;
	    }
          }
      }

       for(Int_t l = 0; l < branchJet->GetEntries(); l++){
        j2 = (Jet *) branchJet->At(l);
        j2_pt = j2->PT;
	  if(j2->BTag == 1){
            if(j2_pt < jet_pt_1 && j2_pt > ref_pt_2){
              ref_pt_2 = j2_pt;
	      //jet_mass_2 = (jet_2->P4()).M();
              B_j2 = j2;
	    }
          }
      }
   histJetInvM->Fill((B_j1->P4() + B_j2->P4()).M());
      

    }
    
    
  } 

  // Show resulting histograms
  TCanvas * canvas1 = new TCanvas("canvas1");

  canvas1->cd();
  histJetInvM->Draw();

}

