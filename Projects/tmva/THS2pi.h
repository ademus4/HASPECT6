
#ifndef THS2PI_h
#define THS2PI_h

#include "THSFinalState.h"
#include "THSParticle.h"
#include "THSCLAS12Trigger.h"
#include "THSCLAS12DeltaTime.h"
//#include "THSMVA.h"
#include "THSMVAPrep.h"
#include "THSMVATrain.h"
#include "THSMVAApp.h"
#include <vector>

class THS2pi : public THSFinalState{

 public :
  THS2pi();
  ~THS2pi()=default;


  //Init functions
  void Init_Iter0();
  void Init_Iter1();
  void Init_Iter2();
  void Init_Iter3();
   //Topology action functions
  void Topo_0();
  void Topo_1();
  void Topo_2();
  void Topo_3();
  void Init_Generated();
  //void Init_IterX();
  //void Topo_X();
  virtual void FileStart();
  virtual Bool_t  CheckParticle(THSParticle* part);
  
  void SetApplication(THSMVA* setup);

  void PrepAddParticle(THSParticle* part);
  void PrepFillVars();
  void AppFillVars();

  void Kinematics();
  protected :

  THSCLAS12Trigger fTrigger;//For CLAS12 trigger info
  THSCLAS12DeltaTime fCuts; //For particle cuts

  //THSMVA fMVA;
  THSMVAPrep fMVAPrep;
  THSMVATrain fMVATrain;
  THSMVAApp fMVAApp;
  
  //Initial state
  HSLorentzVector fBeam=HSLorentzVector(0,0,10.6,10.6);
  HSLorentzVector fTarget=HSLorentzVector(0,0,0,0.938272);

  //Final Particles Detected
  THSParticle fElectron=THSParticle("e-");
  THSParticle fProton=THSParticle("proton");
  THSParticle fPip=THSParticle("pi+");
  THSParticle fPim=THSParticle("pi-");
 
  //Final Parents
 
  //Observables
  Double_t f_t;
  Double_t f_Q2;
  Double_t f_W;
  Double_t f_EGamma;
  Double_t f_Pol;
  Double_t fCMCosTh;
  Double_t fCMPhi;

  //Discriminators
  Double_t fMissMass=0;
  Double_t fMissMass2=0;

  // vector for MVA
  vector<THSParticle * > fParticles;


  //TMVA
  Bool_t fIsTMVA=kTRUE;
  Bool_t fIsTrain=kFALSE;
 public:
  virtual void TMVAOutTree(TTree* tree);
  void TMVAFill();
  void RunTraining() {fMVATrain.DefaultTrain();};
  void RunApp() {fMVAApp.DefaultApp();}
  void EndApplication(TFile* file) {fMVAApp.SetOutputFile(file);fMVAApp.Plots();};
  void WriteConfig(TString name) {fMVATrain.WriteTHSMVA(name);};
  void SetTrain(Bool_t b) {fIsTrain = b;};

  virtual void FinalStateOutTree(TTree* tree);

 

};

#endif //ifdef THS2pi
