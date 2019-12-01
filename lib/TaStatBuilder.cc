#include "TaStatBuilder.hh"
ClassImp(TaStatBuilder)
TaStatBuilder::TaStatBuilder(){
  StatData fZero;
  fZero.Zero();
  fMainDetectorAverage = fZero;
  fMainDetectorCentralMoment = fZero;
  fChargeAsymWeightedAverage = fZero;
  fChargeAsymLocalAverage = fZero;
  fChargeAsymCentralMoment = fZero;
}

void TaStatBuilder::UpdateMainDet(StatData input){
  fStatDataArrayMap["main_det"].push_back(input);
  maindet_errorbar = input.error;
  UpdateLocalAverage(fMainDetectorAverage,input);
  UpdateCentralMoment(fMainDetectorCentralMoment,input);
}

void TaStatBuilder::UpdateChargeAsym(StatData input){
  fStatDataArrayMap["Aq"].push_back(input);
  UpdateWeightedAverage(fChargeAsymWeightedAverage,input);
  UpdateLocalAverage(fChargeAsymLocalAverage,input);
  UpdateCentralMoment(fChargeAsymCentralMoment,input);
}

void TaStatBuilder::UpdateStatDataByName(TString chname,StatData input,Int_t sign){
  fStatDataArrayMap[chname].push_back(input);
}


void TaStatBuilder::UpdateWeightedAverage(StatData &target,
					  StatData input,Int_t sign){
  if(input.error<=0)
    return;

  if(target.error==0){
    target.mean = sign*input.mean;
    target.error = maindet_errorbar;
    // no point to do rms calculation
  }else{
    Double_t mean1= target.mean;
    Double_t weight1= pow(target.error,-2);
    Double_t mean2 = sign*input.mean;
    Double_t weight2 = pow(maindet_errorbar,-2);
    Double_t weight_sum = weight1+weight2;
    target.mean = (mean1*weight1 + mean2*weight2)/weight_sum;
    target.error =  sqrt(1.0/weight_sum);
  }
}

void TaStatBuilder::UpdateLocalAverage(StatData &target,
				       StatData input,Int_t sign){
  if(input.error<=0)
    return;

  if(target.error==0){
    target.mean = sign*input.mean;
    target.error = input.error;
    // no point to do rms calculation
  }else{
    Double_t mean1= target.mean;
    Double_t weight1= pow(target.error,-2);
    Double_t mean2 = sign*input.mean;
    Double_t weight2 = pow(input.error,-2);
    Double_t weight_sum = weight1+weight2;
    target.mean = (mean1*weight1 + mean2*weight2)/weight_sum;
    target.error =  sqrt(1.0/weight_sum);
  }
}

void TaStatBuilder::UpdateCentralMoment(StatData &target,
					StatData input,Int_t sign){
  if(input.error<=0)
    return;

  Double_t mean2 = sign*input.mean;
  Double_t error2 = input.error;
  Double_t rms2 = input.rms;
  
  Double_t nsamples2 = 0.0;
  if(error2!=0)
    nsamples2= pow(rms2/error2,2);
  Double_t M2_2 = pow(rms2,2)*nsamples2;

  if(nsamples2!=0){
    Double_t mean1 = target.mean;
    Double_t error1 = target.error;
    Double_t rms1 = target.rms;
    Double_t nsamples1=0.0;
    if(error1!=0)
      nsamples1 = pow(rms1/error1,2);
    Double_t M2_1 = pow(rms1,2)*nsamples1;
    Double_t delta_mean = mean2-mean1;
    M2_1 += M2_2;
    M2_1 += nsamples1*nsamples2*pow(delta_mean,2)/(nsamples1+nsamples2);
    target.mean += nsamples2*delta_mean/(nsamples1+nsamples2);
    nsamples1+=nsamples2;
    target.rms = sqrt(M2_1/nsamples1);
    target.error = target.rms/sqrt(nsamples1);
  }
}

StatData TaStatBuilder::GetNullAverage(StatData in, StatData out){
  StatData fRetStatData;
  fRetStatData.mean = (in.mean + out.mean)/2.0;
  fRetStatData.error = sqrt(in.error*in.error+out.error*out.error)/2.0;
  return fRetStatData;
}

void TaStatBuilder::FillTree(TTree *fTree){
  TBranch* fBranchMD = fTree->GetBranch("Adet");
  if(fBranchMD==NULL){
    fBranchMD=fTree->Branch("Adet",0,"mean/D:error:mean_sum:error_sum:rms");
  }
    
  fBranchMD->GetLeaf("mean")->SetAddress(&(fMainDetectorAverage.mean));
  fBranchMD->GetLeaf("error")->SetAddress(&(fMainDetectorAverage.error));
  fBranchMD->GetLeaf("mean_sum")->SetAddress(&(fMainDetectorCentralMoment.mean));
  fBranchMD->GetLeaf("error_sum")->SetAddress(&(fMainDetectorCentralMoment.error));
  fBranchMD->GetLeaf("rms")->SetAddress(&(fMainDetectorCentralMoment.rms));

  TBranch* fBranchAq = fTree->GetBranch("Aq");
  if(fBranchAq==NULL){
    fBranchAq=fTree->Branch("Aq",0,"mean/D:error:mean_local:error_local:mean_sum:error_sum:rms");
  }
  
  fBranchAq->GetLeaf("mean")->SetAddress(&(fChargeAsymWeightedAverage.mean));
  fBranchAq->GetLeaf("error")->SetAddress(&(fChargeAsymWeightedAverage.error));
  fBranchAq->GetLeaf("mean_local")->SetAddress(&(fChargeAsymLocalAverage.mean));
  fBranchAq->GetLeaf("error_local")->SetAddress(&(fChargeAsymLocalAverage.error));

  fBranchAq->GetLeaf("mean_sum")->SetAddress(&(fChargeAsymCentralMoment.mean));
  fBranchAq->GetLeaf("error_sum")->SetAddress(&(fChargeAsymCentralMoment.error));
  fBranchAq->GetLeaf("rms")->SetAddress(&(fChargeAsymCentralMoment.rms));

}
void TaStatBuilder::MergeStatBuilder(TaStatBuilder input){
  UpdateMainDet(input.fMainDetectorAverage);
}

void TaStatBuilder::UpdateStatBuilderByIHWP(TaStatBuilder finput,
					    TString ihwp,Int_t sign){

  fStatBuilderByIHWP[ihwp].UpdateStatBuilder(finput);
  UpdateStatBuilder(finput,sign);
}
void TaStatBuilder::UpdateStatBuilder(TaStatBuilder finput ,Int_t sign){
  maindet_errorbar = (finput.fMainDetectorAverage).error;
  
  UpdateLocalAverage(fMainDetectorAverage,
		     finput.fMainDetectorAverage,sign);
  UpdateCentralMoment(fMainDetectorCentralMoment,
		      finput.fMainDetectorCentralMoment,sign);
  
  UpdateWeightedAverage(fChargeAsymWeightedAverage,
			finput.fChargeAsymWeightedAverage,sign);
  UpdateLocalAverage(fChargeAsymLocalAverage,
		     finput.fChargeAsymLocalAverage,sign);
  UpdateCentralMoment(fChargeAsymCentralMoment,
		      finput.fChargeAsymCentralMoment,sign);
}

TaStatBuilder TaStatBuilder::GetNullStatBuilder(){
  TaStatBuilder fNullStatBuilder;
  TaStatBuilder inStatBuilder = fStatBuilderByIHWP["IN"];
  TaStatBuilder outStatBuilder = fStatBuilderByIHWP["OUT"];
  fNullStatBuilder.fMainDetectorAverage = GetNullAverage(inStatBuilder.fMainDetectorAverage,outStatBuilder.fMainDetectorAverage);
  fNullStatBuilder.fMainDetectorCentralMoment = GetNullAverage(inStatBuilder.fMainDetectorCentralMoment,outStatBuilder.fMainDetectorCentralMoment);

  fNullStatBuilder.fChargeAsymWeightedAverage = GetNullAverage(inStatBuilder.fChargeAsymWeightedAverage,outStatBuilder.fChargeAsymWeightedAverage);
  fNullStatBuilder.fChargeAsymLocalAverage = GetNullAverage(inStatBuilder.fChargeAsymLocalAverage,outStatBuilder.fChargeAsymLocalAverage);
  fNullStatBuilder.fChargeAsymCentralMoment = GetNullAverage(inStatBuilder.fChargeAsymCentralMoment,outStatBuilder.fChargeAsymCentralMoment);
  
  return fNullStatBuilder;
}

void TaStatBuilder::FillTreeWithNullAverages(TTree* fTree){
  
  TBranch* fBranchMD = fTree->GetBranch("null_Adet");
  if(fBranchMD==NULL){
    fBranchMD=fTree->Branch("null_Adet",0,"mean/D:error:mean_sum:error_sum");
  }
    
  fBranchMD->GetLeaf("mean")->SetAddress(&(fMainDetectorAverage.mean));
  fBranchMD->GetLeaf("error")->SetAddress(&(fMainDetectorAverage.error));
  fBranchMD->GetLeaf("mean_sum")->SetAddress(&(fMainDetectorCentralMoment.mean));
  fBranchMD->GetLeaf("error_sum")->SetAddress(&(fMainDetectorCentralMoment.error));

  TBranch* fBranchAq = fTree->GetBranch("null_Aq");
  if(fBranchAq==NULL){
    fBranchAq=fTree->Branch("null_Aq",0,"mean/D:error:mean_local:error_local:mean_sum:error_sum");
  }
  
  fBranchAq->GetLeaf("mean")->SetAddress(&(fChargeAsymWeightedAverage.mean));
  fBranchAq->GetLeaf("error")->SetAddress(&(fChargeAsymWeightedAverage.error));
  fBranchAq->GetLeaf("mean_local")->SetAddress(&(fChargeAsymLocalAverage.mean));
  fBranchAq->GetLeaf("error_local")->SetAddress(&(fChargeAsymLocalAverage.error));
  fBranchAq->GetLeaf("mean_sum")->SetAddress(&(fChargeAsymCentralMoment.mean));
  fBranchAq->GetLeaf("error_sum")->SetAddress(&(fChargeAsymCentralMoment.error));

}

void TaStatBuilder::ProcessNullAsym(TTree *fTree){
  TaStatBuilder fNullStatBuilder = GetNullStatBuilder();
  fNullStatBuilder.FillTreeWithNullAverages(fTree);
}
