#include "THSHipoFTMon.h"
#include <TCanvas.h>
#include <TSystem.h>

/*
 	\class THSFTMon
	
	Class to monitor the Forward Tagger in cooked hipo data
	This uses the THipo class to allow access to clas12 banks
	For Example intitialise banks and items like :

	//Get the necessary items from REC::FT Bank
	fFTBank=fHipo->GetBank("REC::ForwardTagger");
	fFTPindex=fFTBank->GetItem("pindex");
	fFTTime=fFTBank->GetItem("time");
	fFTEnergy=fFTBank->GetItem("energy");
 
	Use them in your code like:
	 //loop over entries in bank
	while(FTBank->NextEntry()){
	//Use THipoItem::Val() function to get the value of that item for this entry
	  Histogram1D("FT_Energy")->Fill(fFTEnergy->Val()); 
	  Histogram1D("FT_Time")->Fill(fFTTime->Val());
	}
       

	Note the following banks and are predefined in 
	THSHipoReader and Trigger and can therefor be used 
	directly in this inherited class :

	//Get the necessary items from Particle Bank
	fPBank=fHipo->GetBank("REC::Particle");
	//Get the necessary items from REC::Scintillator Bank
	fSBank=fHipo->GetBank("REC::Scintillator");
	//Get the necessary items from REC::Calorimeter Bank
	fCalBank=fHipo->GetBank("REC::Calorimeter");
	//Get the necessary items from REC::Cherenkov Bank
	fChBank=fHipo->GetBank("REC::Cherenkov");
	//Get the necessary items from REC::FT Bank
	fFTBank=fHipo->GetBank("REC::ForwardTagger");
	
	fEvBank=fHipo->GetBank("REC::Event");
	
  
	If you require any other bank you can add them yourself.
	See for example FT::particles in THSHipoFTMon::Init
	You also need to add to the THSHipoFTMon.h file class declaration


	Histogramming
	Histograms are added to a TList fHistograms and can be filled by
	Histogram1D("name")->Fill(value);

 */
////////////////////////////////////////////////////////////////////////
///Declare histograms for REC bank : REC::Particle;REC::ForwardTagger
void THSHipoFTMon::DeclareHistograms(){


  fHistograms->Add(new TH1F("NParticles","N entries REC::Particle",100,0,100));
  fHistograms->Add(new TH1F("NFT","N entries in REC::ForwardTagger",10,0,10));
  fHistograms->Add(new TH1F("NFT_trig","N entries in FT when FT triggers",10,0,10));
  fHistograms->Add(new TH1F("Trig","FT trigger",20,0,20));
}
/////////////////////////////////////////////////////////////////////////
///Fill histograms for REC banks
void THSHipoFTMon::FillHistograms(){
  //Number of particles in REC::Particle bank (decared in THSHipoReader)
  Histogram1D("NParticles")->Fill(fPBank->GetEntries());
  
  //Number of particles in REC::ForwardTagger bank (decared in THSHipoReader)
  Histogram1D("NFT")->Fill(fFTBank->GetEntries());

  //Number of particles in REC::ForwardTagger bank (decared in THSHipoReader)
  //When FT makes the trigger (declared in THSHipoTrigger)
  if(fTrigFT) Histogram1D("NFT_trig")->Fill(fFTBank->GetEntries());
  //Get the trigger bit from THSHipoTrigger
  Histogram1D("Trig")->Fill(fTrigFT);
  

}
/////////////////////////////////////////////////////////////////////////
///Declare histograms for FT banks i.e. HODO-Calo coincidences
void THSHipoFTMon::Declare_FT_Histograms(){
  fHistograms->Add(new TH1F("FT_Ho-Ca_X","Cluster Hodo-Cal X difference ",100,-150,150));
  fHistograms->Add(new TH1F("FT_Ho-Ca_Y","Cluster Hodo-Cal Y difference ",100,-150,150));
  fHistograms->Add(new TH1F("FT_Ho-Ca_Time","Cluster Hodo-Cal Time difference ",100,-15,15));
  fHistograms->Add(new TH1F("FT_Ho-Ca_CalE0","Cluster Cal Energy neutral",100,0,15));
  fHistograms->Add(new TH1F("FT_Ho-Ca_CalE1","Cluster Cal Energy Single Hodo",100,0,15));
  fHistograms->Add(new TH1F("FT_Ho-Ca_CalE1a","Cluster Cal Energy Front Hodo",100,0,15));
  fHistograms->Add(new TH1F("FT_Ho-Ca_CalE1b","Cluster Cal Energy Rear Hodo",100,0,15));
  fHistograms->Add(new TH1F("FT_Ho-Ca_CalE","Cluster Cal Energy charged",100,0,15));
  fHistograms->Add(new TH1F("FT_Ho-Ca_HodE","Cluster Hodo Energy",100,0,15));
  fHistograms->Add(new TH1F("FT_Ho-Ca_M2Gamma","FT 2 gamma invariant mass",200,0,1));
  fHistograms->Add(new TH1F("FT_Ho-Ca_M2GammaB","FT 2 gamma invariant mass (single hodo)",200,0,1));

  fHistograms->Add(new TH1F("FT_Ca_Charge","FT charge",3,-1,1));
  fHistograms->Add(new TH2F("FT_Ca_YVX-","FT YVX charged",45,-170,170,45,-170,170));
  fHistograms->Add(new TH2F("FT_Ca_YVX0","FT YVX neutral",45,-170,170,45,-170,170));
  fHistograms->Add(new TH2F("FT_Ca_YVX1a","FT YVX front HODO",45,-170,170,45,-170,170));
  fHistograms->Add(new TH2F("FT_Ca_YVX1b","FT YVX rear HODO",45,-170,170,45,-170,170));
  
   fHistograms->Add(new TH1F("FT_Ca_Size-","FT Cal Cluster Size charged",20,0,20));
   fHistograms->Add(new TH1F("FT_Ho_Size-","FT Hodo Cluster Size charged",20,0,20));
   fHistograms->Add(new TH1F("FT_Ca_Size0","FT Cal Cluster Size neutral",20,0,20));
   fHistograms->Add(new TH1F("FT_Ho_Size0","FT Hodo Cluster Size neutral",20,0,20));
 

}
///////////////////////////////////////////////////////////////////////////
///Fill histograms for FT banks
void THSHipoFTMon::Fill_FT_Histograms(){
  //HISTOGRAMS BASED ON HODO CAL coincidence
  if(fFTParticlesBank->GetEntries()==1){
    fFTParticlesBank->NextEntry();
    Int_t cal_id=fFTPB_calID->Val()+1;//numbering starts at 1 not 0
    fFTCALClust_id->FindEntry(cal_id);
    Histogram1D("FT_Ca_Charge")->Fill(fFTPB_ch->Val());

    if(fFTPB_ch->Val()!=0&&fFTCALClustBank->GetEntry()>-1){
      Int_t hodo_id=fFTPB_hodoID->Val();
      fFTHODOClust_id->FindEntry(hodo_id);
  
      if(fFTCALClustBank->GetEntry()<0||fFTHODOClustBank->GetEntry()<0) return;
      //Histogram1D("FT_Ho_Size0")->Fill(fFTHODOClust_size->Val());
    
      Histogram1D("FT_Ho-Ca_X")->Fill(fFTHODOClust_x->Val()-fFTCALClust_x->Val());
      Histogram1D("FT_Ho-Ca_Y")->Fill(fFTHODOClust_y->Val()-fFTCALClust_y->Val());
      Histogram1D("FT_Ho-Ca_HodE")->Fill(fFTHODOClust_e->Val());
      Histogram1D("FT_Ho-Ca_CalE")->Fill(fFTCALClust_e->Val());
      if(fFTHODOClust_size->Val()==1)Histogram1D("FT_Ho-Ca_CalE1")->Fill(fFTCALClust_e->Val());
      if(fFTHODOClust_z->Val()<1814){
	Histogram1D("FT_Ho-Ca_CalE1a")->Fill(fFTCALClust_e->Val());
	Histogram2D("FT_Ca_YVX1a")->Fill(fFTCALClust_x->Val(),fFTCALClust_y->Val());
      }
      if(fFTHODOClust_z->Val()>1825){
	Histogram1D("FT_Ho-Ca_CalE1b")->Fill(fFTCALClust_e->Val());
 	Histogram2D("FT_Ca_YVX1b")->Fill(fFTCALClust_x->Val(),fFTCALClust_y->Val());
     }
      Histogram1D("FT_Ho-Ca_Time")->Fill(fFTHODOClust_t->Val()-fFTCALClust_t->Val());

      Histogram2D("FT_Ca_YVX-")->Fill(fFTCALClust_x->Val(),fFTCALClust_y->Val());
      Histogram1D("FT_Ho_Size-")->Fill(fFTHODOClust_size->Val());
      Histogram1D("FT_Ca_Size-")->Fill(fFTCALClust_size->Val());

    }
    else if(fFTCALClustBank->GetEntry()>-1){
      Histogram2D("FT_Ca_YVX0")->Fill(fFTCALClust_x->Val(),fFTCALClust_y->Val());
      Histogram1D("FT_Ca_Size0")->Fill(fFTCALClust_size->Val());
      Histogram1D("FT_Ho-Ca_CalE0")->Fill(fFTCALClust_e->Val());
      
    }
    // else cout<<"FT hit but no associated FTCalCluster???"<<endl; 
  }
  //now reset
  fFTParticlesBank->ResetEntry();

  //Look for possible FT pi0
  Int_t GotOne=0;
  if(fFTParticlesBank->GetEntries()>1){
    TLorentzVector gamma1;
    TLorentzVector gamma2;
    while(fFTParticlesBank->NextEntry()){
      if(fFTPB_ch->Val()==0&&GotOne==1){
	gamma2.SetXYZM(fFTPB_cx->Val(),
		       fFTPB_cy->Val(),
		       fFTPB_cz->Val(),0);
	gamma2.SetE(fFTPB_e->Val());
 	gamma2.SetRho(fFTPB_e->Val());
	GotOne=2;
	break;
     }
      if(fFTPB_ch->Val()==0&&!GotOne){
	gamma1.SetXYZM(fFTPB_cx->Val(),
		       fFTPB_cy->Val(),
		       fFTPB_cz->Val(),0);
	gamma1.SetE(fFTPB_e->Val());
 	gamma1.SetRho(fFTPB_e->Val());
	GotOne++;
      }
      
    }
    if(GotOne==2) Histogram1D("FT_Ho-Ca_M2Gamma")->Fill((gamma1+gamma2).M());
  }
  fFTParticlesBank->ResetEntry();

  //Look for possible FT pi0 including single layer hodo hits
  GotOne=0;
  if(fFTParticlesBank->GetEntries()>1){
    TLorentzVector gamma1;
    TLorentzVector gamma2;
    while(fFTParticlesBank->NextEntry()){
      Int_t hodo_id=fFTPB_hodoID->Val();
      fFTHODOClust_id->FindEntry(hodo_id);
      if(fFTHODOClustBank->GetEntry()<0) continue;
      if(fFTHODOClust_size->Val()<2&&GotOne==1){
	gamma2.SetXYZM(fFTPB_cx->Val(),
		       fFTPB_cy->Val(),
		       fFTPB_cz->Val(),0);
	gamma2.SetE(fFTPB_e->Val());
 	gamma2.SetRho(fFTPB_e->Val());
	GotOne=2;
	break;
      }
      if(fFTHODOClust_size->Val()<2&&!GotOne){
	gamma1.SetXYZM(fFTPB_cx->Val(),
		       fFTPB_cy->Val(),
		       fFTPB_cz->Val(),0);
	gamma1.SetE(fFTPB_e->Val());
 	gamma1.SetRho(fFTPB_e->Val());
	GotOne++;
      }
      
    }
    if(GotOne==2) Histogram1D("FT_Ho-Ca_M2GammaB")->Fill((gamma1+gamma2).M());
  }
      //now reset
  fFTParticlesBank->ResetEntry();

}
/////////////////////////////////////////////////////////////////////////
///Declare FT Event Builder histograms
void THSHipoFTMon::Declare_RECFT_Histograms(){

  fHistograms->Add(new TH1F("FT_Energy","Energy of REC::ForwardTagger",120,0,12));
  fHistograms->Add(new TH1F("FT_Time","Time of REC::ForwardTagger",200,-200,200));
  
}
/////////////////////////////////////////////////////////////////////////
///Fill FT Event Builder histogams
void THSHipoFTMon::Fill_RECFT_Histograms(){

  //Loop over entries in REC::Forward Tagger bank for this event
  while(fFTBank->NextEntry()){
    //fFTBank decared in THSHipoReader
    Histogram1D("FT_Energy")->Fill(fFTREC_e->Val());
    Histogram1D("FT_Time")->Fill(fFTREC_t->Val());
  } 
}
/////////////////////////////////////////////////////////////////////////
///Declare histograms based on Event Builder particles
void THSHipoFTMon::Declare_Particle_Histograms(){
  // Reconstructed particle histograms using REC::Particle
  // converted to THSParticles

  //Hists for -ve FT
  fHistograms->Add(new TH1F("FT-CD_Theta-","-ve #theta(CD) in conc. with FT",180,0,180));
  fHistograms->Add(new TH1F("FT-CD_FTEnergy-","-ve E(FT) in conc. with CD",100,0,12));
  fHistograms->Add(new TH1F("FT-M2pi-","-ve Invariant mass #pi+ #pi-",100,0,2));
  fHistograms->Add(new TH1F("FT-Z2pi-","-ve DOCA z-vertex  for #pi+ #pi-",200,-100,100));
  //  fHistograms->Add(new TH1F("","",180,0,180));
  fHistograms->Add(new TH1F("FT-Pip_Time-","-ve Coinc. Time FT and #pi+",300,200,500));
  fHistograms->Add(new TH1F("FT-Pip_Theta-","-ve Coinc. FT #pi+ #theta",180,0,180));
  //Hists for neutral FT
  fHistograms->Add(new TH1F("FT-CD_Theta0","Neutral #theta(CD) in conc. with FT",180,0,180));
  fHistograms->Add(new TH1F("FT-CD_FTEnergy0","Neutral E(FT) in conc. with CD",100,0,12));
  fHistograms->Add(new TH1F("FT-M2pi0","Neutral Invariant mass #pi+ #pi-",100,0,2));
  fHistograms->Add(new TH1F("FT-Z2pi0","Neutral DOCA z-vertex  for #pi+ #pi-",200,-100,100));
  //  fHistograms->Add(new TH1F("","",180,0,180));
  fHistograms->Add(new TH1F("FT-Pip_Time0","Neutral Coinc. Time FT and #pi+",300,200,500));
  fHistograms->Add(new TH1F("FT-Pip_Theta0","Neutral Coinc. FT  #pi+ #theta",180,0,180));

}
/////////////////////////////////////////////////////////////////////////
///Fill event builder particle histograms using THSParticle class
///The THSParticles are filled in the base class THSHipoReader
void THSHipoFTMon::Fill_Particle_Histograms(){
  //Take events with 1 FT particle
  Int_t ift=GetSingleFTParticle();
  Int_t icd=GetSingleCDParticle();

  //+ve in CD =>Elastics
  if(icd>-1&&ift>-1&&fParticles[icd]->Charge()==1){
    if(fParticles[ift]->Charge()==-1){
      Histogram1D("FT-CD_Theta-")->Fill(fParticles[icd]->P4().Theta()*TMath::RadToDeg());
      Histogram1D("FT-CD_FTEnergy-")->Fill(fParticles[ift]->P4().E());
    }
    else{
      Histogram1D("FT-CD_Theta0")->Fill(fParticles[icd]->P4().Theta()*TMath::RadToDeg());
      Histogram1D("FT-CD_FTEnergy0")->Fill(fParticles[ift]->P4().E());
    }
  }

  //pi+ pi-
  Int_t ipip=GetSinglePip();
  Int_t ipim=GetSinglePim();
  if(ipip>-1&&ipim>-1&&ift>-1){
    THSParticle sum;
    sum.Add(fParticles[ipip],fParticles[ipim]);
    if(fParticles[ift]->Charge()==-1){
      Histogram1D("FT-M2pi-")->Fill(sum.P4().M());
      Histogram1D("FT-Z2pi-")->Fill(sum.Vertex().Z());
    }
    else  {
      Histogram1D("FT-M2pi0")->Fill(sum.P4().M());
      Histogram1D("FT-Z2pi0")->Fill(sum.Vertex().Z());
    }
  }

  //Single pi+
   if(ipip>-1&&ift>-1){
     if(fParticles[ift]->Charge()==-1){
       Histogram1D("FT-Pip_Theta-")->Fill(fParticles[ipip]->P4().Theta()*TMath::RadToDeg());
       Histogram1D("FT-Pip_Time-")->Fill(fParticles[ipip]->Time()-fParticles[ift]->Time());
     }
     else{
       Histogram1D("FT-Pip_Theta0")->Fill(fParticles[ipip]->P4().Theta()*TMath::RadToDeg());
       Histogram1D("FT-Pip_Time0")->Fill(fParticles[ipip]->Time()-fParticles[ift]->Time());
     }

   }

}

/////////////////////////////////////////////////////////////////////////
THSHipoFTMon::THSHipoFTMon(){
}
/////////////////////////////////////////////////////////////////////////
///intialse the hipo banks required in this class
Bool_t THSHipoFTMon::Init(TString filename,TString name){

  if(!fFTParticlesBank){
    //Add the trigger banks to those to be configured
    //This only needs done for first file
    Info("THSHipoFTMon","Configure banks");
    fHipo->ConfigOnlyBank("FT::particles");
    fHipo->ConfigOnlyBank("FTCAL::clusters");
    fHipo->ConfigOnlyBank("FTHODO::clusters");
  }
  //Init THSHipoTrigger for trigger info and integrated Gated Charge
  //This function will configure all the banks specified above
  THSHipoTrigger::Init(filename,name);
  
  if(!fFTParticlesBank){
    //Get the configured banks
    fFTParticlesBank=fHipo->GetBank("FT::particles");
    fFTCALClustBank=fHipo->GetBank("FTCAL::clusters");
    fFTHODOClustBank=fHipo->GetBank("FTHODO::clusters");

    fFTREC_e=fFTBank->GetItem("energy");
    fFTREC_t=fFTBank->GetItem("time");

    fFTPB_hodoID=fFTParticlesBank->GetItem("hodoID");
    fFTPB_calID=fFTParticlesBank->GetItem("calID");
    fFTPB_ch=fFTParticlesBank->GetItem("charge");
    fFTPB_cx=fFTParticlesBank->GetItem("cx");
    fFTPB_cy=fFTParticlesBank->GetItem("cy");
    fFTPB_cz=fFTParticlesBank->GetItem("cz");
    fFTPB_e=fFTParticlesBank->GetItem("energy");
    fFTHODOClust_id=fFTHODOClustBank->GetItem("id");
    fFTHODOClust_x=fFTHODOClustBank->GetItem("x");
    fFTHODOClust_y=fFTHODOClustBank->GetItem("y");
    fFTHODOClust_z=fFTHODOClustBank->GetItem("z");
    fFTHODOClust_e=fFTHODOClustBank->GetItem("energy");
    fFTHODOClust_t=fFTHODOClustBank->GetItem("time");
    fFTHODOClust_size=fFTHODOClustBank->GetItem("size");
    fFTCALClust_id=fFTCALClustBank->GetItem("id");
    fFTCALClust_x=fFTCALClustBank->GetItem("x");
    fFTCALClust_y=fFTCALClustBank->GetItem("y");
    fFTCALClust_e=fFTCALClustBank->GetItem("energy");
    fFTCALClust_t=fFTCALClustBank->GetItem("time");
    fFTCALClust_size=fFTCALClustBank->GetItem("size");
   }
  
  return kTRUE;
}
////////////////////////////////////////////////////////////////////
///Initialise the output file and declare all histograms
void THSHipoFTMon::InitOutput(TString filename){
  
  fWriteFile=new TFile(filename,"recreate");
  fHistograms=new TList();
  fHistograms->SetName("FTMon Histograms");
  fHistograms->SetOwner(kTRUE);//responsible for deleting

  ///Declare your own histogram function here
  DeclareHistograms();
  Declare_FT_Histograms();
  Declare_RECFT_Histograms();
  Declare_Particle_Histograms();
}
/////////////////////////////////////////////////////////////////////
///write to root and .pdf files
void THSHipoFTMon::CloseOutput(){
  ScaleHistsByCharge();
  Export2PDF();
  fWriteFile->cd();
  SaveSummaryData();
  
  fHistograms->Write();
  delete fWriteFile;
  fWriteFile=nullptr;
  fHistograms=nullptr;
  
}
//////////////////////////////////////////////////////////////////////
/// Get the hipo event, decode into THSParticles and Trigger information
/// in THSHipoTrigger::ReadEvent()
/// Fill histograms
Bool_t THSHipoFTMon::ReadEvent(Long64_t entry){

  //Get the entry and get hipo trigger bank items
  if(!THSHipoTrigger::ReadEvent()) return kFALSE;



  fHipo->ResetEntries(); //Make sure all banks back to first entry

  //Fill histograms
  FillHistograms();
  //Only fill rest of histograms if FT trig
  if(!fTrigFT) return kTRUE;
  Fill_FT_Histograms();
  Fill_RECFT_Histograms();
  Fill_Particle_Histograms();
  
  return kTRUE;

}
///////////////////////////////////////////////////////////////////////
///Do something after fWriteTree->Fill() NOT needed here
void  THSHipoFTMon::PostWrite(){
  THSHipoTrigger::PostWrite(); //reset current 
}
///////////////////////////////////////////////////////////////////////
///Get the fParticles index for a single FT hit
Int_t THSHipoFTMon::GetSingleFTParticle(){
  
  UInt_t ift=0;
  if(fFTBank->GetEntries()==1){
    //Find FT particle (detector # <0)
    for(ift =0;ift<fParticles.size();ift++)
      if(fParticles[ift]->Detector()<0) break;
  }
  else return -1;

  return ift;
  
}
///////////////////////////////////////////////////////////////////////
///Get the fParticles index for a CD hit
Int_t THSHipoFTMon::GetSingleCDParticle(){
  //Look for coincidence with 1 particle in CD (detector # = 10000)
  Int_t icd=-1;
  for(UInt_t i =0;i<fParticles.size();i++)
    // if(fParticles[i]->Detector()==10000){//based o CTOF not currently working
    if(fParticles[i]->P4p()->Theta()*TMath::RadToDeg()>35){
      if(icd==-1) icd =i;
      else {icd=-1;break;} //more than 1 particle in CD
    }

  return icd;
}
///////////////////////////////////////////////////////////////////////
///Get the fParticles index for a single identified pi+
Int_t THSHipoFTMon::GetSinglePip(){
  Int_t ipip=-1;
  for(UInt_t i =0;i<fParticles.size();i++)
    // if(fParticles[i]->Detector()==10000){//based o CTOF not currently working
    if(fParticles[i]->PDG()==211){
      if(ipip==-1) ipip =i;
      else {ipip=-1;break;} //more than 1 particle in CD
    }

  return ipip;

}
///////////////////////////////////////////////////////////////////////
///Get the fParticles index for a single identified pi-
Int_t THSHipoFTMon::GetSinglePim(){
  Int_t ipip=-1;
  for(UInt_t i =0;i<fParticles.size();i++)
    // if(fParticles[i]->Detector()==10000){//based o CTOF not currently working
    if(fParticles[i]->PDG()==-211){
      if(ipip==-1) ipip =i;
      else {ipip=-1;break;} //more than 1 particle in CD
    }

  return ipip;

}
///////////////////////////////////////////////////////////////////////
///Scale all histograms by the integrated charge for this file
void THSHipoFTMon::ScaleHistsByCharge(){

  for(Int_t i=0;i<fHistograms->GetEntries();i++)
    ((TH1*)fHistograms->At(i))->Scale(1./fTotCharge);

}
//////////////////////////////////////////////////////////////////////
///Make .pdf files of all the histograms
void THSHipoFTMon::Export2PDF(){

  //Get output rootfile name and make pdf equivalent
  TString pdfname=fWriteFile->GetName();
  pdfname.ReplaceAll(".root",".pdf");


  //REC histograms
  TCanvas canREC("REC","Event Builder REC items");
  canREC.Divide(2,2);
  canREC.cd(1);
  Histogram1D("NParticles")->Draw("hist");
  canREC.cd(2);
  Histogram1D("NFT")->Draw("hist");
  canREC.cd(3);
  Histogram1D("Trig")->Draw("hist");
  canREC.cd(4);
  Histogram1D("NFT_trig")->Draw("hist");

  canREC.Print(pdfname+TString("(")); //Open pdf file

  //FT clusters Histograms
  TCanvas canFTCh("FTCh","FT Charge Dependent Banks items");
  canFTCh.Divide(3,2);
  canFTCh.cd(1);
  Histogram1D("FT_Ca_Charge")->Draw("hist");
  canFTCh.cd(2);
  Histogram2D("FT_Ca_YVX-")->Draw("col1");
  canFTCh.cd(3);
  Histogram2D("FT_Ca_YVX0")->Draw("col1");
  canFTCh.cd(4);
  Histogram2D("FT_Ca_YVX1a")->Draw("col1");
  canFTCh.cd(5);
  Histogram2D("FT_Ca_YVX1b")->Draw("col1");
  canFTCh.Print(pdfname);
  
  TCanvas canFT("FT","FT Banks items");
  canFT.Divide(5,2);
  canFT.cd(1);
  Histogram1D("FT_Ho-Ca_X")->Draw("hist");
  canFT.cd(2);
  Histogram1D("FT_Ho-Ca_Y")->Draw("hist");
  canFT.cd(3);
  Histogram1D("FT_Ho-Ca_Time")->Draw("hist");
  canFT.cd(4);
  Histogram1D("FT_Ca_Size-")->Draw("hist");
  Histogram1D("FT_Ca_Size0")->SetLineColor(1);
  Histogram1D("FT_Ca_Size0")->Draw("hist same");
  canFT.cd(5);
  Histogram1D("FT_Ho_Size-")->Draw("hist");

  canFT.cd(6);
  Histogram1D("FT_Ho-Ca_CalE")->Draw("hist");
  Histogram1D("FT_Ho-Ca_CalE1")->SetLineColor(2);
  Histogram1D("FT_Ho-Ca_CalE1")->Draw("hist same");
  Histogram1D("FT_Ho-Ca_CalE0")->SetLineColor(1);
  Histogram1D("FT_Ho-Ca_CalE0")->Draw("hist same");
  canFT.cd(7);
  Histogram1D("FT_Ho-Ca_CalE1b")->Draw("hist");
  Histogram1D("FT_Ho-Ca_CalE1a")->SetLineColor(2);
  Histogram1D("FT_Ho-Ca_CalE1a")->Draw("hist same");
  canFT.cd(8);
  Histogram1D("FT_Ho-Ca_HodE")->Draw("hist");
  canFT.cd(9);
  Histogram1D("FT_Ho-Ca_M2Gamma")->Draw("hist");
  canFT.cd(10);
  Histogram1D("FT_Ho-Ca_M2GammaB")->Draw("hist");
  canFT.Print(pdfname); //Close pdf file

  //FT Particle Histograms
  TCanvas canPn("Particle-","-ve FT Particle spectra");
  canPn.Divide(3,2);
  canPn.cd(1);
  Histogram1D("FT-CD_Theta-")->Draw("hist");
  canPn.cd(4);
  Histogram1D("FT-CD_FTEnergy-")->Draw("hist");
  canPn.cd(2);
  Histogram1D("FT-M2pi-")->Draw("hist");
  canPn.cd(5);
  Histogram1D("FT-Z2pi-")->Draw("hist");
  canPn.cd(3);
  Histogram1D("FT-Pip_Time-")->Draw("hist");
  canPn.cd(6);
  Histogram1D("FT-Pip_Theta-")->Draw("hist");
  canPn.Print(pdfname); 

  TCanvas canP0("Particle0","Neutral FT Particle spectra");
  canP0.Divide(3,2);
  canP0.cd(1);
  Histogram1D("FT-CD_Theta0")->Draw("hist");
  canP0.cd(4);
  Histogram1D("FT-CD_FTEnergy0")->Draw("hist");
  canP0.cd(2);
  Histogram1D("FT-M2pi0")->Draw("hist");
  canP0.cd(5);
  Histogram1D("FT-Z2pi0")->Draw("hist");
  canP0.cd(3);
  Histogram1D("FT-Pip_Time0")->Draw("hist");
  canP0.cd(6);
  Histogram1D("FT-Pip_Theta0")->Draw("hist");
 
  canP0.Print(pdfname+TString(")")); //Close pdf file

}

void THSHipoFTMon::SaveSummaryData(){
  TTree* tree=new TTree("History","Summary data from FTMonitor");
  //Get numbers, removing bins with 0 hits
  Float_t NAll=Histogram1D("NParticles")->Integral(2,100);
  Float_t NFT=Histogram1D("NFT")->Integral(2,10);
  Float_t NFT_trig=Histogram1D("NFT_trig")->Integral(2,10);
  Float_t NPer=NFT_trig/NAll*100;

  Float_t MeanHodoE=Histogram1D("FT_Ho-Ca_HodE")->GetMean();
  Float_t MeanCalE=Histogram1D("FT_Ho-Ca_CalE")->GetMean();
  Float_t NFTpi0=Histogram1D("FT_Ho-Ca_M2Gamma")->Integral(Histogram1D("FT_Ho-Ca_M2Gamma")->FindFixBin(0.06),Histogram1D("FT_Ho-Ca_M2Gamma")->FindFixBin(0.15));
  Float_t Np=Histogram1D("FT_Ca_Charge")->Integral(1,1);
  Float_t MeanCurrent=fTotCharge/fNScalerReads/0.033;
  
  tree->Branch("NAll",&NAll,"NAll/F");
  tree->Branch("NFT",&NFT,"NFT/F");
  tree->Branch("NFT_trig",&NFT_trig,"NFT_trig/F");
  tree->Branch("NPer",&NPer,"NPer/F");
  tree->Branch("MeanHodoE",&MeanHodoE,"MeanHodoE/F");
  tree->Branch("MeanCalE",&MeanCalE,"MeanCalE/F");
  tree->Branch("NFTpi0",&NFTpi0,"NFTpi0/F");
  tree->Branch("Np",&Np,"Np/F");
  tree->Branch("TotCharge",&fTotCharge,"TotCharge/F");
  tree->Branch("MeanCurrent",&MeanCurrent,"MeanCurrent/F");

  fWriteFile->Write();
  tree->Fill();
  tree->Write();
  
}
void THSHipoFTMon::History(TChain* chain,TString outname){

  Float_t NAll;
  Float_t NFT;
  Float_t NPer;
  Float_t NFT_trig;
  Float_t MeanHodoE;
  Float_t MeanCalE;
  Float_t NFTpi0;
  Float_t Np;
  Float_t MeanCurrent;

  chain->SetBranchAddress("NAll",&NAll);
  chain->SetBranchAddress("NFT",&NFT);
  chain->SetBranchAddress("NPer",&NPer);
  chain->SetBranchAddress("NFT_trig",&NFT_trig);
  chain->SetBranchAddress("MeanHodoE",&MeanHodoE);
  chain->SetBranchAddress("MeanCalE",&MeanCalE);
  chain->SetBranchAddress("NFTpi0",&NFTpi0);
  chain->SetBranchAddress("Np",&Np);
  chain->SetBranchAddress("MeanCurrent",&MeanCurrent);

  TFile* fileh=new TFile(outname,"recreate");
  Int_t Nent=chain->GetEntries();
  TH1F* hNAll=new TH1F("NAll","Number of REC bank particles",Nent+1,0,Nent+1);
  TH1F* hNFT=new TH1F("NFT","Number of REC bank FT particles",Nent+1,0,Nent+1);
  TH1F* hNPer=new TH1F("NPer","Percentage of REC particles from FT",Nent+1,0,Nent+1);
  TH1F* hNFT_trig=new TH1F("NFT_trig","Number of FT particles when FT makes trigger",Nent+1,0,Nent+1);
  TH1F* hMeanHodoE=new TH1F("MeanHodoE","Mean Hodoscope Energy",Nent+1,0,Nent+1);
  TH1F* hMeanCalE=new TH1F("MeanCalE","Mean Calorimter Energy",Nent+1,0,Nent+1);
  TH1F* hNFTpi0=new TH1F("NFTpi0","Number of 2 gamma = #pi0 in FT ",Nent+1,0,Nent+1);
  TH1F* hNp=new TH1F("Np","Number of charged particles in FT",Nent+1,0,Nent+1);
  TH1F* hMeanCurrent=new TH1F("MeanCurrent","Mean Current",Nent+1,0,Nent+1);
  
  
  for(Int_t i=0;i<Nent;i++){
    chain->GetEntry(i);
    AddHistPoint(hNAll,i,NAll);
    AddHistPoint(hNFT,i,NFT);
    AddHistPoint(hNPer,i,NPer);
    AddHistPoint(hNFT_trig,i,NFT_trig);
    AddHistPoint(hMeanHodoE,i,MeanHodoE);
    AddHistPoint(hMeanCalE,i,MeanCalE);
    AddHistPoint(hNFTpi0,i,NFTpi0);
    AddHistPoint(hNp,i,Np);
    AddHistPoint(hMeanCurrent,i,MeanCurrent);
  }
 
  hNAll->Write();
  hNFT->Write();
  hNPer->Write();
  hNFT_trig->Write();
  hMeanHodoE->Write();
  hMeanCalE->Write();
  hNFTpi0->Write();
  hNp->Write();
  hMeanCurrent->Write();
  delete fileh;
  
}
void THSHipoFTMon::AddHistPoint(TH1F* his,Int_t i,Float_t val){
  his->SetBinContent(i+1,val);
  cout<<i<<" "<<val<<endl;
  his->GetXaxis()->SetBinLabel(i+1,fFileNumbers[i]);
    
}
void THSHipoFTMon::GetFileNumbers(TChain* chain){

  fFileNumbers.clear();

  for(Int_t ifi=0;ifi<chain->GetNtrees();ifi++){
    TString filen=chain->GetListOfFiles()->At(ifi)->GetTitle();
    TString base(gSystem->BaseName(filen));
    TString sNum0(base(9,6));//e.g. out_clas_002835
    TString sNum1(base(21,base.Sizeof()-32));
    TString filabel=sNum0+"."+sNum1;
    fFileNumbers.push_back(filabel);
    cout<<filabel<<endl;
  }
    
}
