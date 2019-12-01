#include "LoadNormalizationMap.C"
#include "LoadPittsMap.C"
#include "LoadRunInfoMap.C"
#include "lib/TaRunInfo_v2.cc"
#include "lib/TaResult.cc"
#include "lib/TaStatBuilder.cc"
#include "lib/PlotPullFit.C"
void RegisterBranchesAddress(TTree*, vector<TString>,map<TString,StatData> &fMap);
TF1* PullFitByPitts(map<Int_t,TaStatBuilder> fPittsStatBuilderMap, TString chname);

void AveragePostpan(){
  vector<TString> fDetectorNameList{"reg_asym_us_avg","reg_asym_usr","reg_asym_usl",
				    "reg_asym_us_dd",
				    "reg_asym_left_dd","reg_asym_right_dd"};
  
  vector<TString> fRawDetectorNameList{"asym_us_avg","asym_usr","asym_usl",
				       "asym_us_dd",
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
  map<TString,StatData> fDetChannelMap;
  map<TString,StatData> fRawDetChannelMap;
  map<TString,StatData> fMonChannelMap;
  map<TString,StatData> fBCMChannelMap;
  StatData fAqStatData;
  
  map<SLUG_ARM, TaStatBuilder > fSlugStatBuilderMap;
  map<Int_t, TaStatBuilder > fPittsStatBuilderMap;
  
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
    Int_t last_run_number=0;
    mini_tree->SetBranchAddress("run",&run_number);
    RegisterBranchesAddress(mini_tree,fBPMNameList,fMonChannelMap);
    RegisterBranchesAddress(mini_tree,fRawDetectorNameList,fRawDetChannelMap);
    RegisterBranchesAddress(mini_tree,fDetectorNameList,fDetChannelMap);
    RegisterBranchesAddress(mini_tree,fBCMNameList,fBCMChannelMap);
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
	  continue;
	}else
	  bcmName = fBCMRunMap[run_number];
	last_run_number = run_number;
	if(fSlugStatBuilderMap.find(myKey)==fSlugStatBuilderMap.end()){
	  TaStatBuilder fStatBuilder;
	  fSlugStatBuilderMap.insert(make_pair(myKey,fStatBuilder));
	  fSlugInfoMap[myKey] = make_pair(myRunInfo.GetIHWPStatus(),
					  myRunInfo.GetWienMode());
	  fSlugSignMap[myKey] = myRunInfo.GetSign();
	}
      }
      if(myRunInfo.GetRunFlag()=="Good"){
	fSlugStatBuilderMap[myKey].UpdateMainDet(fDetChannelMap[detName]);
	fSlugStatBuilderMap[myKey].UpdateChargeAsym(fBCMChannelMap[bcmName]);
      }
    } // end of run loop inside a slug
    input_file->Close();
  }// end of slug loop
  TFile *output_rf =  TFile::Open("test.root","RECREATE");
  TTree *fSlugTree = new TTree("slug","Slug Averages");
  Double_t fSlugID;
  Double_t fArmSlug;
  Double_t fSign;
  TBranch *fBranchSlug = fSlugTree->Branch("slug",&fSlugID);
  TBranch *fBranchArm = fSlugTree->Branch("arm_flag",&fArmSlug);
  TBranch *fBranchSign = fSlugTree->Branch("sign",&fSign);
  auto iter_slug = fSlugStatBuilderMap.begin();
  while(iter_slug!=fSlugStatBuilderMap.end()){
    fSlugID = ((*iter_slug).first).first;
    fArmSlug = ((*iter_slug).first).second;
    fSlugID += fArmSlug/10.0;
    fSign = fSlugSignMap[(*iter_slug).first];
    ((*iter_slug).second).FillTree(fSlugTree);
    fSlugTree->Fill();

    Int_t pittsID = fPittMap[(*iter_slug).first];
    if(pittsID!=-1){
      TString IHWP_status = (fSlugInfoMap[(*iter_slug).first]).first;
      fPittsStatBuilderMap[pittsID].UpdateStatBuilderByIHWP( (*iter_slug).second,
							     IHWP_status,
							     fSign);
    }
    iter_slug++;
  }
  map<Int_t,TaStatBuilder> fPittsNullStatMap;
  TTree *fPittsTree = new TTree("pitt","Pitts Averages");
  Int_t fPittsID;
  fPittsTree->Branch("pitt",&fPittsID);
  auto iter_pitts = fPittsStatBuilderMap.begin();
  while(iter_pitts!=fPittsStatBuilderMap.end()){
    fPittsID = (*iter_pitts).first;
    ((*iter_pitts).second).FillTree(fPittsTree);
    TaStatBuilder fNullStat =((*iter_pitts).second).GetNullStatBuilder();
    fPittsNullStatMap[fPittsID] = fNullStat;
    fNullStat.FillTreeWithNullAverages(fPittsTree);
    fPittsTree->Fill();
    iter_pitts++;
  }
  PullFitByPitts(fPittsStatBuilderMap,"AdetByPitts");
  PullFitByPitts(fPittsNullStatMap,"NullAdetByPitts");
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
    y_val.push_back(((*iter).second).fMainDetectorAverage.mean*1e9);
    y_err.push_back(((*iter).second).fMainDetectorAverage.error*1e9);
    fRes.AddLine();
    fRes.AddFloatEntry((*iter).first);
    fRes.AddFloatEntry(((*iter).second).fMainDetectorAverage.mean*1e9);
    fRes.AddFloatEntry(((*iter).second).fMainDetectorAverage.error*1e9);
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
				  
