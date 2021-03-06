/**
        \class THSDataManager
	Interface to different data formats
	Can read ROOT, HIPO, LUND,...files and converts into HSData format
	for writing or immediate event processing
	HSData format :

	vector< THSParticle >  + THSEventInfo + THSRunInfo

	For CLAS12 data see derived classes THSHipoReader and THSHipoTrigger

	For reading ROOT files for example,

	    THSDataManager* dm=new THSDataManager();
	    TChain chain("HSParticles");
	    chain.Add("*.root");
	    dm->InitChain(&chain);
	    vector<THSParticle> *parts=dm->GetParticles();
	    THSEventInfo* event=dm->GetEventInfo();
	    while(dm->ReadEvent())//loop over events
	      parts->at(0).P4().E()  ... event->TrigBit(30) .... 

	For reading CLAS12 HIPO files you need enviroment variables
	RHIPO, CHIPO and LZ4_h set. For more info see
	https://github.com/dglazier/HASPECT6/wiki/hiporoot
	
	    THSHipoTrigger* dm=new THSHipoTrigger();
	    TChain chain("HSParticles");
	    chain.Add("/home/dglazier/clas12data/out_clas_003432.evio.0.hipo");
	    dm->InitChain(&chain);
	    vector<THSParticle> *parts=dm->GetParticles();
	    THSEventInfo* event=dm->GetEventInfo();
	    while(dm->ReadEvent())//loop over events
	      parts->at(0).P4().E() ... event->TrigBit(30) .... 

	For reading LUND files
	
	    THSLundReader* dm=new THSLundReader();
	    TChain chain("HSParticles");
	    chain.Add("*.ld");
	    dm->InitChain(&chain);
	    vector<THSParticle> *parts=dm->GetParticles();
	    THSEventInfo* event=dm->GetEventInfo();
	    while(dm->ReadEvent())//loop over events
	      parts->at(0).P4().E() ... 


	In general it is more likely that this class will be used with 
	THSFinalState:

	    THS2pi* fs=new THS2pi();
	    fs->SetDataManager(dm);
	    TFile* outfile=new TFile("hipofile.root","recreate");
	    TTree* outtree=new TTree("FinalTree","output tree");
	    fs->FinalStateOutTree(outtree); //connect ouput tree to project branches
	    while(dm->ReadEvent()){//loop over events
	    fs->ProcessEvent();
	    }

	In this way the final state analysis can be applied to any input file 
	type that has a THSDataManger interface


*/

#include "THSDataManager.h"
#include "TTreeCache.h"
#include "TSystem.h"
#include "TObjArray.h"


THSDataManager::~THSDataManager(){

  // for(UInt_t i=0;i<fParticles.size();i++){
  //   if(fParticles[i])delete fParticles[i];  
  // }
  // for(UInt_t i=0;i<fGenerated.size();i++){
  //   if(fGenerated[i])delete fGenerated[i];  
  // }
  
  fParticles.clear();
  fGenerated.clear();

  
  if(fReadParticles){
   fReadParticles->clear();
  }
  if(fReadGenerated){
   fReadGenerated->clear();
  }

  CloseReadTree();
  // if(fReadParticles) delete fReadParticles;
  if(fWeights)delete fWeights;
  if(fEntryList)delete fEntryList;

  if(fEventInfo) delete fEventInfo;
  if(fRunInfo) delete fRunInfo;
  
}
void THSDataManager::CloseReadTree(){
  if(fReadFile) {
    delete fReadFile;fReadFile=nullptr;
    fReadTree=nullptr;
    fRunTree=nullptr;
  }
  if(fPerfstats) delete fPerfstats;fPerfstats=nullptr;

}
Bool_t THSDataManager::InitReader(TString filename,TString name){
  //Default read root format
  fReadFile=TFile::Open(filename);
  if(!fReadFile) {Error("THSDataManager::Init","No file found %s",filename.Data());return kFALSE;}
  fReadTree=dynamic_cast<TTree*>(fReadFile->Get(name));
  if(!fReadTree){Error("THSDataManager::Init","No tree found %s",name.Data());return kFALSE;}
  //standard particle vector
  fReadParticles=&fParticles;
  fReadTree->SetBranchAddress(fReadBName.Data(),&fReadParticles);
  fReadPIDs=&fPIDs;
  fReadTree->SetBranchAddress("PIDs",&fReadPIDs);
  fBParticles=fReadTree->GetBranch(fReadBName.Data());
  fBPIDs=fReadTree->GetBranch("PIDs");
  cout<<"THSDataManager::InitReader set branch "<<fReadParticles<<" "<<&fParticles<<endl;
   //generated particles if simualtions
  fReadGenerated=&fGenerated;
  //if(fInGenerated)fReadTree->SetBranchAddress(fReadGName.Data(),&fReadGenerated);
  if(fReadTree->GetBranch(fReadGName.Data())){
    fReadTree->SetBranchAddress(fReadGName.Data(),&fReadGenerated);
    fInGenerated=kTRUE;
    cout<<"THSDataManager::InitReader set branch "<<fReadGName<<" "<<fReadGenerated<<" "<<endl;
  }
  else fReadGenerated=nullptr;
  
  //Get Event and Run info if exists
  if(fReadTree->GetBranch("EventInfo"))fReadTree->SetBranchAddress("EventInfo",&fEventInfo);
  fRunTree=dynamic_cast<TTree*>(fReadFile->Get("HSRunInfo"));
  if(fRunTree){
    fRunInfo->LoadTree(filename);
  }
  //if(fFinalState) fFinalState->FileStart();

  fReadTree->SetCacheSize(50E6);//10MB
  fReadTree->StopCacheLearningPhase();
  fPerfstats = new TTreePerfStats("ioperf", fReadTree);
  
  return kTRUE;
}
Bool_t THSDataManager::Init(TString filename,TString name){
  fTreeName=name;
  fCurFileName=gSystem->BaseName(filename);
  return InitReader(filename,name); //default use tree reader
}
Bool_t THSDataManager::NextChainFile(){
  if(fChainFileN==fChainFiles->GetEntries()){
    cout<<"THSDataManager::NextChainFile() Finished all files "<<endl;
    PrintPerfStats();
    CloseReadTree();
    if(fWriteThis)CloseOutput();
    return kFALSE; //no more files
  }
  CloseReadTree();
  if(fWriteThis)CloseOutput();
  cout<<"THSDataManager::NextChainFile() new file : "<<fChainFiles->At(fChainFileN)->GetTitle()<<" "<<fChainFileN<<" "<<fChainFiles->GetEntries()<<endl;
  //initialise next file
  fEntry=0;
  TString fname=fChainFiles->At(fChainFileN++)->GetTitle();
  Init(fname,fTreeName);
  fCurFileName=gSystem->BaseName(fname);
  if(fOutDir!=TString("")){
    // fCurFileName.Append(fFileAppend);
    InitOutput(fOutDir+"/"+fCurFileName);
  }
  // if(fEntryList) FilterFinalStateEvent();//If filtering via entrlist, redo for this file
  return kTRUE;
}
Bool_t THSDataManager::ReadEvent(Long64_t entry){
  //Default reader for root files with vector<THSParticle*> branches
  // if(fEntry<fReadTree->GetEntries())fBParticles->GetEntry(fEntry++);
  if(fEntry<fReadTree->GetEntries())fReadTree->GetEntry(fEntry++);
  else if(fChainFiles){
    cout<<fEntry<<" "<<fChainFileN<<" "<<fChainFiles->GetEntries()<<endl;
    if(fChainFileN<=fChainFiles->GetEntries()){//next file
      NextChainFile();  
      //first event in new file

      if(!fReadFile) return kFALSE;
      fReadTree->GetEntry(fEntry++);
    }
  }
  else return kFALSE;
  return kTRUE; 
}
// void THSDataManager::FilterFinalStateEvent(Long64_t entry){
//   if(fEntryList) delete fEntryList;fEntryList=nullptr;
//   fEntryList=new TEntryList();
//   //Fill EntryList with events passing finalstate topology
//   fReadTree->DropBranchFromCache(fBParticles);
//   fReadTree->AddBranchToCache(fBPIDs,kTRUE);
//   fReadTree->StopCacheLearningPhase();
//   for(Int_t i=0;i<fReadTree->GetEntries();i++){
//      fBPIDs->GetEntry(i);
//      if(fFinalState->CheckForATopology())
//        fEntryList->Enter(i);
//   }
//   fReadTree->SetEntryList(fEntryList);
//   fReadTree->DropBranchFromCache(fBPIDs);
//   fReadTree->AddBranchToCache(fBParticles,kTRUE);

//   cout<<"THSDataManager::FilterFinalStateEvent found final state entries = "<<fEntryList->GetN()<<endl;
// }
// Bool_t THSDataManager::ReadFinalStateEvent(Long64_t entry){
//   //Default reader for root files with vector<THSParticle*> branches
//   if(fEntry<fReadTree->GetEntries()){

//     fBPIDs->GetEntry(fEntry);
//     while(!fFinalState->CheckForATopology()){
//       fEntry++;
//        fBPIDs->GetEntry(fEntry);
//        if(fEntry==fReadTree->GetEntries()-1) break;//read last entry so can go to next chain     
//     }
//      fReadTree->GetEntry(fEntry);
//     fEntry++;
//   }
//   else if(fChainFiles){
//     if(fChainFileN<=fChainFiles->GetEntries()){//next file
//       NextChainFile();  
//       if(!fReadFile) return kFALSE;
//       fBPIDs->GetEntry(fEntry);
//       while(!fFinalState->CheckForATopology()){
// 	fEntry++;
// 	fBPIDs->GetEntry(fEntry);
//         if(fEntry==fReadTree->GetEntries()-1) break;//read last entry so can go to next chain     
//      }
//       //fBParticles->GetEntry(fEntry);
//       fReadTree->GetEntry(fEntry);
//       fEntry++;
      
//     }
//   }
//   else return kFALSE;
//   return kTRUE; 
// }

Bool_t THSDataManager::InitChain(TChain* chain){
  //loop over all files in chain
  fChainFiles=chain->GetListOfFiles();
  Info("THSDataManager::InitChain"," Will proceess all %d files in chain",fChainFiles->GetEntries());
  //Load first file
  fCurFileName=fChainFiles->At(fChainFileN++)->GetTitle();
  fTreeName=chain->GetName();
  Init(fCurFileName,chain->GetName());
  fCurFileName=gSystem->BaseName(fCurFileName);
  if(fOutDir!=TString("")){
     InitOutput(fOutDir+"/"+fCurFileName);
  }
  return kTRUE;  
}


void THSDataManager::WriteParticles(){
  cout<<":WriteParticles() "<<fChainFiles<<" "<<fWriteTree<<endl;
  //Write all input events on 1 go
  if(!fChainFiles){
    if(fOutDir!=TString("")){
      fCurFileName.Append(fFileAppend);
      InitOutput(fOutDir+"/"+fCurFileName);
    }
  }
  while(ReadEvent()){
    if(fWriteTree->GetEntries()==100)
      fWriteTree->OptimizeBaskets();
    if(fWriteThis&&fWriteTree){fWriteTree->Fill();PostWrite();}
    fWriteThis=kTRUE;//assume will write next event unless told otherwise
  }
  if(fWriteTree)CloseOutput();
}
void THSDataManager::InitOutput(TString filename){
  fWriteThis=kTRUE;
  //Make output directory if not existing
  if(!fWriteFile) gSystem->Exec(Form("mkdir -p %s",fOutDir.Data()));
  if(!filename.Contains(".root")){
      filename.Append(".root");
  }
  fCurFileName=filename;
  fWriteFile=new TFile(filename,"recreate");
  //fWriteFile->SetCompressionSettings(401);//LZ4
  fWriteTree=new TTree("HSParticles","data tree");
  fWriteTree->Branch(fReadBName,&fParticles);
  fWriteTree->Branch("PIDs",&fPIDs);
  //Make an unsplit tree for MakeSelector
  TTree* UnSplitTree=new TTree("HSUnSplit","unsplit tree for MakeSelector");
  UnSplitTree->Branch(fReadBName,&fParticles,256000,0);
  UnSplitTree->Branch("PIDs",&fPIDs,64000,0);

  if(fEventInfo){
    fWriteTree->Branch("EventInfo",&fEventInfo);
    UnSplitTree->Branch("EventInfo",&fEventInfo,64000,0);
  }
  if(fRunInfo){
    fRunTree=new TTree("HSRunInfo","once per run information");
    fRunTree->Branch("Info",&fRunInfo);
  }
  UnSplitTree->Write();
  delete UnSplitTree;

  if(fAddGenerated)fWriteTree->Branch(fWriteGName,&fGenerated);
  Info("THSDataManager::InitOutput","Saving particles to %s",filename.Data());
}
void THSDataManager::CloseOutput(){
  if(!fWriteFile) return;
  fWriteFile->cd();
  if(fWriteTree)fWriteTree->Write();
  if(fRunTree){fRunTree->Fill();fRunTree->Write();}
  fWriteFile->Close();
  delete fWriteFile;
  fWriteFile=nullptr;
  fWriteTree=nullptr;
  fRunTree=nullptr;
}
void THSDataManager::PrintEvent(Long64_t entry){

  Info("PrintLundEvent","Printing First event, this should help configure project class");
  if(!ReadEvent(entry)) {Error("PrintEvent","error in event read");return;}  
  cout<<"Initialised with "<<fNin<<" particles"<<endl;
  for(UInt_t i=0;i<fNin;i++){
    fParticles[i].Print("");  
  }
  
}
void THSDataManager::PrintPerfStats(){
  if(fReadTree)fReadTree->PrintCacheStats();
  if(fPerfstats) fPerfstats->Print();
}
void THSDataManager::AddUID(TString filename,TString treename){
  //Once all files are available you can give every
  //event a unique ID for use in synchronisating weights etc.
  
  TFile* file=TFile::Open(filename,"update");
  if(!file) {Error("THSDataManager::AddUID","No file %s found",filename.Data());return;}
  TTree* tree=(TTree*)file->Get(treename);
  if(!tree) {Error("THSDataManager::AddUID","No tree %s found in %s",treename.Data(),filename.Data());return;}
  if(tree->GetBranch("UID")) return; //aready exists
  TBranch* branch=tree->Branch("UID",&UID,"UID/D");
  if(!branch) return;
  
  Long64_t id=(Long64_t)UID;
  UID=id;
   for(Long64_t i=0;i<tree->GetEntries();i++){
    branch->Fill();
    id++;
    UID=id;   
   }
   Info("THSDataManager::AddUID","Added UID for file %s ",filename.Data());
   tree->Write(tree->GetName(),TObject::kOverwrite);
   file->Close();
   delete file;
   
}
void THSDataManager::LoadWeights(TString fname,TString wname){
    Info("LoadWeights"," Opening weights file %s name %s",fname.Data(),wname.Data());
    fWeights=new THSWeights();
    fWeights->LoadSaved(fname,wname);
    fWeights->PrintWeight();
   
}
void THSDataManager::GetWeightEvent(){
  //if(fWeights)fWeights->GetEntryBinarySearch((Long64_t)fTUID);
}
Double_t THSDataManager::GetWeight(Int_t species){
  if(fWeights) fWeight=fWeights->GetWeight(species);
  return fWeight;
}
