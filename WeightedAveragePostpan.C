#include "LoadNormalizationMap.C"
#include "LoadPittsMap.C"
#include "LoadRunInfoMap.C"
#include "lib/TaRunInfo_v2.cc"
#include "lib/TaResult.cc"
#include "lib/TaStatBuilder.cc"
#include "lib/PlotPullFit.C"
void RegisterBranchesAddress(TTree*, vector<TString>,map<TString,StatData> &fMap);
TF1* PullFitByPitts(map<Int_t,TaStatBuilder> fPittsStatBuilderMap, TString chname);

void WeightedAveragePostpan(){
  vector<TString> fDetectorNameList{"reg_asym_us_avg","reg_asym_usr","reg_asym_usl",
				    "reg_asym_us_dd","reg_asym_ds_avg",
				    "reg_asym_left_dd","reg_asym_right_dd"};
  
  vector<TString> fRawDetectorNameList{"asym_us_avg","asym_usr","asym_usl",
				       "asym_us_dd","asym_ds_avg",
				       "asym_left_dd","asym_right_dd"};
  
  vector<TString> fBPMNameList{"diff_bpm4aX","diff_bpm4eX",
			       "diff_bpm4aY","diff_bpm4eY",
			       "diff_bpm11X","diff_bpm12X"};
  
  vector<TString> fBCMNameList{"asym_bcm_an_us","asym_bcm_an_ds",
			       "asym_bcm_an_ds3","asym_bcm_dg_us",
			       "asym_bcm_dg_ds"};
  
  TString arm_switch[3]={"reg_asym_us_avg","reg_asym_usr","reg_asym_usl"};
  
  Int_t nBPM = fBPMNameList.size();
  Int_t nDet = fDetectorNameList.size();
  
  map< Int_t,TString> fBCMRunMap = LoadNormalizationMap();
  map< Int_t, TaRunInfo > fRunInfoMap = LoadRunInfoMap();
  map<SLUG_ARM,Int_t> fPittMap = LoadPittsMap();
  map<SLUG_ARM,pair<TString,TString> > fSlugInfoMap;
  map<SLUG_ARM,Int_t > fSlugSignMap;

  map<TString,StatData> fChannelMap;
  vector<TString> fDeviceNameList;
  
  fDeviceNameList.insert(fDeviceNameList.end(),
			 fDetectorNameList.begin(),fDetectorNameList.end());
  fDeviceNameList.insert(fDeviceNameList.end(),
			 fRawDetectorNameList.begin(),fRawDetectorNameList.end());
  fDeviceNameList.insert(fDeviceNameList.end(),
			 fBPMNameList.begin(),fBPMNameList.end());
  fDeviceNameList.insert(fDeviceNameList.end(),
			 fBCMNameList.begin(),fBCMNameList.end());
  
  map<SLUG_ARM, TaStatBuilder > fSlugStatBuilderMap;
  map<Int_t, TaStatBuilder > fPittsStatBuilderMap;
  map<Int_t, TaStatBuilder > fWienStatBuilderMap;
  
  for(int islug=1;islug<=94;islug++){
    TString file_name = Form("./rootfiles/postpan/prexPrompt_slug%d.root",islug);
    TFile* input_file = TFile::Open(file_name);
    if(input_file==NULL){
      cerr <<  file_name << " is not found and is skipped" << endl;
      continue;
    }
    TTree* mini_tree = (TTree*)input_file->Get("T");
    if(mini_tree==NULL){
      cerr << " Error: Mini Tree not found in "
	   << file_name << endl;
      continue;
    }
    Int_t nEntries = mini_tree->GetEntries();
    Int_t run_number=0;
    Int_t mini_number=0;
    Int_t last_run_number=0;
    mini_tree->SetBranchAddress("run",&run_number);
    mini_tree->SetBranchAddress("mini",&mini_number);
    RegisterBranchesAddress(mini_tree,fDeviceNameList,fChannelMap);
    TaRunInfo myRunInfo;
    SLUG_ARM myKey;
    TString detName, bcmName;
    for(int ievt=0;ievt<nEntries;ievt++){
      mini_tree->GetEntry(ievt);
      if(fRunInfoMap.find(run_number)==fRunInfoMap.end()){
	cerr << "-- run info not found for run  "
	     << run_number << endl;
	continue;
      }
      if(run_number!=last_run_number){
	myRunInfo = fRunInfoMap[run_number];
	myKey = make_pair(myRunInfo.GetSlugNumber(),
			  myRunInfo.GetArmFlag());
	detName = arm_switch[myRunInfo.GetArmFlag()];
	if(fBCMRunMap.find(run_number)==fBCMRunMap.end()){
	  cerr << "-- normalizing BCM info not found for run  "
	       << run_number << endl;
	  continue;  // FIXME
	}else
	  bcmName = "asym_"+fBCMRunMap[run_number];
	last_run_number = run_number;
	if(fSlugStatBuilderMap.find(myKey)==fSlugStatBuilderMap.end()){
	  TaStatBuilder fStatBuilder;
	  fSlugStatBuilderMap.insert(make_pair(myKey,fStatBuilder));
	  fSlugInfoMap[myKey] = make_pair(myRunInfo.GetIHWPStatus(),
					  myRunInfo.GetWienMode());
	  fSlugSignMap[myKey] = myRunInfo.GetSign();
	}
      } // end if it is a new run number
      if(myRunInfo.GetRunFlag()=="Good"){
	fSlugStatBuilderMap[myKey].SetTmpTitle(Form("%d.%d",run_number,mini_number));
	fSlugStatBuilderMap[myKey].UpdateWeightingError(fChannelMap[detName]);
	fSlugStatBuilderMap[myKey].UpdateMainDet(fChannelMap[detName]);
	fSlugStatBuilderMap[myKey].UpdateStatData("Aq",fChannelMap[bcmName]);
	fSlugStatBuilderMap[myKey].UpdateStatData("diff_bpmE",
						  fChannelMap["diff_bpm11X"]);
	fSlugStatBuilderMap[myKey].UpdateStatData("diff_bpmE",
						  fChannelMap["diff_bpm12X"]);
	auto iter_dev = fDeviceNameList.begin();
	while(iter_dev!=fDeviceNameList.end()){
	  fSlugStatBuilderMap[myKey].UpdateStatData(*iter_dev,fChannelMap[*iter_dev]);
	  iter_dev++;
	}
      }
    } // end of minirun loop inside a slug
    input_file->Close();
  }// end of slug loop
  
  TFile *output_rf =  TFile::Open("test_weighted_average.root","RECREATE");
  TTree *fSlugTree = new TTree("slug","Slug Averages");
  Double_t fSlugID;
  Double_t fArmSlug;
  Double_t fSign;
  TBranch *fBranchSlug = fSlugTree->Branch("slug",&fSlugID);
  TBranch *fBranchArm = fSlugTree->Branch("arm_flag",&fArmSlug);
  TBranch *fBranchSign = fSlugTree->Branch("sign",&fSign);
  TaResult fSlugLog_md("average_by_slug_maindet.log");
  vector<TString> header_slug{"Slug","Mean(ppb)","Error","chi2/ndf"};
  fSlugLog_md.AddHeader(header_slug);
  auto iter_slug = fSlugStatBuilderMap.begin();
  Int_t wienID = -1;
  TString last_wien_state="";
  while(iter_slug!=fSlugStatBuilderMap.end()){
    fSlugID = ((*iter_slug).first).first;
    TString slug_label = Form("%.0f",fSlugID);
    fArmSlug = ((*iter_slug).first).second;
    fSlugID += fArmSlug/10.0;
    if(fArmSlug==1)
      slug_label+="R";
    else if(fArmSlug==2)
      slug_label+="L";
    fSign = fSlugSignMap[(*iter_slug).first];
    // (*iter_slug).second.PullFitAllChannels("./plots/slug"+slug_label+".pdf");
    (*iter_slug).second.FillTree(fSlugTree);
    fSlugTree->Fill();
    fSlugLog_md.AddLine();
    fSlugLog_md.AddStringEntry(slug_label);
    fSlugLog_md.AddFloatEntry(((*iter_slug).second).fAverageMap["Adet"].mean*1e9);
    fSlugLog_md.AddFloatEntry(((*iter_slug).second).fAverageMap["Adet"].error*1e9);
    fSlugLog_md.AddChi2NDF(((*iter_slug).second).fAverageMap["Adet"].chi2,
			   ((*iter_slug).second).fAverageMap["Adet"].ndf);
    Int_t pittsID = fPittMap[(*iter_slug).first];
    TString IHWP_state = (fSlugInfoMap[(*iter_slug).first]).first;
    TString Wien_state = (fSlugInfoMap[(*iter_slug).first]).second;
    if(pittsID!=-1)
      fPittsStatBuilderMap[pittsID].UpdateStatBuilderByIHWP( (*iter_slug).second,
							     IHWP_state,
							     fSign);
    if(Wien_state!=last_wien_state){
      wienID++;
      last_wien_state = Wien_state;
    }
    fWienStatBuilderMap[wienID].UpdateStatBuilderByIHWP( (*iter_slug).second,
							 IHWP_state,
							 fSign);
    iter_slug++;
  }
  fSlugLog_md.InsertHorizontalLine();
  fSlugLog_md.Print();
  fSlugLog_md.Close();

  TaStatBuilder fPittsStatBuilder;
  map<Int_t,TaStatBuilder> fPittsNullStatMap;
  TTree *fPittsTree = new TTree("pitt","Pitts Averages");
  Int_t fPittsID;
  fPittsTree->Branch("pitt",&fPittsID);
  auto iter_pitts = fPittsStatBuilderMap.begin();
  while(iter_pitts!=fPittsStatBuilderMap.end()){
    fPittsID = (*iter_pitts).first;
    fPittsStatBuilder.SetTmpTitle(Form("%d",fPittsID));
    fPittsStatBuilder.UpdateStatBuilder((*iter_pitts).second);
    ((*iter_pitts).second).FillTree(fPittsTree);
    
    TaStatBuilder fNullStat =(*iter_pitts).second.GetNullStatBuilder();
    fPittsNullStatMap[fPittsID] = fNullStat;
    fNullStat.FillTree(fPittsTree,"null_");
    fPittsTree->Fill();
    iter_pitts++;
  }
  fPittsStatBuilder.PullFitAllChannels("pitts_pullfit.pdf");
  // PullFitByPitts(fPittsStatBuilderMap,"AdetByPitts");
  // PullFitByPitts(fPittsNullStatMap,"NullAdetByPitts");

  TaStatBuilder fWienStatBuilder;
  map<Int_t,TaStatBuilder> fWienNullStateMap;
  TTree *fWienTree = new TTree("wien","Wien Averages");
  Int_t fWienID;
  fWienTree->Branch("wien",&fWienID);
  auto iter_wien = fWienStatBuilderMap.begin();
  while(iter_wien!=fWienStatBuilderMap.end()){
    fWienID = (*iter_wien).first;
    fWienStatBuilder.SetTmpTitle(Form("%d",fWienID));
    fWienStatBuilder.UpdateStatBuilder((*iter_wien).second);
    (*iter_wien).second.FillTree(fWienTree);
    
    TaStatBuilder fNullStat = (*iter_wien).second.GetNullStatBuilder();
    fWienNullStateMap[fWienID] = fNullStat;
    fNullStat.FillTree(fWienTree,"null_");
    fWienTree->Fill();
    iter_wien++;
  }

  fWienStatBuilder.PullFitAllChannels("wien_pullfit.pdf");
  fWienTree->Write();
  fPittsTree->Write();
  fSlugTree->Write();
  output_rf->Close();
}

TF1* PullFitByPitts(map<Int_t,TaStatBuilder> fPittsStatBuilderMap, TString chname){
  vector<Double_t> x_val, y_val, y_err;
  TaResult fRes(chname+".log");
  vector<TString> header_txt{"Pitt","Mean(ppb)","Error","chi2/ndf"};
  fRes.AddHeader(header_txt);
  auto iter = fPittsStatBuilderMap.begin();
  while(iter!=fPittsStatBuilderMap.end()){
    x_val.push_back((*iter).first);
    y_val.push_back(((*iter).second).fAverageMap["Adet"].mean*1e9);
    y_err.push_back(((*iter).second).fAverageMap["Adet"].error*1e9);
    fRes.AddLine();
    fRes.AddFloatEntry((*iter).first);
    fRes.AddFloatEntry(((*iter).second).fAverageMap["Adet"].mean*1e9);
    fRes.AddFloatEntry(((*iter).second).fAverageMap["Adet"].error*1e9);
    iter++;
  }
  fRes.InsertHorizontalLine();
  TF1* fret =  PlotPullFit(y_val,y_err,x_val,chname);
  fRes.AddStringEntry("Average");
  fRes.AddFloatEntry(fret->GetParameter(0));
  fRes.AddFloatEntry(fret->GetParError(0));
  fRes.AddStringEntry(Form("%.1f/%d",
			   fret->GetChisquare(),
			   fret->GetNDF()));
  fRes.InsertHorizontalLine();
  fRes.Print();
  fRes.Close();
  return fret;
}

void RegisterBranchesAddress(TTree *aTree,
			     vector<TString> namelist,
			     map<TString,StatData> &fMap){
  StatData fStatDataZero;
  auto iter = namelist.begin();
  while(iter!=namelist.end()){
    if(fMap.find(*iter)==fMap.end()){
      fMap.insert(make_pair(*iter,fStatDataZero));
    }
    TBranch *aBranch = aTree->GetBranch(*iter);
    if(aBranch!=NULL){
      fMap[*iter].RegisterAddressByName_postpan(aBranch);
    }else{
      fMap[*iter] = fStatDataZero;
    }
    iter++;
  }

}
				  
