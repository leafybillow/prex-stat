void ReportDetectorLog(map<SLUG_ARM,TaStatBuilder> fSBMap,TaResult& aLog);
void ReportBeamLineLog(map<SLUG_ARM,TaStatBuilder> fSBMap,TaResult& aLog);

void ReportDetectorLog(map<Int_t,TaStatBuilder> fSBMap,TaResult& aLog);
void ReportBeamLineLog(map<Int_t,TaStatBuilder> fSBMap,TaResult& aLog);

void ReportDetectorLog(TaStatBuilder fSBMap,TaResult& aLog);
void ReportDetectorLogByIHWP(TaStatBuilder fSBMap,TaResult& aLog);
void ReportBeamLineLog(TaStatBuilder fSBMap,TaResult& aLog);

void ReportDetectorLog(map<SLUG_ARM,TaStatBuilder> fSBMap,
		       TaResult& aLog){
  vector<TString> header_slug={"#","Mean(ppb)","Error(ppb)","RMS(ppm)","chi2/ndf"};
  aLog.AddHeader(header_slug);
  Int_t icount=0;
  auto iter = fSBMap.begin();
  while(iter!=fSBMap.end()){
    aLog.AddLine();
    
    TString label = Form("%d", ((*iter).first).first);
    Int_t fArmSlug = ((*iter).first).second;
    if(fArmSlug==1)
      label+="R";
    else if(fArmSlug==2)
      label+="L";
    
    aLog.AddStringEntry(label);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Adet"].mean*1e9);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Adet"].error*1e9);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Adet"].rms*1e6);
    aLog.AddChi2NDF(((*iter).second).fAverageMap["Adet"].chi2,
		    ((*iter).second).fAverageMap["Adet"].ndf);
    iter++;
  }
  
  aLog.InsertHorizontalLine();
  aLog.Print();
  aLog.Close();
}

void ReportBeamLineLog(map<SLUG_ARM,TaStatBuilder> fSBMap,
		       TaResult& aLog){
  
  vector<TString> headers={"#",
			   "Aq(ppb)","RMS(ppm)",
			   "D4aX(nm)","RMS(um)",
			   "D4eX(ppb)","RMS(ppm)",
			   "D4aY(nm)","RMS(um)",
			   "D4eY(nm)","RMS(um)",
			   "DXE(nm)","RMS(um)"};

  vector<TString> bpmlist={"diff_bpm4aX","diff_bpm4eX",
			   "diff_bpm4aY","diff_bpm4eY",
  			   "diff_bpmE"};

  aLog.AddHeader(headers);
  auto iter = fSBMap.begin();
  while(iter!=fSBMap.end()){
    aLog.AddLine();
    TString label = Form("%d", ((*iter).first).first);
    Int_t fArmSlug = ((*iter).first).second;
    if(fArmSlug==1)
      label+="R";
    else if(fArmSlug==2)
      label+="L";
    aLog.AddStringEntry(label);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Aq"].mean*1e9);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Aq"].rms*1e6);

    auto iter_bpm = bpmlist.begin();
    while(iter_bpm!=bpmlist.end()){
      aLog.AddFloatEntry(((*iter).second).fAverageMap[*iter_bpm].mean*1e6);
      aLog.AddFloatEntry(((*iter).second).fAverageMap[*iter_bpm].rms*1e3);
      iter_bpm++;
    }
    iter++;
  }
  aLog.InsertHorizontalLine();
  aLog.Print();
  aLog.Close();
}

void ReportDetectorLog(map<Int_t,TaStatBuilder> fSBMap,
		       TaResult& aLog){
  vector<TString> header_slug={"#","Mean(ppb)","Error(ppb)","RMS(ppm)","chi2/ndf"};
  aLog.AddHeader(header_slug);
  Int_t icount=0;
  auto iter = fSBMap.begin();
  while(iter!=fSBMap.end()){
    aLog.AddLine();
    TString label = Form("%d", (*iter).first);
    aLog.AddStringEntry(label);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Adet"].mean*1e9);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Adet"].error*1e9);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Adet"].rms*1e6);
    aLog.AddChi2NDF(((*iter).second).fAverageMap["Adet"].chi2,
		    ((*iter).second).fAverageMap["Adet"].ndf);
    iter++;
  }
  
  aLog.InsertHorizontalLine();
  aLog.Print();
  aLog.Close();
}

void ReportBeamLineLog(map<Int_t,TaStatBuilder> fSBMap,
		       TaResult& aLog){
  
  vector<TString> headers={"#",
			   "Aq(ppb)","RMS(ppm)",
			   "D4aX(nm)","RMS(um)",
			   "D4eX(nm)","RMS(um)",
			   "D4aY(nm)","RMS(um)",
			   "D4eY(nm)","RMS(um)",
			   "DXE(nm)","RMS(um)"};

  vector<TString> bpmlist={"diff_bpm4aX","diff_bpm4eX",
			   "diff_bpm4aY","diff_bpm4eY",
  			   "diff_bpmE"};

  aLog.AddHeader(headers);
  auto iter = fSBMap.begin();
  while(iter!=fSBMap.end()){
    aLog.AddLine();
    TString label = Form("%d", (*iter).first);
    aLog.AddStringEntry(label);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Aq"].mean*1e9);
    aLog.AddFloatEntry(((*iter).second).fAverageMap["Aq"].rms*1e6);

    auto iter_bpm = bpmlist.begin();
    while(iter_bpm!=bpmlist.end()){
      aLog.AddFloatEntry(((*iter).second).fAverageMap[*iter_bpm].mean*1e6);
      aLog.AddFloatEntry(((*iter).second).fAverageMap[*iter_bpm].rms*1e3);
      iter_bpm++;
    }
    iter++;
  }
  aLog.InsertHorizontalLine();
  aLog.Print();
  aLog.Close();
}

void ReportDetectorLog(TaStatBuilder fStatBuilder,
		       TaResult& aLog){
  
  vector<TString> header_slug={"#","Mean(ppb)","Error(ppb)","RMS(ppm)","chi2/ndf"};
  aLog.AddHeader(header_slug);
  vector<TString> fLabel = fStatBuilder.GetStatDataLabelByName("Adet");
  StatDataArray fStatDataArray = fStatBuilder.GetStatDataArrayByName("Adet");
  Int_t nData = fStatDataArray.size();
  for(int i=0;i<nData;i++){
    aLog.AddLine();
    aLog.AddStringEntry(fLabel[i]);
    aLog.AddFloatEntry(fStatDataArray[i].mean*1e9);
    aLog.AddFloatEntry(fStatDataArray[i].error*1e9);
    aLog.AddFloatEntry(fStatDataArray[i].rms*1e6);
    aLog.AddChi2NDF(fStatDataArray[i].chi2,
		    fStatDataArray[i].ndf);

  }
  
  aLog.InsertHorizontalLine();
  aLog.AddStringEntry("Average");
  aLog.AddFloatEntry(fStatBuilder.fAverageMap["Adet"].mean*1e9);
  aLog.AddFloatEntry(fStatBuilder.fAverageMap["Adet"].error*1e9);
  aLog.AddFloatEntry(fStatBuilder.fAverageMap["Adet"].rms*1e6);
  aLog.AddChi2NDF(fStatBuilder.fAverageMap["Adet"].chi2,
		  fStatBuilder.fAverageMap["Adet"].ndf);
  
  aLog.InsertHorizontalLine();
  aLog.Print();
  aLog.Close();
}
void ReportDetectorLogByIHWP(TaStatBuilder fStatBuilder,
		       TaResult& aLog){
  
  vector<TString> header_slug={"#","Mean(ppb)","Error(ppb)","RMS(ppm)","chi2/ndf"};
  aLog.AddHeader(header_slug);
  vector<TString> fLabel = fStatBuilder.GetStatDataLabelByName("Adet");
  StatDataArray fStatDataArray = fStatBuilder.GetStatDataArrayByName("Adet");
  StatDataArray fStatDataArray_in = fStatBuilder.GetStatDataArrayByName("Adet","IN");
  StatDataArray fStatDataArray_out = fStatBuilder.GetStatDataArrayByName("Adet","OUT");
  Int_t nData = fStatDataArray.size();
  for(int i=0;i<nData;i++){
    aLog.AddLine();
    aLog.AddStringEntry("IN");
    aLog.AddFloatEntry(fStatDataArray_in[i].mean*1e9);
    aLog.AddFloatEntry(fStatDataArray_in[i].error*1e9);
    aLog.AddFloatEntry(fStatDataArray_in[i].rms*1e6);
    aLog.AddChi2NDF(fStatDataArray_in[i].chi2,
		    fStatDataArray_in[i].ndf);
    aLog.AddLine();
    aLog.AddStringEntry("OUT");
    aLog.AddFloatEntry(fStatDataArray_out[i].mean*1e9);
    aLog.AddFloatEntry(fStatDataArray_out[i].error*1e9);
    aLog.AddFloatEntry(fStatDataArray_out[i].rms*1e6);
    aLog.AddChi2NDF(fStatDataArray_out[i].chi2,
		    fStatDataArray_out[i].ndf);
    aLog.InsertHorizontalLine();
    aLog.AddLine();
    aLog.AddStringEntry(fLabel[i]+" NULL");
    aLog.AddFloatEntry(fStatDataArray[i].mean*1e9);
    aLog.AddFloatEntry(fStatDataArray[i].error*1e9);
    aLog.AddFloatEntry(fStatDataArray[i].rms*1e6);
    aLog.AddChi2NDF(fStatDataArray[i].chi2,
		    fStatDataArray[i].ndf);
    aLog.InsertHorizontalLine();
  }
  
  aLog.InsertHorizontalLine();
  aLog.AddStringEntry("Average");
  aLog.AddFloatEntry(fStatBuilder.fAverageMap["Adet"].mean*1e9);
  aLog.AddFloatEntry(fStatBuilder.fAverageMap["Adet"].error*1e9);
  aLog.AddFloatEntry(fStatBuilder.fAverageMap["Adet"].rms*1e6);
  aLog.AddChi2NDF(fStatBuilder.fAverageMap["Adet"].chi2,
		  fStatBuilder.fAverageMap["Adet"].ndf);
  
  aLog.InsertHorizontalLine();
  aLog.Print();
  aLog.Close();
}

void ReportBeamLineLog(TaStatBuilder fStatBuilder,TaResult& aLog){
  vector<TString> headers={"#",
			   "Aq(ppb)","RMS(ppm)",
			   "D4aX(nm)","RMS(um)",
			   "D4eX(nm)","RMS(um)",
			   "D4aY(nm)","RMS(um)",
			   "D4eY(nm)","RMS(um)",
			   "DXE(nm)","RMS(um)"};

  vector<TString> bpmlist={"diff_bpm4aX","diff_bpm4eX",
			   "diff_bpm4aY","diff_bpm4eY",
  			   "diff_bpmE"};

  aLog.AddHeader(headers);
  vector<TString> fLabel = fStatBuilder.GetStatDataLabelByName("Adet");
  map<TString, StatDataArray> fStatArrayMap;
  auto iter_bpm = bpmlist.begin();
  while(iter_bpm!=bpmlist.end()){
    fStatArrayMap[*iter_bpm] = fStatBuilder.GetStatDataArrayByName(*iter_bpm);
    iter_bpm++;
  }
  fStatArrayMap["Aq"] = fStatBuilder.GetStatDataArrayByName("Aq");
  Int_t ndata = fLabel.size();
  for(int i=0;i<ndata;i++){
    aLog.AddLine();
    aLog.AddStringEntry(fLabel[i]);
    aLog.AddFloatEntry(fStatArrayMap["Aq"][i].mean*1e9);
    aLog.AddFloatEntry(fStatArrayMap["Aq"][i].rms*1e6);
    auto iter_bpm = bpmlist.begin();
    while(iter_bpm!=bpmlist.end()){
      aLog.AddFloatEntry(fStatArrayMap[*iter_bpm][i].mean*1e6);
      aLog.AddFloatEntry(fStatArrayMap[*iter_bpm][i].rms*1e3);
      iter_bpm++;
    }
  }
  aLog.InsertHorizontalLine();
  aLog.AddStringEntry("Average");
  aLog.AddFloatEntry(fStatBuilder.fAverageMap["Aq"].mean*1e9);
  aLog.AddFloatEntry(fStatBuilder.fAverageMap["Aq"].rms*1e6);
  iter_bpm = bpmlist.begin();
  while(iter_bpm!=bpmlist.end()){
    aLog.AddFloatEntry(fStatBuilder.fAverageMap[*iter_bpm].mean*1e6);
    aLog.AddFloatEntry(fStatBuilder.fAverageMap[*iter_bpm].rms*1e3);
    iter_bpm++;
  }
  
  aLog.InsertHorizontalLine();
  aLog.Print();
  aLog.Close();
}
