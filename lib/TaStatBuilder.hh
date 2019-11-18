#ifndef __TASTATBUILDER_HH__
#define __TASTATBUILDER_HH__

#include "TaAccumulator.hh"

typedef struct {Double_t mean, error, rms;} StatData;
  
class TaStatBuilder{
public:
  TaStatBuilder(){};
  
  virtual ~TaStatBuilder(){};

  void MergeStatData(StatData, TaRunInfo);
  void AddStatBuilder(TaStatBuilder);
  void LoadMainDetector(StatData *main_det){ primary_stat = main_det;};
  
  void Process();
  void CalcAverages();
  void CalcNullAverages();
  
private:
  TString channel_name;
  TString tree_name;
  Int_t myID;

  StatData* primary_stat;

  StatData weighted_average;
  
  vector<StatData> in_state_data;
  vector<StatData> out_state_data;
  
  StatData local_average;
  StatData sum_average;
  StatData null_average;
  
  ClassDef(TaStatBuilder,0);
};
#endif
