#include "LoadNormalizationMap.C"
#include "LoadPittsMap.C"
#include "LoadRunInfoMap.C"
#include "lib/TaRunInfo_v2.cc"
#include "lib/TaResult.cc"
#include "lib/TaStatBuilder.cc"
#include "lib/PlotPullFit.C"
#include "report_utility.cc"

void RegisterBranchesAddress(TTree*, vector<TString>,map<TString,StatData> &fMap);

void AverageLRBBurst(){
  vector<TString> fDetectorNameList{"cor_asym_us_avg","cor_usr","cor_usl",
				    "cor_asym_us_dd"};
  
  TString arm_switch[3]={"cor_asym_us_avg","cor_usr","cor_usl"};
  
  map< Int_t, TaRunInfo > fRunInfoMap = LoadRunInfoMap();
  map<SLUG_ARM,Int_t> fPittMap = LoadPittsMap();
  map<SLUG_ARM,pair<TString,TString> > fSlugInfoMap;
  map<SLUG_ARM,Int_t > fSlugSignMap;

  map<TString,StatData> fChannelMap;
  vector<TString> fDeviceNameList;
  
  fDeviceNameList.insert(fDeviceNameList.end(),
			 fDetectorNameList.begin(),fDetectorNameList.end());
  
  map<SLUG_ARM, TaStatBuilder > fSlugStatBuilderMap;
  map<Int_t, TaStatBuilder > fPittsStatBuilderMap;
  map<Int_t, TaStatBuilder > fWienStatBuilderMap;
  
  for(int islug=1;islug<=94;islug++){
    TString file_name = Form("./rootfiles/MergedSum_slug%d.root",islug);
    TFile* input_file = TFile::Open(file_name);
    if(input_file==NULL){
      cerr <<  file_name << " is not found and is skipped" << endl;
      continue;
    }
    TTree* lrbs_tree = (TTree*)input_file->Get("burst_mulc_lrb_burst");
    lrbs_tree->AddFriend( (TTree*)input_file->Get("burst_mulc") );

    if(lrbs_tree==NULL){
      cerr << " Error: Lrb Tree not found in "
	   << file_name << endl;
      continue;
    }
    Int_t nEntries = lrbs_tree->GetEntries();
    Int_t run_number=0;
    Int_t mini_id=0;
    Int_t last_run_number=0;
    lrbs_tree->SetBranchAddress("run",&run_number);
    lrbs_tree->SetBranchAddress("minirun",&mini_id);
    // lrbs_tree->SetBranchAddress("mini",&mini_id);
    RegisterBranchesAddress(lrbs_tree,fDeviceNameList,fChannelMap);
    TaRunInfo myRunInfo;
    SLUG_ARM myKey;
    TString detName, bcmName;
    for(int ievt=0;ievt<nEntries;ievt++){
      lrbs_tree->GetEntry(ievt);
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
	fSlugStatBuilderMap[myKey].SetLabel(Form("%d.%d",run_number,mini_id));
	fSlugStatBuilderMap[myKey].UpdateStatData("Adet",fChannelMap[detName]);
	auto iter_dev = fDeviceNameList.begin();
	while(iter_dev!=fDeviceNameList.end()){
	  if( myRunInfo.GetArmFlag()==1 ){
	    if((*iter_dev).Contains("l")
	       || (*iter_dev).Contains("s_avg")
	       || (*iter_dev).Contains("s_dd") ) {
	      iter_dev++;
	      continue;
	    }
	  }
	  if(myRunInfo.GetArmFlag()==2 ){
	    if((*iter_dev).Contains("r")
	       || (*iter_dev).Contains("s_avg")
	       || (*iter_dev).Contains("s_dd") ) {
	      iter_dev++;
	      continue;
	    }
	  }
	  fSlugStatBuilderMap[myKey].UpdateStatData(*iter_dev,fChannelMap[*iter_dev]);
	  iter_dev++;
	}
      }else
	cout << "run " << run_number << " is not a good run " << endl;
    } // end of minirun loop inside a slug
    input_file->Close();
  }// end of slug loop
  
  TFile *output_rf =  TFile::Open("prex_grand_average_lrb_respin2.root","RECREATE");
  TTree *fSlugTree = new TTree("slug","Slug Averages");
  Double_t fSlugID;
  Double_t fArmSlug;
  Double_t fSign;
  TBranch *fBranchSlug = fSlugTree->Branch("slug",&fSlugID);
  TBranch *fBranchArm = fSlugTree->Branch("arm_flag",&fArmSlug);
  TBranch *fBranchSign = fSlugTree->Branch("sign",&fSign);
  TaResult fSlugLog_md("average_by_slug_maindet_lrb.log");
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
    (*iter_slug).second.PullFitAllChannels("./plots/lrb_slug"+slug_label+".pdf");
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
  
  ReportDetectorLog(fSlugStatBuilderMap,fSlugLog_md);
  
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
    (*iter_pitts).second.PullFitAllChannels(Form("./plots/lrb_pitts%d.pdf",fPittsID));
    fPittsStatBuilder.UpdateStatBuilder((*iter_pitts).second);
    ((*iter_pitts).second).FillTree(fPittsTree);
    
    TaStatBuilder fNullStat =(*iter_pitts).second.GetNullStatBuilder();
    fPittsNullStatMap[fPittsID] = fNullStat;
    fPittsNullStatBuilder.SetLabel(Form("%d",fPittsID));
    fPittsNullStatBuilder.UpdateStatBuilder(fNullStat);
    fNullStat.FillTree(fPittsTree,"null_");
    fPittsTree->Fill();
    iter_pitts++;
  }
  fPittsStatBuilder.PullFitAllChannels("lrb_pitts_pullfit.pdf");
  fPittsNullStatBuilder.PullFitAllChannels("lrb_pitts_null_pullfit.pdf");
  TaResult fPittLog_md("average_by_pitt_maindet_lrb.log");
  TaResult fPittLog_mdnull("null_by_pitt_maindet_lrb.log");
  ReportDetectorLog(fPittsStatBuilder,fPittLog_md);
  ReportDetectorLogByIHWP(fPittsNullStatBuilder,fPittLog_mdnull);

  TaStatBuilder fWienStatBuilder;
  map<Int_t,TaStatBuilder> fWienNullStateMap;
  TTree *fWienTree = new TTree("wien","Wien Averages");
  Int_t fWienID;
  fWienTree->Branch("wien",&fWienID);
  auto iter_wien = fWienStatBuilderMap.begin();
  while(iter_wien!=fWienStatBuilderMap.end()){
    fWienID = (*iter_wien).first;
    cout << fWienID << endl;
    fWienStatBuilder.SetLabel(Form("%d",fWienID));
    (*iter_wien).second.PullFitAllChannels(Form("./plots/lrb_wien%d.pdf",fWienID));
    (*iter_wien).second.PullFitAllChannelsByIHWP("dummy.pdf");
    fWienStatBuilder.UpdateStatBuilder((*iter_wien).second);
    (*iter_wien).second.FillTree(fWienTree);
    
    TaStatBuilder fNullStat = (*iter_wien).second.GetNullStatBuilder();
    fWienNullStateMap[fWienID] = fNullStat;
    fNullStat.FillTree(fWienTree,"null_");
    fWienTree->Fill();
    iter_wien++;
  }
  
  fWienStatBuilder.PullFitAllChannels("lrb_wien_pullfit.pdf");
  TaResult fWienLog_md("average_by_wien_maindet_lrb.log");
  ReportDetectorLogByIHWP(fWienStatBuilder,fWienLog_md);

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
      fMap[*iter].RegisterAddressByName_dither(aBranch);
    }else{
      fMap[*iter] = fStatDataZero;
    }
    iter++;
  }

}
