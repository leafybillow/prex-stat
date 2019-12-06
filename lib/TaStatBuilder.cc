#include "TaStatBuilder.hh"
ClassImp(TaStatBuilder)

void TaStatBuilder::UpdateMainDet(StatData input){
  weighting_errorbar = input.error;
  if(input.error>0.0)
    fAxisTitle.push_back(fTitle_tmp);
  UpdateStatDataByName("Adet",input);
}

void TaStatBuilder::UpdateStatDataByName(TString chname,StatData input,Int_t sign){

  if(fStatDataArrayMap.find(chname)==fStatDataArrayMap.end())
    fDeviceNameList.push_back(chname);
  
  fStatDataArrayMap[chname].push_back(input);
  
  if(fWeightedAverageMap.find(chname)==fWeightedAverageMap.end())
    fWeightedAverageMap[chname].Zero();
  if(fLocalAverageMap.find(chname)==fLocalAverageMap.end())
    fLocalAverageMap[chname].Zero();
  if(fCentralMomentMap.find(chname)==fCentralMomentMap.end())
    fCentralMomentMap[chname].Zero();

  UpdateWeightedAverage(fWeightedAverageMap[chname],input,sign);
  UpdateLocalAverage(fLocalAverageMap[chname],input,sign);
  UpdateCentralMoment(fCentralMomentMap[chname],input,sign);
}


void TaStatBuilder::UpdateWeightedAverage(StatData &target,
					  StatData input,Int_t sign){
  if(input.error<=0)
    return;
  
  if(target.error==0){
    target.mean = sign*input.mean;
    target.error = weighting_errorbar;
  }else{
    Double_t mean1= target.mean;
    Double_t weight1= pow(target.error,-2);
    Double_t mean2 = sign*input.mean;
    Double_t weight2 = pow(weighting_errorbar,-2);
    Double_t weight_sum = weight1+weight2;
    target.mean = (mean1*weight1 + mean2*weight2)/weight_sum;
    target.error =  sqrt(1.0/weight_sum);
  }
}

void TaStatBuilder::UpdateLocalAverage(StatData &dest,
				       StatData input,Int_t sign){
  if(input.error<=0)
    return;

  if(dest.error==0){
    dest.mean = sign*input.mean;
    dest.error = input.error;
    // no point to do rms calculation
  }else{
    Double_t mean1= dest.mean;
    Double_t weight1= pow(dest.error,-2);
    Double_t mean2 = sign*input.mean;
    Double_t weight2 = pow(input.error,-2);
    Double_t weight_sum = weight1+weight2;
    dest.mean = (mean1*weight1 + mean2*weight2)/weight_sum;
    dest.error =  sqrt(1.0/weight_sum);
  }
}

void TaStatBuilder::UpdateCentralMoment(StatData &dest,
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
    Double_t mean1 = dest.mean;
    Double_t error1 = dest.error;
    Double_t rms1 = dest.rms;
    Double_t nsamples1=0.0;
    if(error1!=0)
      nsamples1 = pow(rms1/error1,2);
    Double_t M2_1 = pow(rms1,2)*nsamples1;
    Double_t delta_mean = mean2-mean1;
    M2_1 += M2_2;
    M2_1 += nsamples1*nsamples2*pow(delta_mean,2)/(nsamples1+nsamples2);
    dest.mean += nsamples2*delta_mean/(nsamples1+nsamples2);
    nsamples1+=nsamples2;
    dest.rms = sqrt(M2_1/nsamples1);
    dest.error = dest.rms/sqrt(nsamples1);
  }
}

StatData TaStatBuilder::GetNullAverage(StatData in, StatData out){
  StatData fRetStatData;
  fRetStatData.mean = (in.mean + out.mean)/2.0;
  fRetStatData.error = sqrt(in.error*in.error+out.error*out.error)/2.0;
  return fRetStatData;
}

void TaStatBuilder::FillTree(TTree *fTree, TString prefix){

  TString leaflist = "mean/D:error:mean_local:error_local:mean_sum:error_sum:rms";
  auto iter_dev = fDeviceNameList.begin();
  while(iter_dev!=fDeviceNameList.end()){
    TString chname = *iter_dev;
    TBranch *fBranch_ptr = fTree->GetBranch(prefix+chname);
    if(fBranch_ptr==NULL)
      fBranch_ptr = fTree->Branch(prefix+chname,0,leaflist);
    fBranch_ptr->GetLeaf("mean")->SetAddress(&(fWeightedAverageMap[chname].mean));
    fBranch_ptr->GetLeaf("error")->SetAddress(&(fWeightedAverageMap[chname].error));
    fBranch_ptr->GetLeaf("mean_local")->SetAddress(&(fLocalAverageMap[chname].mean));
    fBranch_ptr->GetLeaf("error_local")->SetAddress(&(fLocalAverageMap[chname].error));
    fBranch_ptr->GetLeaf("mean_sum")->SetAddress(&(fCentralMomentMap[chname].mean));
    fBranch_ptr->GetLeaf("error_sum")->SetAddress(&(fCentralMomentMap[chname].error));
    fBranch_ptr->GetLeaf("rms")->SetAddress(&(fCentralMomentMap[chname].rms));

    iter_dev++;
  }
}

void TaStatBuilder::UpdateStatBuilderByIHWP(TaStatBuilder finput,
					    TString ihwp,Int_t sign){
  fStatBuilderByIHWP[ihwp].UpdateStatBuilder(finput);
  UpdateStatBuilder(finput,sign);
}

void TaStatBuilder::UpdateStatBuilder(TaStatBuilder finput ,Int_t sign){
  weighting_errorbar = (finput.fWeightedAverageMap["Adet"]).error;
  
  auto iter_dev = (finput.fDeviceNameList).begin();
  while(iter_dev != (finput.fDeviceNameList).end()){
    
    if( find(fDeviceNameList.begin(),fDeviceNameList.end(),*iter_dev)==fDeviceNameList.end())
      fDeviceNameList.push_back(*iter_dev);
    UpdateWeightedAverage(fWeightedAverageMap[*iter_dev],
			  finput.fWeightedAverageMap[*iter_dev],sign);
    UpdateLocalAverage(fLocalAverageMap[*iter_dev],
		       finput.fLocalAverageMap[*iter_dev],sign);
    UpdateCentralMoment(fCentralMomentMap[*iter_dev],
			finput.fCentralMomentMap[*iter_dev],sign);
    iter_dev++;
  }
}

TaStatBuilder TaStatBuilder::GetNullStatBuilder(){
  TaStatBuilder fNullStatBuilder;
  TaStatBuilder inStatBuilder = fStatBuilderByIHWP["IN"];
  TaStatBuilder outStatBuilder = fStatBuilderByIHWP["OUT"];

  auto iter_dev = fDeviceNameList.begin();
  while(iter_dev!=fDeviceNameList.end()){
    if(find(fNullStatBuilder.fDeviceNameList.begin(),
	    fNullStatBuilder.fDeviceNameList.end(),
	    *iter_dev) == fNullStatBuilder.fDeviceNameList.end())
      fNullStatBuilder.fDeviceNameList.push_back(*iter_dev);
	    
    fNullStatBuilder.fWeightedAverageMap[*iter_dev] = GetNullAverage(inStatBuilder.fWeightedAverageMap[*iter_dev],outStatBuilder.fWeightedAverageMap[*iter_dev]);
    fNullStatBuilder.fLocalAverageMap[*iter_dev] = GetNullAverage(inStatBuilder.fLocalAverageMap[*iter_dev],outStatBuilder.fLocalAverageMap[*iter_dev]);
    fNullStatBuilder.fCentralMomentMap[*iter_dev] = GetNullAverage(inStatBuilder.fCentralMomentMap[*iter_dev],outStatBuilder.fCentralMomentMap[*iter_dev]);
    iter_dev++;
  }
  return fNullStatBuilder;
}

void TaStatBuilder::ProcessNullAsym(TTree *fTree){
  TaStatBuilder fNullStatBuilder = GetNullStatBuilder();
  fNullStatBuilder.FillTree(fTree,"null_");
}

void TaStatBuilder::PullFitAllChannels(){
  auto iter_dev = fStatDataArrayMap.begin();
  while(iter_dev!=fStatDataArrayMap.end()){
    StatDataArray fStatDataArray  = (*iter_dev).second;
    vector<Double_t> x_val;
    vector<Double_t> y_val;
    vector<Double_t> y_err;
    Double_t fCounter=0;
    auto iter_data = fStatDataArray.begin();
    while(iter_data!=fStatDataArray.end()){
      y_val.push_back((*iter_data).mean);
      y_err.push_back((*iter_data).error);
      x_val.push_back(fCounter++);
      iter_data++;
    }
    
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);

    TCanvas c1("c1","c1",1100,600);
    c1.cd();
    c1.Clear();
    TString title = (*iter_dev).first;
    double ySep = 0.3;
    double xSep = 0.65;

    TPad p1("p1","",0,ySep,xSep,1);
    p1.Draw();
    TPad p2("p2","",0,0,xSep,ySep);
    p2.Draw();
    TPad p3("p3","",xSep,0,1,1);
    p3.Draw();
  
    p1.cd();
    p1.UseCurrentStyle();
    p1.SetBottomMargin(0);
    
    Int_t npt = x_val.size();
    Double_t *x_array = new Double_t[npt];
    Double_t *y_array = new Double_t[npt];
    Double_t *yerr_array = new Double_t[npt];
    for(int i=0;i<npt;i++){
      x_array[i]=i;
      y_array[i]=y_val[i];
      yerr_array[i]=y_err[i];
    }

    TF1 *f1 = new TF1("f1","pol0",-1e3,1e3);
    TF1 *fg = new TF1("fg","gaus",-1e3,1e3);
    TGraphErrors *tge = new TGraphErrors(npt,x_array,y_array,0,yerr_array);
    tge->Draw("AP");
    tge->SetMarkerStyle(20);
    tge->Fit("f1","Q");
    tge->SetTitle(title);

    Double_t fit_mean = f1->GetParameter(0);
    TH1F *htge = tge->GetHistogram();
    htge->GetXaxis()->Set(npt,-0.5,npt-0.5);
    for(int ibin=1;ibin<=npt;ibin++)
      htge->GetXaxis()->SetBinLabel(ibin,Form("%.1f",x_val[ibin-1]));
    TH1D hPull("hPull","",npt,-0.5,npt-0.5);
    TH1D hPull1D("hPull1D","1D pull distribution",30,-8,8);
  
    for(int i=0;i<npt;i++){
      double val = (y_array[i]-fit_mean)/yerr_array[i];
      int ibin = x_array[i]+1;
      hPull.SetBinContent(ibin,val);
      hPull.GetXaxis()->SetBinLabel(ibin,fAxisTitle[i]);
      hPull1D.Fill(val);
    }
    
    p2.cd();
    p2.SetTopMargin(0);
    p2.SetBottomMargin(0.3);
    hPull.GetYaxis()->SetLabelSize(0.1);
    hPull.GetXaxis()->SetLabelSize(0.1);
    hPull.SetFillColor(kGreen-3);
    hPull.SetStats(0);
    hPull.GetXaxis()->LabelsOption("v");
    hPull.DrawCopy("b");
    p2.SetGridx(1);
    p2.SetGridy(1);

  
    gStyle->SetOptFit(1);
    gStyle->SetOptStat("MRou");
    p3.cd();
    p3.UseCurrentStyle();
    fg->SetParameters(hPull1D.GetMaximum()*0.8,hPull1D.GetMean(),hPull1D.GetRMS());
    hPull1D.Fit(fg->GetName(),"Q");
    hPull1D.DrawCopy();
    fg->Draw("same");
  
    c1.SaveAs(title+"_test.pdf");

    iter_dev++;
  }

}

