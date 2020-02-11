#ifndef __TaAccumulator_hh__
#define __TaAccumulator_hh__
#include "Rtypes.h"
#include "TObject.h"
#include "TMath.h"

typedef struct{Double_t mean,err,rms,num_samples;} STAT;

class TaAccumulator:public TObject{
public:
  TaAccumulator();
  TaAccumulator(Double_t m2_2, Double_t mu2, Double_t n2);
  virtual ~TaAccumulator();
  void Update(Double_t m2_2, Double_t mu2, Double_t n2);
  void Merge( TaAccumulator);
  void UpdateRunStat(STAT &input);
  void Zero();
  inline Double_t GetM2() const {return M2;};
  inline Double_t GetNSamples() const {return nsamples;};
  inline Double_t GetMean() const {return mu;};
  inline Double_t GetMeanError() const {return  TMath::Sqrt(M2)/nsamples;};
  inline Double_t GetRMS() const {return  TMath::Sqrt(M2/nsamples);};
private:
  Double_t M2;
  Double_t mu;
  Double_t nsamples;

  ClassDef(TaAccumulator,0);
};

typedef std::vector<TaAccumulator> AccumulatorArray;
#endif
