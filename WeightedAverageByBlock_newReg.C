#include "LoadNormalizationMap.C"
#include "LoadPittsMap.C"
#include "LoadRunInfoMap.C"
#include "lib/TaRunInfo_v2.cc"
#include "lib/TaResult.cc"
#include "lib/TaStatBuilder.cc"
#include "lib/PlotPullFit.C"
#include "report_utility.cc"

void RegisterBranchesAddress(TTree*, vector<TString>,map<TString,StatData> &fMap);
void WeightedAverageByBlock_newReg(Int_t block_id);

void WeightedAverageByBlock_newReg(){
  for(int i=0;i<4;i++)
    WeightedAverageByBlock_newReg(i);
}

void WeightedAverageByBlock_newReg(Int_t block_id){
  TString tree_name = Form("block%d_newReg",block_id);
  vector<TString> fDetectorNameList={"asym_bcm_an_us","asym_bcm_an_ds",
				     "asym_bcm_an_ds3","asym_bcm_dg_us",
				     "asym_bcm_dg_ds","asym_cav4cQ",
				     "asym_us_avg","asym_usr","asym_usl",
				     "asym_us_dd","asym_ds_avg",
				     "reg_asym_us_avg","reg_asym_usr","reg_asym_usl",
				     "reg_asym_us_dd","reg_asym_ds_avg",
				     "diff_bpm4aX","diff_bpm4eX",
				     "diff_bpm4aY","diff_bpm4eY",
				     "diff_bpmE",
				     "diff_bpm11X","diff_bpm12X"};

  TString arm_switch[3]={"reg_asym_us_avg","reg_asym_usr","reg_asym_usl"};
  
  Int_t nDet = fDetectorNameList.size();
  TString dot_suffix = Form("_block%d",block_id);
  for(int idet=0;idet<nDet;idet++){
    fDetectorNameList[idet]+= dot_suffix;
  }
  map< Int_t,TString> fBCMRunMap = LoadNormalizationMap();  
  map< Int_t, TaRunInfo > fRunInfoMap = LoadRunInfoMap();
  map<SLUG_ARM,Int_t> fPittMap = LoadPittsMap();
  map<SLUG_ARM,pair<TString,TString> > fSlugInfoMap;
  map<SLUG_ARM,Int_t > fSlugSignMap;

  map<TString,StatData> fChannelMap;
  vector<TString> fDeviceNameList;
  StatData fZeroData;
  fZeroData.Zero();
  
  fDeviceNameList.insert(fDeviceNameList.end(),
			 fDetectorNameList.begin(),fDetectorNameList.end());
  
  map<SLUG_ARM, TaStatBuilder > fSlugStatBuilderMap;
  map<Int_t, TaStatBuilder > fPittsStatBuilderMap;
  map<Int_t, TaStatBuilder > fWienStatBuilderMap;
  
  for(int islug=1;islug<=94;islug++){
    TString file_name = Form("./rootfiles/newReg_slug%d.root",islug);
    TFile* input_file = TFile::Open(file_name);
    if(input_file==NULL){
      cerr <<  file_name << " is not found and is skipped" << endl;
      continue;
    }
    TTree* burst_tree = (TTree*)input_file->Get("mini");
    TTree* minireg_tree = (TTree*)input_file->Get("mini_reg");

    if(burst_tree==NULL || minireg_tree==NULL){
      cerr << " Error: mini Tree not found in "
	   << file_name << endl;
      continue;
    }
    burst_tree->AddFriend(minireg_tree);
    
    Int_t nEntries = burst_tree->GetEntries();
    Int_t run_number=0;
    Int_t mini_id=0;
    Double_t fMini_id=0;
    Int_t last_run_number=0;
    burst_tree->SetBranchAddress("run",&run_number);
    burst_tree->SetBranchAddress("mini",&fMini_id);
    RegisterBranchesAddress(burst_tree,fDeviceNameList,fChannelMap);
    TaRunInfo myRunInfo;
    SLUG_ARM myKey;
    TString detName, bcmName;
    for(int ievt=0;ievt<nEntries;ievt++){
      burst_tree->GetEntry(ievt);
      mini_id = (Int_t)fMini_id;
      if(fRunInfoMap.find(run_number)==fRunInfoMap.end()){
	cerr << "-- run info not found for run  "
	     << run_number << " and will skip "  << endl;
	continue;
      }
      if(run_number!=last_run_number){
	myRunInfo = fRunInfoMap[run_number];
	myKey = make_pair(myRunInfo.GetSlugNumber(),
			  myRunInfo.GetArmFlag());
	detName = arm_switch[myRunInfo.GetArmFlag()]+dot_suffix;
	if(fBCMRunMap.find(run_number)==fBCMRunMap.end()){
	  cerr << "-- normalizing BCM info not found for run  "
	       << run_number << endl;
	  continue;  // FIXME
	}else
	  bcmName = "asym_"+fBCMRunMap[run_number]+dot_suffix;

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
	fSlugStatBuilderMap[myKey].UpdateWeightingError(fChannelMap[detName]);
	fSlugStatBuilderMap[myKey].UpdateStatData("Adet"+dot_suffix,fChannelMap[detName]);
	fSlugStatBuilderMap[myKey].UpdateStatData("Aq"+dot_suffix,fChannelMap[bcmName]);
	fSlugStatBuilderMap[myKey].UpdateStatData("diff_bpmE"+dot_suffix,
						  fChannelMap["diff_bpm12X"+dot_suffix]);

	auto iter_dev = fDeviceNameList.begin();
	while(iter_dev!=fDeviceNameList.end()){
	  
	  if( myRunInfo.GetArmFlag()==1 ){
	    if((*iter_dev).Contains("sl")
	       || (*iter_dev).Contains("s_avg")
	       || (*iter_dev).Contains("s_dd") ) {
	      fSlugStatBuilderMap[myKey].UpdateStatData(*iter_dev,fZeroData);
	      iter_dev++;
	      continue;
	    }
	  }
	  if(myRunInfo.GetArmFlag()==2 ){
	    if((*iter_dev).Contains("sr")
	       || (*iter_dev).Contains("s_avg")
	       || (*iter_dev).Contains("s_dd") ) {
	      fSlugStatBuilderMap[myKey].UpdateStatData(*iter_dev,fZeroData);
	      iter_dev++;
	      continue;
	    }
	  }

	  fSlugStatBuilderMap[myKey].UpdateStatData(*iter_dev,fChannelMap[*iter_dev]);
	  iter_dev++;
	}
      }
    } // end of minirun loop inside a slug
    input_file->Close();
  }// end of slug loop
  
  TFile *output_rf =  TFile::Open(Form("prex_grand_average_%s_weighted.root",tree_name.Data()),"RECREATE");
  TTree *fSlugTree = new TTree("slug","Slug Averages");
  Double_t fSlugID;
  Double_t fArmSlug;
  Double_t fSign;
  Int_t  fWien;
  TString fIHWP;
  TBranch *fBranchSlug = fSlugTree->Branch("slug",&fSlugID);
  TBranch *fBranchArm = fSlugTree->Branch("arm_flag",&fArmSlug);
  TBranch *fBranchSign = fSlugTree->Branch("sign",&fSign);
  TBranch *fBranchWien = fSlugTree->Branch("wien",&fWien);
  TBranch *fBranchIHWP = fSlugTree->Branch("ihwp",&fIHWP);
  TaResult fSlugLog_md("average_by_slug_"+tree_name+".log");
  // TaResult fSlugLog_beamline("average_by_slug_beamline.log");
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
    (*iter_slug).second.PullFitAllChannels("./plots/slug"+slug_label+"_"+tree_name+".pdf");
    (*iter_slug).second.FillTree(fSlugTree);
    fWien = wienID;
    fIHWP = (fSlugInfoMap[(*iter_slug).first]).first;
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
  // ReportBeamLineLog(fSlugStatBuilderMap,fSlugLog_beamline);
  
  TaStatBuilder fPittsStatBuilder;
  TaStatBuilder fPittNullStatBuilder;
  map<Int_t,TaStatBuilder> fPittsNullStatMap;
  TTree *fPittsTree = new TTree("pitt","Pitts Averages");
  Int_t fPittsID;
  fPittsTree->Branch("pitt",&fPittsID);
  auto iter_pitts = fPittsStatBuilderMap.begin();
  while(iter_pitts!=fPittsStatBuilderMap.end()){
    fPittsID = (*iter_pitts).first;
    fPittsStatBuilder.SetLabel(Form("%d",fPittsID));
    (*iter_pitts).second.PullFitAllChannels(Form("./plots/pitts%d_%s.pdf",
						 fPittsID,tree_name.Data()));
    fPittsStatBuilder.UpdateStatBuilder((*iter_pitts).second);
    ((*iter_pitts).second).FillTree(fPittsTree);
    
    TaStatBuilder fNullStat =(*iter_pitts).second.GetNullStatBuilder();
    fPittNullStatBuilder.SetLabel(Form("%d",fPittsID));
    fPittNullStatBuilder.UpdateStatBuilder(fNullStat);
    fPittsNullStatMap[fPittsID] = fNullStat;
    fNullStat.FillTree(fPittsTree,"null_");
    fPittsTree->Fill();
    iter_pitts++;
  }
  fPittsStatBuilder.PullFitAllChannels(tree_name+"_pitts_pullfit.pdf");
  fPittNullStatBuilder.PullFitAllChannels(tree_name+"_pitt_null_pullfit.pdf");
  // TaResult fPittLog_md("average_by_pitt_maindet_dit.log");
  // ReportDetectorLog(fPittsStatBuilder,fPittLog_md);
  // ReportBeamLineLog(fPittsStatBuilderMap,fPittLog_beamline);
  // TaResult fPittLog_beamline("average_by_pitt_beamline.log");
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
    (*iter_wien).second.PullFitAllChannels(Form("./plots/wien%d_%s.pdf",fWienID,tree_name.Data()));
    fWienStatBuilder.UpdateStatBuilder((*iter_wien).second);
    (*iter_wien).second.FillTree(fWienTree);
    
    TaStatBuilder fNullStat = (*iter_wien).second.GetNullStatBuilder();
    fWienNullStateMap[fWienID] = fNullStat;
    fNullStat.FillTree(fWienTree,"null_");
    fWienTree->Fill();
    iter_wien++;
  }
  
  // TaResult fWienLog_beamline("average_by_wien_beamline.log"); // 
  // ReportBeamLineLog(fWienStatBuilderMap,fWienLog_beamline);
  fWienStatBuilder.PullFitAllChannels(tree_name+"_wien_pullfit.pdf");
  // TaResult fWienLog_md("average_by_wien_maindet_dit.log");
  // ReportDetectorLog(fWienStatBuilder,fWienLog_md);

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
