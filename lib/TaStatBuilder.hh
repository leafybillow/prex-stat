#ifndef __TASTATBUILDER_HH__
#define __TASTATBUILDER_HH__

class StatData{
public:
  StatData(){ Zero(); };
  virtual ~StatData(){};
  Double_t mean;
  Double_t error;
  Double_t rms;
  Double_t num_samples;
  Double_t m2;
  Double_t chi2;
  Double_t ndf;
  
  void Zero(){
    mean =0.0;
    error =0.0;
    rms =0.0;
    num_samples =0.0;
    m2 = 0.0;
    chi2 = 0.0;
    ndf = 0.0;
  };
  void RegisterAddressByName_postpan(TBranch* aBranch){
    aBranch->GetLeaf("mean")->SetAddress(&mean);
    aBranch->GetLeaf("rms")->SetAddress(&rms);
    aBranch->GetLeaf("err")->SetAddress(&error);
  };
  
  void SetChi2NDF(Double_t a,Double_t b){ chi2=a;ndf=b;};
  
  ClassDef(StatData,0);
};
typedef vector<StatData> StatDataArray;
class TaStatBuilder{
public:
  TaStatBuilder(){};
  virtual ~TaStatBuilder(){};

  void UpdateStatBuilder(TaStatBuilder,Int_t sign=1);
  void UpdateStatBuilderByIHWP(TaStatBuilder,TString,Int_t sign=1);
  void Process();
  void CalcAverages();
  TaStatBuilder GetNullStatBuilder();
  
  void LoadRunInfo(TaRunInfo* aRunInfo);

  void UpdateMainDet(StatData);

  void UpdateWeightedAverage(StatData &tgt,StatData input,Int_t sign=1);
  void UpdateLocalAverage(StatData &tgt,StatData input,Int_t sign=1);
  void UpdateCentralMoment(StatData &tgt,StatData input,Int_t sign=1);
  StatData GetNullAverage(StatData in,StatData out);
  void UpdateStatDataByName(TString chname, StatData input,Int_t sign=1);

  void PullFitAllChannels(TString filename);
  void FillTree(TTree *,TString prefix="");
  void ProcessNullAsym(TTree*);
  void SetTmpTitle(TString input){ fTitle_tmp = input;};
  map<TString,StatData> fWeightedAverageMap;
  map<TString,StatData> fLocalAverageMap;
  map<TString,StatData> fCentralMomentMap;
  vector<TString> fDeviceNameList;
private:
  Double_t weighting_errorbar; 
  Int_t fSign;
  
  map<TString, StatDataArray> fStatDataArrayMap;
  vector<TString> fAxisTitle;
  TString fTitle_tmp;
  vector<TString> fIVNameList;
  vector<TString> fDVNameList;
  map<TString,TaStatBuilder> fStatBuilderByIHWP;
  
  ClassDef(TaStatBuilder,0);
};


#endif
