#include "TaStatBuilder.hh"
ClassImp(TaStatBuilder)

TaStatBuilder::TaStatBuilder(){
  weighting_errorbar  = -1.0;
  kUseWeight = kFALSE;
}

void TaStatBuilder::UpdateWeightingError(StatData input){
  kUseWeight = kTRUE;
  weighting_errorbar = input.error;
}

void TaStatBuilder::UpdateStatData(TString chname,StatData input, Int_t sign){

  Double_t nsamples2 = 0.0;
  Double_t error2 = input.error;
  Double_t rms2 = input.rms;
  
  if(input.num_samples==0)
    nsamples2= pow(rms2/error2,2);
  else
    nsamples2 = input.num_samples;

  if(nsamples2>0 && nsamples2<=4500)
    return;
  
  if(fStatDataArrayMap.find(chname)==fStatDataArrayMap.end())
    fDeviceNameList.push_back(chname);
  
  if(fAverageMap.find(chname)==fAverageMap.end())
    fAverageMap[chname].Zero();

  UpdateStatData(fAverageMap[chname],input,sign);

  input.mean = sign*input.mean;
  input.sign = sign;
  if(kUseWeight)
    input.error = weighting_errorbar;
  fStatDataArrayMap[chname].push_back(input);
  fLabelMap[chname].push_back(fLabel_tmp);
}

void TaStatBuilder::UpdateStatData(StatData &dest,
				   StatData input,Int_t sign){
  if(input.error<=0 || input.error!=input.error)
    return;

  Double_t nsamples2 = 0.0;
  Double_t error2 = input.error;
  Double_t rms2 = input.rms;
  if(input.num_samples==0)
    nsamples2= pow(rms2/error2,2);
  else
    nsamples2 = input.num_samples;

  if(nsamples2>0 && nsamples2<=4500)
    return;
  
  if(dest.error==0){
    dest.mean = sign*input.mean;
    if(kUseWeight){
      dest.error = weighting_errorbar;
      dest.error_scaled = pow(input.error,2) * pow(weighting_errorbar,-4);
    }else{
      dest.error = input.error;
      dest.error_scaled = input.error_scaled;
    }
  }else{
    Double_t mean1= dest.mean;
    Double_t weight1= pow(dest.error,-2);
    Double_t mean2 = sign*input.mean;
    Double_t weight2;
    if(kUseWeight){
      weight2 = pow(weighting_errorbar,-2);
      dest.error_scaled += (pow(input.error,2)*pow(weighting_errorbar,-4));
    }else{
      weight2 = pow(input.error,-2);
      dest.error_scaled +=input.error_scaled;
    }
    Double_t weight_sum = weight1+weight2;
    dest.mean = (mean1*weight1 + mean2*weight2)/weight_sum;
    dest.error =  sqrt(1.0/weight_sum);
  }
  dest.sign = sign;
  UpdateCentralMoment(dest,input,sign);
}

// void TaStatBuilder::UpdateCentralMoment(TString chname,StatData input, Int_t sign){
//   if(fStatDataArrayMap.find(chname)==fStatDataArrayMap.end())
//     fDeviceNameList.push_back(chname);
//   if(input.error>0)
//     fStatDataArrayMap[chname].push_back(input);

//   if(fAverageMap.find(chname)==fAverageMap.end())
//      fAverageMap[chname].Zero();
  
//   UpdateCentralMoment(fAverageMap[chname],input,sign);
// }

void TaStatBuilder::UpdateCentralMoment(StatData &dest,
					StatData input,Int_t sign){

  // FIXME sign not in used
  if(input.error<=0 || input.error!=input.error)
    return;

  if(input.mean_sum==0)
    input.mean_sum = input.mean;
  
  Double_t mean2 = sign*input.mean_sum;
  Double_t error2 = input.error;
  Double_t rms2 = input.rms;
  
  Double_t nsamples2 = 0.0;
  if(error2!=0 && input.num_samples==0)
    nsamples2= pow(rms2/error2,2);
  else
    nsamples2 = input.num_samples;

  if(nsamples2>0 && nsamples2<=4500)
    return;
  
  Double_t M2_2 = pow(rms2,2)*nsamples2;

  if(nsamples2!=0){
    Double_t mean1 = dest.mean_sum;
    Double_t rms1 = dest.rms;
    Double_t nsamples1=dest.num_samples;
    Double_t M2_1 = pow(rms1,2)*nsamples1;
    Double_t delta_mean = mean2-mean1;
    M2_1 += M2_2;
    M2_1 += nsamples1*nsamples2*pow(delta_mean,2)/(nsamples1+nsamples2);
    dest.mean_sum += nsamples2*delta_mean/(nsamples1+nsamples2);
    nsamples1+=nsamples2;
    dest.num_samples = nsamples1;
    dest.rms = sqrt(M2_1/nsamples1);
  }
}

StatData TaStatBuilder::GetNullAverage(StatData in1, StatData in2){
  StatData fRetStatData;
  // cout << in1.sign << " \t " << in2.sign << endl;
  fRetStatData.mean = (in1.mean + in2.mean)/2.0;
  fRetStatData.error = sqrt(in1.error*in1.error+in2.error*in2.error)/2.0;
  return fRetStatData;
}

void TaStatBuilder::FillTree(TTree *fTree, TString prefix){

  TString leaflist = "mean/D:error:rms:nsamp:chi2:ndf";
  auto iter_dev = fDeviceNameList.begin();
  while(iter_dev!=fDeviceNameList.end()){
    TString chname = *iter_dev;
    TBranch *fBranch_ptr = fTree->GetBranch(prefix+chname);
    if(fBranch_ptr==NULL)
      fBranch_ptr = fTree->Branch(prefix+chname,0,leaflist);
    fBranch_ptr->GetLeaf("mean")->SetAddress(&(fAverageMap[chname].mean));
    fBranch_ptr->GetLeaf("error")->SetAddress(&(fAverageMap[chname].error));
    fBranch_ptr->GetLeaf("chi2")->SetAddress(&(fAverageMap[chname].chi2));
    fBranch_ptr->GetLeaf("ndf")->SetAddress(&(fAverageMap[chname].ndf));
    fBranch_ptr->GetLeaf("rms")->SetAddress(&(fAverageMap[chname].rms));
    fBranch_ptr->GetLeaf("nsamp")->SetAddress(&(fAverageMap[chname].num_samples));
    iter_dev++;
  }
}

void TaStatBuilder::UpdateStatBuilderByIHWP(TaStatBuilder* finput,
					    TString ihwp,Int_t sign){

  if( fStatBuilderByIHWP.find(ihwp)==fStatBuilderByIHWP.end())
    fStatBuilderByIHWP[ihwp] = new TaStatBuilder();
  fStatBuilderByIHWP[ihwp]->UpdateStatBuilder(finput,sign);
  UpdateStatBuilder(finput,sign);
}

void TaStatBuilder::UpdateStatBuilder(TaStatBuilder* finput ,Int_t sign){
  weighting_errorbar = (finput->fAverageMap["Adet"]).error;
  // FIXME : Error if Adet is not found in the fAverageMap
  auto iter_dev = (finput->fDeviceNameList).begin();
  while(iter_dev != (finput->fDeviceNameList).end()){
    UpdateStatData(*iter_dev,
		   finput->fAverageMap[*iter_dev],sign);
    iter_dev++;
  }
  if(finput->HasStatBuilderByIHWP()){
    if( fStatBuilderByIHWP.find("IN")==fStatBuilderByIHWP.end())
      fStatBuilderByIHWP["IN"] = new TaStatBuilder();
    fStatBuilderByIHWP["IN"]->UpdateStatBuilder(finput->GetStatBuilderByIHWP("IN"));
    
    if( fStatBuilderByIHWP.find("OUT")==fStatBuilderByIHWP.end())
      fStatBuilderByIHWP["OUT"] = new TaStatBuilder();
    fStatBuilderByIHWP["OUT"]->UpdateStatBuilder(finput->GetStatBuilderByIHWP("OUT"));
  }
  
  fStatBuilderArray.push_back(finput);
}

TaStatBuilder* TaStatBuilder::GetNullStatBuilder(){
  TaStatBuilder* fNullStatBuilder= new TaStatBuilder();
  if(fStatBuilderByIHWP["IN"]==NULL)
    fStatBuilderByIHWP["IN"] = new TaStatBuilder();
  if(fStatBuilderByIHWP["OUT"]==NULL)
    fStatBuilderByIHWP["OUT"] = new TaStatBuilder();
  TaStatBuilder* inStatBuilder = fStatBuilderByIHWP["IN"];
  TaStatBuilder* outStatBuilder = fStatBuilderByIHWP["OUT"];

  // No ... I want to re-assemble in and out statbuilder here

  if(inStatBuilder->fAverageMap["Adet"].error> outStatBuilder->fAverageMap["Adet"].error){

    TaStatBuilder* fStatBuilderCandidate = new TaStatBuilder();
    vector<TaStatBuilder*> fStatBuilderArray = outStatBuilder->GetStatBuilderArray();
    auto iter_sb = fStatBuilderArray.begin();
    while(iter_sb!=fStatBuilderArray.end()){
      if(fStatBuilderCandidate->fAverageMap["Adet"].error==0){
	fStatBuilderCandidate->UpdateStatBuilder(*iter_sb);
	iter_sb++;
	continue;
      }
      Double_t dist_old = distance(fStatBuilderCandidate->fAverageMap["Adet"].error,
				   inStatBuilder->fAverageMap["Adet"].error);
      Double_t errorbar = 1.0/sqrt( 1.0/pow(fStatBuilderCandidate->fAverageMap["Adet"].error,2)+
				    1.0/pow((*iter_sb)->fAverageMap["Adet"].error,2) );
      Double_t dist_new = distance(errorbar,
				   inStatBuilder->fAverageMap["Adet"].error);

      if( dist_new < dist_old)
	fStatBuilderCandidate->UpdateStatBuilder(*iter_sb);
      iter_sb++;
    }
    fNullStatBuilder->UpdateStatBuilderByIHWP(fStatBuilderCandidate,"OUT");
    fNullStatBuilder->UpdateStatBuilderByIHWP(inStatBuilder,"IN");
    // Average stat data get updated at this point but will be refresh afterwards.
    
  }else{
    TaStatBuilder* fStatBuilderCandidate = new TaStatBuilder();
    vector<TaStatBuilder*> fStatBuilderArray = inStatBuilder->GetStatBuilderArray();
    auto iter_sb = fStatBuilderArray.begin();
    while(iter_sb!=fStatBuilderArray.end()){
      if(fStatBuilderCandidate->fAverageMap["Adet"].error==0){
	fStatBuilderCandidate->UpdateStatBuilder(*iter_sb);
	iter_sb++;
	continue;
      }
      Double_t dist_old = distance(fStatBuilderCandidate->fAverageMap["Adet"].error,
				   outStatBuilder->fAverageMap["Adet"].error);
      Double_t errorbar = 1.0/sqrt( 1.0/pow(fStatBuilderCandidate->fAverageMap["Adet"].error,2)+
				    1.0/pow((*iter_sb)->fAverageMap["Adet"].error,2) );
      Double_t dist_new = distance(errorbar,
				   outStatBuilder->fAverageMap["Adet"].error);

      if( dist_new < dist_old)
	fStatBuilderCandidate->UpdateStatBuilder(*iter_sb);
      iter_sb++;
    }
    fNullStatBuilder->UpdateStatBuilderByIHWP(fStatBuilderCandidate,"IN");
    fNullStatBuilder->UpdateStatBuilderByIHWP(outStatBuilder,"OUT");
  }
  
  
  auto iter_dev = fDeviceNameList.begin();
  while(iter_dev!=fDeviceNameList.end()){
    if(find(fNullStatBuilder->fDeviceNameList.begin(),
	    fNullStatBuilder->fDeviceNameList.end(),
	    *iter_dev) == fNullStatBuilder->fDeviceNameList.end())
      fNullStatBuilder->fDeviceNameList.push_back(*iter_dev);
    // FIXME : should be aborted when channel map mismatched
    fNullStatBuilder->fAverageMap[*iter_dev] = GetNullAverage(fNullStatBuilder->GetStatBuilderByIHWP("IN")->fAverageMap[*iter_dev],fNullStatBuilder->GetStatBuilderByIHWP("OUT")->fAverageMap[*iter_dev]);

    iter_dev++;
  }
  return fNullStatBuilder;
}

void TaStatBuilder::ProcessNullAsym(TTree *fTree){
  TaStatBuilder* fNullStatBuilder = GetNullStatBuilder();
  fNullStatBuilder->FillTree(fTree,"null_");
}

void TaStatBuilder::PullFitAllChannels(TString filename){
  if(filename!="")
    plot_filename  = filename;
  else
    filename = plot_filename;
  TCanvas c1("c1","c1",1100,600);
  TCanvas c2("c2","c2",1100,600);
  c1.cd();
  c1.Print(filename+"[");
  auto iter_dev = fDeviceNameList.begin();
  while(iter_dev!=fDeviceNameList.end()){
    c1.Clear();
    StatDataArray fStatDataArray  = fStatDataArrayMap[*iter_dev];
    vector<Double_t> x_val;
    vector<Double_t> y_val;
    vector<Double_t> y_err;
    vector<Double_t> y_rms;
    Double_t fCounter=0;
    Double_t rescale = 1.0;
    TString unit;
    TString rms_unit;
    if( (*iter_dev).Contains("asym")){
      rescale = 1e9;
      unit = "(ppb)";
      rms_unit = "(ppm)";
    }
    if( (*iter_dev).Contains("Adet")){
      rescale = 1e9;
      unit = "(ppb)";
      rms_unit = "(ppm)";
    }
    if( (*iter_dev).Contains("Aq")){
      rescale = 1e9;
      unit = "(ppb)";
      rms_unit = "(ppm)";
    }
    if( (*iter_dev).Contains("diff_bpm")){
      rescale = 1e6;
      unit = "(nm)";
      rms_unit = "(um)";
    }
    if( (*iter_dev).Contains("diff_evMon")){
      rescale = 1e6;
      unit = "(nm)";
      rms_unit = "(um)";
    }
    
    if( (*iter_dev).Contains("diff") &&
	(*iter_dev).Contains("battery")	){
      rescale = 1e9;
      unit = "(nV)";
      rms_unit = "(uV)";
    }
    
    if( (*iter_dev).Contains("diff") &&
	(*iter_dev).Contains("ch_battery")){
      rescale = 76e-6*1e9;
      unit = "(nV)";
      rms_unit = "(uV)";
    }

    auto iter_data = fStatDataArray.begin();
    while(iter_data!=fStatDataArray.end()){
      if((*iter_data).error>0.0){
	y_val.push_back((*iter_data).mean*rescale);
	y_err.push_back((*iter_data).error*rescale);
	y_rms.push_back((*iter_data).rms*rescale/1e3);
	x_val.push_back(fCounter);
      }
      iter_data++;
      fCounter++;
    }
    if(x_val.size()==0){iter_dev++; continue;};
    
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);

    c1.Clear("D");
    TString title = *iter_dev +" " + unit;;
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
    Double_t *rms_array = new Double_t[npt];
    Double_t *yerr_array = new Double_t[npt];
    for(int i=0;i<npt;i++){
      x_array[i]=i;
      y_array[i]=y_val[i];
      rms_array[i]=y_rms[i];
      yerr_array[i]=y_err[i];
    }

    TF1 *f1 = new TF1("f1","pol0",-1e3,1e3);
    TF1 *fg = new TF1("fg","gaus",-1e3,1e3);
    TGraphErrors tge(npt,x_array,y_array,0,yerr_array);
    tge.Draw("AP");
    tge.SetMarkerStyle(20);
    tge.Fit("f1","Q");
    tge.SetTitle(title);

    fAverageMap[(*iter_dev)].SetChi2NDF(f1->GetChisquare(),
					f1->GetNDF());
    Double_t fit_mean = f1->GetParameter(0);
    TH1F *htge = tge.GetHistogram();
    htge->GetXaxis()->Set(npt,-0.5,npt-0.5);
    for(int ibin=1;ibin<=npt;ibin++)
      htge->GetXaxis()->SetBinLabel(ibin,Form("%.1f",x_val[ibin-1]));
    TH1D hPull("hPull","",npt,-0.5,npt-0.5);
    TH1D hPull1D("hPull1D","1D pull distribution",30,-8,8);

    vector<TString> fAxisTitle = fLabelMap[*iter_dev];
    for(int i=0;i<npt;i++){
      double val = (y_array[i]-fit_mean)/yerr_array[i];
      int ibin = x_array[i]+1;
      hPull.SetBinContent(ibin,val);
      hPull.GetXaxis()->SetBinLabel(ibin,fAxisTitle[x_val[i]]);
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
    
    c1.Print(filename);

    c2.cd();
    c2.Clear("D");
    TGraph g_rms(npt,x_array,rms_array);

    g_rms.SetTitle( *iter_dev+" RMS " +rms_unit);
    TH1F *htrms = g_rms.GetHistogram();
    htrms->GetXaxis()->Set(npt,-0.5,npt-0.5);
    htrms->GetYaxis()->SetTitle("RMS "+rms_unit);
    for(int ibin=1;ibin<=npt;ibin++)
      htrms->GetXaxis()->SetBinLabel(ibin,fAxisTitle[x_val[ibin-1]]);
    g_rms.SetMarkerStyle(20);
    g_rms.Draw("APL");
    c2.Print(filename);
    iter_dev++;
  }
  c1.Print(filename+"]");
  cout << " -- Done printing plots to " << filename << endl;
}

void TaStatBuilder::ReportLogByIHWP(TaResult &log){
  auto iter_device = fDeviceNameList.begin();
  while(iter_device!=fDeviceNameList.end()){
    TString unit,rms_unit;
    Double_t rescale  = LoadScaleFactor(*iter_device,unit, rms_unit);
    vector<TString> header = { " " , "Mean"+unit,"Error"+unit, "RMS"+rms_unit, "Chi2/NDF"};
    header.insert(header.begin(), *iter_device);
    log.AddHeader(header);
    vector<TString> fLabel = GetStatDataLabelByName( *iter_device);
    StatDataArray fStatDataArray = GetStatDataArrayByName(*iter_device);
    StatDataArray fStatDataArray_in =GetStatDataArrayByName( *iter_device,"IN");
    StatDataArray fStatDataArray_out =GetStatDataArrayByName( *iter_device,"OUT");
    StatData fGrandAverage  = fAverageMap[*iter_device];
    Int_t nData = fLabel.size();
    for(int idata=0;idata<nData;idata++){
      log.AddLine();
      log.AddStringEntry(" ");
      log.AddStringEntry("IN");
      log.AddFloatEntry( fStatDataArray_in[idata].mean*rescale);
      log.AddFloatEntry( fStatDataArray_in[idata].error*rescale);
      log.AddFloatEntry( fStatDataArray_in[idata].rms*rescale/1e3);
      log.AddChi2NDF(fStatDataArray_in[idata].chi2, fStatDataArray_in[idata].ndf);
      log.AddLine();
      log.AddStringEntry(" ");
      log.AddStringEntry("OUT");
      log.AddFloatEntry( fStatDataArray_out[idata].mean*rescale);
      log.AddFloatEntry( fStatDataArray_out[idata].error*rescale);
      log.AddFloatEntry( fStatDataArray_out[idata].rms*rescale/1e3);
      log.AddChi2NDF(fStatDataArray_out[idata].chi2, fStatDataArray_out[idata].ndf);
      log.AddLine();
      log.AddStringEntry(" ");
      log.AddStringEntry( fLabel[idata] );
      log.AddFloatEntry( fStatDataArray[idata].mean*rescale);
      log.AddFloatEntry( fStatDataArray[idata].error*rescale);
      log.AddFloatEntry( fStatDataArray[idata].rms*rescale/1e3);
      log.AddChi2NDF(fStatDataArray[idata].chi2, fStatDataArray[idata].ndf);
      log.InsertHorizontalLine();
    }
    log.AddLine();
    log.AddStringEntry(" ");
    log.AddStringEntry("Grand Avg.");
    log.AddFloatEntry( fGrandAverage.mean*rescale);
    log.AddFloatEntry( fGrandAverage.error*rescale);
    log.AddFloatEntry( fGrandAverage.rms*rescale/1e3);
    log.AddChi2NDF(fGrandAverage.chi2, fGrandAverage.ndf);
    
    log.InsertHorizontalLine();
    iter_device++;
  }
  log.Print();
  log.Close();
}

void TaStatBuilder::ReportLog(TaResult &log){
  auto iter_device = fDeviceNameList.begin();
  while(iter_device!=fDeviceNameList.end()){
    TString unit,rms_unit;
    Double_t rescale  = LoadScaleFactor(*iter_device,unit, rms_unit);
    vector<TString> header = { " " , "Mean"+unit,"Error"+unit, "RMS"+rms_unit, "Chi2/NDF"};
    header.insert(header.begin(), *iter_device);
    log.AddHeader(header);
    vector<TString> fLabel = GetStatDataLabelByName( *iter_device);
    StatDataArray fStatDataArray =GetStatDataArrayByName( *iter_device);
    StatData fGrandAverage  = fAverageMap[*iter_device];
    Int_t nData = fLabel.size();
    for(int idata=0;idata<nData;idata++){
      log.AddLine();
      log.AddStringEntry(" ");
      log.AddStringEntry( fLabel[idata] );
      log.AddFloatEntry( fStatDataArray[idata].mean*rescale);
      log.AddFloatEntry( fStatDataArray[idata].error*rescale);
      log.AddFloatEntry( fStatDataArray[idata].rms*rescale/1e3);
      log.AddChi2NDF(fStatDataArray[idata].chi2, fStatDataArray[idata].ndf);
    }
    log.AddLine();
    log.InsertHorizontalLine();
    log.AddStringEntry(" ");
    log.AddStringEntry("Grand Avg.");
    log.AddFloatEntry( fGrandAverage.mean*rescale);
    log.AddFloatEntry( fGrandAverage.error*rescale);
    log.AddFloatEntry( fGrandAverage.rms*rescale/1e3);
    log.AddChi2NDF(fGrandAverage.chi2, fGrandAverage.ndf);

    log.InsertHorizontalLine();
    iter_device++;
  }
  log.Print();
  log.Close();
}

Double_t TaStatBuilder::LoadScaleFactor(TString device_name,
					TString &unit, TString &rms_unit){
  Double_t rescale = 1.0;
  if( device_name.Contains("asym")){
    rescale = 1e9;
    unit = "(ppb)";
    rms_unit = "(ppm)";
  }
  if( device_name.Contains("Adet")){
    rescale = 1e9;
    unit = "(ppb)";
    rms_unit = "(ppm)";
  }
  if( device_name.Contains("Aq")){
    rescale = 1e9;
    unit = "(ppb)";
    rms_unit = "(ppm)";
  }
  if( device_name.Contains("diff_bpm")){
    rescale = 1e6;
    unit = "(nm)";
    rms_unit = "(um)";
  }
  if( device_name.Contains("diff_evMon")){
    rescale = 1e6;
    unit = "(nm)";
    rms_unit = "(um)";
  }
    
  if( device_name.Contains("diff") &&
      device_name.Contains("battery")	){
    rescale = 1e9;
    unit = "(nV)";
    rms_unit = "(uV)";
  }
    
  if( device_name.Contains("diff") &&
      device_name.Contains("ch_battery")){
    rescale = 76e-6*1e9;
    unit = "(nV)";
    rms_unit = "(uV)";
  }
  return rescale ;
}


void TaStatBuilder::RescaleErrorBar(){
  auto iter_dev = fDeviceNameList.begin();

  while(iter_dev!=fDeviceNameList.end()){
    StatData fAveraged = fAverageMap[*iter_dev];
    Double_t weighted_error = fAveraged.error;
    Double_t scaled_error_square_sum =  fAveraged.error_scaled;
    Double_t scaled_factor =sqrt( scaled_error_square_sum)* weighted_error;
    Double_t scaled_error = scaled_factor * weighted_error;
    fAverageMap[*iter_dev].error = scaled_error;
    Int_t nData = fStatDataArrayMap[*iter_dev].size();
    for(int id=0;id<nData;id++)
      fStatDataArrayMap[*iter_dev][id].error *= scaled_factor;
    iter_dev++;
  }
  
  if(HasStatBuilderByIHWP()){
    fStatBuilderByIHWP["IN"]->RescaleErrorBarBy(this);
    fStatBuilderByIHWP["OUT"]->RescaleErrorBarBy(this);
  }

  auto iter_fsb = fStatBuilderArray.begin();
  while(iter_fsb!=fStatBuilderArray.end()){
    (*iter_fsb)->RescaleErrorBarBy(this);
    iter_fsb++;
  }

  iter_dev = fDeviceNameList.begin();

  while(iter_dev!=fDeviceNameList.end()){
    Int_t nData = fStatDataArrayMap[*iter_dev].size();
    for(int id=0;id<nData;id++){
      fStatDataArrayMap[*iter_dev][id].chi2 = fStatBuilderArray[id]->fAverageMap[*iter_dev].chi2;
      fStatDataArrayMap[*iter_dev][id].ndf = fStatBuilderArray[id]->fAverageMap[*iter_dev].ndf;
    }
    iter_dev++;
  }
}


void TaStatBuilder::RescaleErrorBarBy( TaStatBuilder* fGrandBuilder){
  auto iter_dev = fDeviceNameList.begin();
  while(iter_dev!=fDeviceNameList.end()){
    StatData fAveraged = fGrandBuilder->fAverageMap[*iter_dev];
    Double_t rescaled_error = fAveraged.error;
    Double_t scaled_error_square_sum =  sqrt(fAveraged.error_scaled);
    Double_t weighted_error = sqrt( rescaled_error / scaled_error_square_sum);
    Double_t scaled_factor = rescaled_error / weighted_error;
    fAverageMap[*iter_dev].error *= scaled_factor;
    Int_t nData = fStatDataArrayMap[*iter_dev].size();
    for(int id=0;id<nData;id++)
      fStatDataArrayMap[*iter_dev][id].error*=scaled_factor;
    iter_dev++;
  }
  PullFitAllChannels();
  
  auto iter_fsb = fStatBuilderArray.begin();
  while(iter_fsb!=fStatBuilderArray.end()){
    (*iter_fsb)->RescaleErrorBarBy(fGrandBuilder);
    iter_fsb++;
  }
  
  if(HasStatBuilderByIHWP()){
    fStatBuilderByIHWP["IN"]->RescaleErrorBarBy(fGrandBuilder);
    fStatBuilderByIHWP["OUT"]->RescaleErrorBarBy(fGrandBuilder);
  }

}
