#ifndef __TASTATBUILDER_HH__
#define __TASTATBUILDER_HH__

#include "TaResult.hh"
#include "TaRunInfo_v2.hh"

#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TF1.h"
#include "TCanvas.h"

#include <map>
class StatData{
public:
  StatData(){ Zero(); };
  virtual ~StatData(){};
  Double_t mean;
  Double_t error;
  Double_t error_scaled;
  Double_t mean_sum;
  Double_t rms;
  Double_t num_samples;
  Double_t m2;
  Double_t chi2;
  Double_t ndf;
  Int_t sign;
  TString tag;
  
  void Zero(){
    mean =0.0;
    error =0.0;
    error_scaled = 0.0;
    mean_sum=0.0;
    rms =0.0;
    num_samples =0.0;
    m2 = 0.0;
    chi2 = 0.0;
    ndf = 0.0;
    sign =1;
  };
  void RegisterAddressByName(TBranch* aBranch){
    aBranch->GetLeaf("mean")->SetAddress(&mean);
    aBranch->GetLeaf("rms")->SetAddress(&rms);
    aBranch->GetLeaf("err")->SetAddress(&error);
  };
  
  void SetChi2NDF(Double_t a,Double_t b){ chi2=a;ndf=b;};
  StatData MultiplyBySlope(Double_t slope){
    StatData fRet;
    fRet.mean = mean * slope;
    fRet.error = fabs(slope)*error;
    fRet.error_scaled = fabs(slope)*error_scaled;
    fRet.mean_sum = mean_sum*slope;
    fRet.rms = rms*fabs(slope);
    fRet.num_samples = num_samples;
    fRet.m2 = pow(slope,2)*m2;
    fRet.chi2 = chi2;
    fRet.ndf = ndf;
    fRet.sign = sign;
    fRet.tag = "corr_"+tag;
    return fRet;
  };
  ClassDef(StatData,0);
};

typedef vector<StatData> StatDataArray;

class TaStatBuilder{
public:
  TaStatBuilder();
  virtual ~TaStatBuilder(){};

  void UpdateStatBuilder(TaStatBuilder*,Int_t sign=1);
  void UpdateStatBuilderByIHWP(TaStatBuilder*,TString,Int_t sign=1);
  void Process();
  void CalcAverages();
  TaStatBuilder* GetNullStatBuilder(Bool_t kBalanced=kTRUE);
  Bool_t HasStatBuilderByIHWP(){
    if(fStatBuilderByIHWP.find("IN")!=fStatBuilderByIHWP.end() &&
       fStatBuilderByIHWP.find("OUT")!=fStatBuilderByIHWP.end())
      return kTRUE;
    else
      return kFALSE;
  }
  TaStatBuilder* GetStatBuilderByIHWP(TString ihwp){
    return fStatBuilderByIHWP[ihwp];
  };
  
  void LoadRunInfo(TaRunInfo* aRunInfo);
  void UpdateWeightingError(StatData input);
  void DisableShortRunCut(){ kShortRunCut=kFALSE;};
  void UpdateStatData(StatData &tgt,StatData input,Int_t sign=1);
  void UpdateStatData(TString chname,StatData input,Int_t sign=1);
  
  void UpdateCentralMoment(StatData &tgt,StatData input,Int_t sign=1);
  // void UpdateCentralMoment(TString chname,StatData input,Int_t sign=1);
  
  StatData GetNullAverage(StatData in1,StatData in2, Int_t sign_flip=1);

  void SetPlotFileName(TString filename){
    plot_filename = filename;
  }
  void PullFitAllChannels(TString filename="");
  void PullFitAllChannelsByIHWP(){
    if(fStatBuilderByIHWP.find("IN")!=fStatBuilderByIHWP.end()){
      TString filename = plot_filename;
      filename.ReplaceAll(".pdf","_hwp_in.pdf");
      fStatBuilderByIHWP["IN"]->PullFitAllChannels(filename);
    }
    if(fStatBuilderByIHWP.find("OUT")!=fStatBuilderByIHWP.end()){
      TString filename = plot_filename;
      filename.ReplaceAll(".pdf","_hwp_out.pdf");
      fStatBuilderByIHWP["OUT"]->PullFitAllChannels(filename);
    }
  };
  void FillTree(TTree *,TString prefix="");
  void ProcessNullAsym(TTree*);
  void SetLabel(TString input){ fLabel_tmp = input;};
  StatDataArray GetStatDataArrayByName(TString name){
    return fStatDataArrayMap[name];};
  
  vector<TString> GetStatDataLabelByName(TString name){
    return fLabelMap[name];};

  StatDataArray GetStatDataArrayByName(TString name,TString ihwp){
    return  fStatBuilderByIHWP[ihwp]->GetStatDataArrayByName(name);};
  
  vector<TString> GetStatDataLabelByName(TString name,TString ihwp){
    return fStatBuilderByIHWP[ihwp]->GetStatDataLabelByName(name);};
  
  vector<TaStatBuilder*> GetStatBuilderArray(){
    return fStatBuilderArray;
  };

  void ReportLog(TaResult &log);
  void ReportLogByIHWP(TaResult &log);
  Double_t LoadScaleFactor(TString, TString &, TString &);
  void RescaleErrorBar();
  void RescaleErrorBarBy(TaStatBuilder*);
  void ReloadChi2NDF();
  void SetStatBuilderSign(Int_t sign){
    fSign = sign; 
  };
  int GetStatBuilderSign(){
    return fSign; 
  };

  map<TString,StatData> fAverageMap;  
  vector<TString> fDeviceNameList;
  map<TString, StatDataArray> fStatDataArrayMap;
  map<TString, vector<TString> >fLabelMap;
  
private:
  Double_t weighting_errorbar;
  Bool_t kUseWeight;
  Bool_t kShortRunCut;
  Int_t fSign;
  TString plot_filename;
  TString fLabel_tmp;
  map<TString,TaStatBuilder*> fStatBuilderByIHWP;
  vector<TaStatBuilder*> fStatBuilderArray;
  //FIXME: NOT in used so far
  vector<TString> fIVNameList; 
  vector<TString> fDVNameList;
  Double_t distance(Double_t a, Double_t b){
    return sqrt( fabs(pow(a,2) - pow(b,2)) );
  }
  ClassDef(TaStatBuilder,0);
};


#endif
