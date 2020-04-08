#include "LoadNormalizationMap.C"
#include "LoadPittsMap.C"
#include "LoadRunInfoMap.C"
#include "lib/TaRunInfo_v2.cc"
#include "lib/TaResult.cc"
#include "lib/TaStatBuilder.cc"
#include "lib/PlotPullFit.C"
#include "report_utility.cc"
void RegisterBranchesAddress(TTree*, vector<TString>,map<TString,StatData> &fMap);

void WeightedAveragePostpanCobpm(){
  vector<TString> fDetectorNameList{"reg_asym_us_avg","reg_asym_usr","reg_asym_usl",
				    "reg_asym_us_dd","reg_asym_ds_avg",
				    "reg_asym_left_dd","reg_asym_right_dd"};
  
  vector<TString> fRawDetectorNameList{"asym_us_avg","asym_usr","asym_usl",
  				       "asym_us_dd","asym_ds_avg",
  				       "asym_left_dd","asym_right_dd"};
  
  vector<TString> fBPMNameList{"diff_bpm4aX","diff_bpm4eX",
  			       "diff_bpm4aY","diff_bpm4eY",
  			       "diff_bpmE","diff_bpm12X"};
  
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

    TString file_name = Form("./rootfiles/prexPrompt_slug%d_cbpm.root",islug);
    if(islug<=3)
      file_name = Form("./rootfiles/prexPrompt_slug%d.root",islug);
    
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
	     << run_number << " and will skip "  << endl;
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
	fSlugStatBuilderMap[myKey].SetLabel(Form("%d.%d",run_number,mini_number));
	fSlugStatBuilderMap[myKey].UpdateWeightingError(fChannelMap[detName]);
	fSlugStatBuilderMap[myKey].UpdateStatData("Adet",fChannelMap[detName]);
	fSlugStatBuilderMap[myKey].UpdateStatData("Aq",fChannelMap[bcmName]);
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
  
  TFile *output_rf =  TFile::Open("prex_grand_weighted_average_regress.root","RECREATE");
  TTree *fSlugTree = new TTree("slug","Slug Averages");
  Double_t fSlugID;
  Double_t fArmSlug;
  Double_t fSign;
  TBranch *fBranchSlug = fSlugTree->Branch("slug",&fSlugID);
  TBranch *fBranchArm = fSlugTree->Branch("arm_flag",&fArmSlug);
  TBranch *fBranchSign = fSlugTree->Branch("sign",&fSign);
  // TaResult fSlugLog_md("average_by_slug_maindet_reg_reg.log");
  TaResult fSlugLog_beamline("weighted_average_by_slug_beamline_reg_cbpm.log");
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
    // (*iter_slug).second.PullFitAllChannels("./plots/reg_slug"+slug_label+".pdf");
    (*iter_slug).second.FillTree(fSlugTree);
    fSlugTree->Fill();
    Int_t pittsID = fPittMap[(*iter_slug).first];
    TString IHWP_state = (fSlugInfoMap[(*iter_slug).first]).first;
    TString Wien_state = (fSlugInfoMap[(*iter_slug).first]).second;
    if(pittsID!=-1){
      fPittsStatBuilderMap[pittsID].SetLabel(slug_label);
      fPittsStatBuilderMap[pittsID].UpdateStatBuilderByIHWP( (*iter_slug).second,
							     IHWP_state,
							     fSign);
    }
    if(Wien_state!=last_wien_state){
      wienID++;
      last_wien_state = Wien_state;
    }
    fWienStatBuilderMap[wienID].SetLabel(slug_label);
    fWienStatBuilderMap[wienID].UpdateStatBuilderByIHWP( (*iter_slug).second,
							 IHWP_state,
							 fSign);
    iter_slug++;
  }
  
  // ReportDetectorLog(fSlugStatBuilderMap,fSlugLog_md);
  ReportBeamLineLog(fSlugStatBuilderMap,fSlugLog_beamline);
  
  TaStatBuilder fPittsStatBuilder;
  TaStatBuilder fPittsNullStatBuilder;
  map<Int_t,TaStatBuilder> fPittsNullStatMap;
  TTree *fPittsTree = new TTree("pitt","Pitts Averages");
  Int_t fPittsID;
  fPittsTree->Branch("pitt",&fPittsID);
  auto iter_pitts = fPittsStatBuilderMap.begin();
  while(iter_pitts!=fPittsStatBuilderMap.end()){
    fPittsID = (*iter_pitts).first;
    fPittsStatBuilder.SetLabel(Form("%d",fPittsID));
    fPittsStatBuilder.UpdateStatBuilder((*iter_pitts).second);
    // (*iter_pitts).second.PullFitAllChannels(Form("./plots/reg_pitts%d.pdf",fPittsID));
    ((*iter_pitts).second).FillTree(fPittsTree);
    
    TaStatBuilder fNullStat =(*iter_pitts).second.GetNullStatBuilder();
    fPittsNullStatMap[fPittsID] = fNullStat;
    fPittsNullStatBuilder.SetLabel(Form("%d",fPittsID));
    fPittsNullStatBuilder.UpdateStatBuilder(fNullStat);
    fNullStat.FillTree(fPittsTree,"null_");
    fPittsTree->Fill();
    iter_pitts++;
  }
  // fPittsStatBuilder.PullFitAllChannels("reg_pitts_pullfit.pdf");
  // TaResult fPittLog_md("average_by_pitt_maindet_reg.log");
  // ReportDetectorLog(fPittsStatBuilderMap,fPittLog_md);
  TaResult fPittLog_beamline("weighted_average_by_pitt_beamline_reg_cbpm.log");
  ReportBeamLineLog(fPittsStatBuilder,fPittLog_beamline);

  fPittsNullStatBuilder.PullFitAllChannels("reg_cbpm_pitt_null_pullfit.pdf");
  TaResult fPittLog_null_beamline("weighted_null_by_pitt_beamline_reg_cbpm.log");
  ReportBeamLineLog(fPittsNullStatBuilder,fPittLog_null_beamline);

  TaResult fPittLog_null_md("weighted_null_by_pitt_maindet_reg_cbpm.log");
  ReportDetectorLogByIHWP(fPittsNullStatBuilder,fPittLog_null_md);

  TaStatBuilder fWienStatBuilder;
  TaStatBuilder fWienNullStatBuilder;
  map<Int_t,TaStatBuilder> fWienNullStatMap;
  TTree *fWienTree = new TTree("wien","Wien Averages");
  Int_t fWienID;
  fWienTree->Branch("wien",&fWienID);
  auto iter_wien = fWienStatBuilderMap.begin();
  while(iter_wien!=fWienStatBuilderMap.end()){
    fWienID = (*iter_wien).first;
    fWienStatBuilder.SetLabel(Form("%d",fWienID));
    fWienNullStatBuilder.SetLabel(Form("%d",fWienID));
    fWienStatBuilder.UpdateStatBuilder((*iter_wien).second);
    // (*iter_wien).second.PullFitAllChannels(Form("./plots/reg_wien%d.pdf",fWienID));
    (*iter_wien).second.FillTree(fWienTree);
    
    TaStatBuilder fNullStat = (*iter_wien).second.GetNullStatBuilder();
    fWienNullStatMap[fWienID] = fNullStat;
    fWienNullStatBuilder.UpdateStatBuilder(fNullStat);
    fNullStat.FillTree(fWienTree,"null_");
    fWienTree->Fill();
    iter_wien++;
  }
  
  // TaResult fWienLog_md("average_by_wien_maindet_reg.log");
  //  ReportDetectorLog(fWienStatBuilderMap,fWienLog_md);
  fWienNullStatBuilder.PullFitAllChannels("reg_cbpm_wien_null_pullfit.pdf");
  TaResult fWienLog_beamline("weighted_average_by_wien_beamline_reg_cbpm.log");
  ReportBeamLineLog(fWienStatBuilder,fWienLog_beamline);
  TaResult fWienLog_null_beamline("weighted_null_by_wien_beamline_reg_cbpm.log");
  ReportBeamLineLog(fWienNullStatBuilder,fWienLog_null_beamline);
  TaResult fWienLog_null_md("weighted_null_by_wien_maindet_reg_cbpm.log");
  ReportDetectorLogByIHWP(fWienNullStatBuilder,fWienLog_null_md);

  // fWienStatBuilder.PullFitAllChannels("reg_wien_pullfit.pdf");
  fWienTree->Write();
  fPittsTree->Write();
  fSlugTree->Write();
  output_rf->Close();
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
