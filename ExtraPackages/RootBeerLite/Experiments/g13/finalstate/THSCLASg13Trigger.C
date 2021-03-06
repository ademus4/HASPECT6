/**
	\class THSCLASg13Trigger
	
	Class to handle issues related to the CLAS g13 experimental trigger
	Includes, 

*/
#include "THSCLASg13Trigger.h"
#include <TH1.h>
#include <TSpectrum.h>
//HSParticles->Draw("Particles.DeltaTime()-EventInfo.fCJSTTime+(Particles[0].Vertex().Z()+20)/TMath::C()*1E9/100 >>(1000,-50,50)","Particles.fPDGCode==-22","col1")
//Note Sim Beam does not need +20

////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTimeBeam(THSParticle* part,THSParticle* beam){
  SetStartTime();
  Float_t offset=20*(!fIsSim);
  Float_t startime=fStartTime-(part->Vertex().Z()+offset)/TMath::C()*1E9/100;
  beam->ShiftTime(-startime);
}
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part){
  SetStartTime();
  Float_t startime=fStartTime-(part->Vertex().Z()+20)/TMath::C()*1E9/100;
  part->ShiftTime(-startime);
}
////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1){
  SetStartTime();
  Float_t startime=fStartTime-(part0->Vertex().Z()+20)/TMath::C()*1E9/100;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
}
////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2){
  SetStartTime();
  Float_t startime=fStartTime-(part0->Vertex().Z()+20)/TMath::C()*1E9/100;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
}
////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3){
  SetStartTime();
  Float_t startime=fStartTime-(part0->Vertex().Z()+20)/TMath::C()*1E9/100;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
}
////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4){
  SetStartTime();
  Float_t startime=fStartTime-(part0->Vertex().Z()+20)/TMath::C()*1E9/100;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
}
////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5){
  SetStartTime();
  Float_t startime=fStartTime-(part0->Vertex().Z()+20)/TMath::C()*1E9/100;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
}
////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6){
  SetStartTime();
  Float_t startime=fStartTime-(part0->Vertex().Z()+20)/TMath::C()*1E9/100;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
  part6->ShiftTime(-startime);
}
////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7){
  SetStartTime();
  Float_t startime=fStartTime-(part0->Vertex().Z()+20)/TMath::C()*1E9/100;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
  part6->ShiftTime(-startime);
  part7->ShiftTime(-startime);
}
////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8){
  SetStartTime();
  Float_t startime=fStartTime-(part0->Vertex().Z()+20)/TMath::C()*1E9/100;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
  part6->ShiftTime(-startime);
  part7->ShiftTime(-startime);
  part8->ShiftTime(-startime);
}
////////////////////////////////////////////////////////////////////////
///subtract the event startime from the given particle
///Should be called after fTrigger.StartTime(&fElectron);
void THSCLASg13Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8,THSParticle* part9){
  SetStartTime();
  Float_t startime=fStartTime-(part0->Vertex().Z()+20)/TMath::C()*1E9/100;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
  part6->ShiftTime(-startime);
  part7->ShiftTime(-startime);
  part8->ShiftTime(-startime);
  part9->ShiftTime(-startime);
}
