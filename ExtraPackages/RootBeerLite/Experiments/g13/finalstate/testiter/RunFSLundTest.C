//root --hsdata --hsfinal=THSTest RunFSLundTest.C
//You need to replace Test with your final state class name
{
  //Create FinalState
  THSTest* fs=new THSTest();
  //create datamanager
  THSLundReader* dm=new THSLundReader();
  dm->Init("/lund/file/name.lund","HSParticles");
  fs->SetDataManager(dm);
  Int_t counter=0;
  
  //create ouput tree
  TFile* outfile=new TFile("gentest.root","recreate");
  TTree* outtree=new TTree("FinalTree","output tree");
  fs->FinalStateOutTree(outtree); //connect ouput tree to project branches
  
  gBenchmark->Start("timer");
  
  while(dm->ReadEvent()){//loop over events
    fs->ProcessEvent();
  }
  gBenchmark->Stop("timer");
  gBenchmark->Print("timer");
  
  outfile->cd();
  outtree->Write();
  delete outfile;
}
