#include "TaAccumulator.hh"

ClassImp(TaAccumulator);

TaAccumulator::TaAccumulator(){
  M2=0;
  mu=0;
  nsamples=0;
}
TaAccumulator::TaAccumulator(Double_t m2_in, 
			     Double_t mu_in,
			     Double_t n_in){
  M2=m2_in;
  mu=mu_in;
  nsamples=n_in;
}
TaAccumulator::~TaAccumulator(){};

void TaAccumulator::Update(Double_t m2_2, Double_t mu2, Double_t n2){
  if(n2!=0){
    double n1 = nsamples;
    nsamples += n2;
    double delta_mu = mu2 - mu;
    M2+= m2_2;
    mu+=n2*delta_mu/nsamples;
    M2+= n1*n2*pow(delta_mu,2)/nsamples;
  }
}

void TaAccumulator::Merge(TaAccumulator other){
  double mu2 = other.GetMean();
  double n2 = other.GetNSamples();
  double M2_2 = other.GetM2();
  Update(M2_2,mu2,n2);
}

void TaAccumulator::Zero(){
  M2=0;
  mu=0;
  nsamples=0;
}

void TaAccumulator::UpdateRunStat(STAT &input){
  input.mean = mu;
  input.num_samples = nsamples;
  if(nsamples>0){
    input.err = TMath::Sqrt(M2)/nsamples;
    input.rms = TMath::Sqrt(M2/nsamples);
  }
  else{
    input.err = -1;
    input.rms = -1;
  }
}
