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

  void Zero(){
    mean =0.0;
    error =0.0;
    rms =0.0;
    num_samples =0.0;
    m2 = 0.0;
  };
  void RegisterAddressByName_postpan(TBranch* aBranch){
    aBranch->GetLeaf("mean")->SetAddress(&mean);
    aBranch->GetLeaf("rms")->SetAddress(&rms);
    aBranch->GetLeaf("err")->SetAddress(&error);
  };
  ClassDef(StatData,0);
};
typedef vector<StatData> StatDataArray;
class TaStatBuilder{
public:
  TaStatBuilder();
  virtual ~TaStatBuilder(){};

  void MergeStatBuilder(TaStatBuilder input);
  void UpdateStatBuilder(TaStatBuilder,Int_t sign=1);
  void UpdateStatBuilderByIHWP(TaStatBuilder,TString,Int_t sign=1);
  void Process();
  void CalcAverages();
  TaStatBuilder GetNullStatBuilder();
  
  void LoadRunInfo(TaRunInfo* aRunInfo);

  void UpdateMainDet(StatData);
  void UpdateChargeAsym(StatData);

  void UpdateWeightedAverage(StatData &tgt,StatData input,Int_t sign=1);
  void UpdateLocalAverage(StatData &tgt,StatData input,Int_t sign=1);
  void UpdateCentralMoment(StatData &tgt,StatData input,Int_t sign=1);
  StatData GetNullAverage(StatData in,StatData out);
  void UpdateStatDataByName(TString chname, StatData input,Int_t sign=1);

  void FillTree(TTree *);
  void FillTreeWithNullAverages(TTree *);
  void ProcessNullAsym(TTree*);

  StatData fMainDetectorAverage;
  StatData fMainDetectorCentralMoment;

private:
  TaRunInfo fCurrentRunInfo;
  Double_t maindet_errorbar; 
  Int_t fSign;
  
  StatData fChargeAsymWeightedAverage;
  StatData fChargeAsymLocalAverage;
  StatData fChargeAsymCentralMoment;
  
  StatDataArray fMainDetectorByMini;  
  StatDataArray fChargeAsymByMini;
  
  map<TString,StatData> fWeightedAverageMap;
  map<TString,StatData> fLocalAverageMap;
  map<TString,StatData> fCentralMomentMap;
  
  map<TString, StatDataArray> fStatDataArrayMap;
  
  vector<TString> fIVNameList;
  vector<TString> fDVNameList;
  map<TString,TaStatBuilder> fStatBuilderByIHWP;
  
  ClassDef(TaStatBuilder,0);
};


#endif
