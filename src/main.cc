/**********************************   
PREX-II Statistics Toolkit
	author: Tao Ye	<tao.ye@stonybrook.edu>
	Last update: Aug 2020
************************************/
#include "TaConfig.hh"
#include "TaRunInfo_v2.hh"
#include "TaResult.hh"
#include "TaStatBuilder.hh"
#include "UserFunction.cc"

#include "TFile.h"

int main(int argc, char** argv){
  if( argc == 1)
    return 1;
  int opt;
  TString confFileName;
  while( (opt=getopt(argc,argv,":c:"))!=-1){
    switch(opt){
    case ':':
      cerr << argv[optind-1] << "requires value. " <<endl;
      return 1;
    case '?':
      cerr << "Unknown arguments " <<optopt << endl;
      return 1;
    case 'c':
      confFileName=TString(optarg);
      break;
    }
  }
  
  TaConfig *fConfig= new TaConfig();
  fConfig->ParseFile(confFileName);
  TString output_tag  = fConfig->GetParameter("output_tag");
  TString output_suffix  = fConfig->GetParameter("output_suffix");
  TString plot_dir = fConfig->GetParameter("plot_dir");
  
  TString input_path = fConfig->GetParameter("input_path");
  TString input_prefix = fConfig->GetParameter("input_prefix");
  TString input_suffix = fConfig->GetParameter("input_suffix");
  vector<TString> fTreeList = fConfig->GetParameterList("input_tree");
  TString run_counter = fConfig->GetParameter("run_counter");
  TString minirun_counter =fConfig->GetParameter("minirun_counter");
  
  TString sign_correction = fConfig->GetParameter("sign_correction");
  Bool_t kSignCorrection = kFALSE;
  if(sign_correction=="true")
    kSignCorrection = kTRUE;

  vector<TString> maindet_switch = fConfig->GetParameterList("maindet_switch");
  TString maindet_weighted = fConfig->GetParameter("maindet_weighted");
  Bool_t kWeighted = kFALSE;
  if(maindet_weighted=="true")
    kWeighted = kTRUE;
  
  vector<TString> fDeviceNameList;
  if( (fConfig->GetParameter("device_list"))!="") {
    fDeviceNameList = fConfig->GetParameterList("device_list");
  }else{
    for(int islug=1;islug<=94;islug++){
      TString file_name = Form("%s/%sslug%d%s.root",
			       input_path.Data(),
			       input_prefix.Data(),islug,input_suffix.Data());
      TFile* input_file = TFile::Open(file_name);
      if(input_file==NULL){
	cerr <<  file_name << " is not found and is skipped" << endl;
	continue;
      }
      Int_t nTree = fTreeList.size();
      for(int itree=0;itree<nTree;itree++){
	if( ( input_file->Get(fTreeList[itree] ))!=NULL){
	  TTree* fTree = (TTree*)input_file->Get(fTreeList[itree] );
	  vector<TString> fBranchList = ExtractBranchList(fTree);
	  UnionBranchList( fDeviceNameList , fBranchList);
	}
      }
      input_file->Close();
    } // end of loop over slugs
  }
  cout << " -- Device List " << endl;
  Int_t nDev = fDeviceNameList.size();
  for(int idev=0;idev<nDev;idev++){
    cout<< " \t \t " << fDeviceNameList[idev]  << endl;
  }
  map< Int_t, TaRunInfo > fRunInfoMap = LoadRunInfoMap();
  map<SLUG_ARM,Int_t> fPittMap = LoadPittsMap();
  map<SLUG_ARM,pair<TString,TString> > fSlugInfoMap;
  map<SLUG_ARM,Int_t > fSlugSignMap;

  map<SLUG_ARM, TaStatBuilder > fSlugStatBuilderMap;
  map<Int_t, TaStatBuilder > fPittsStatBuilderMap;
  map<Int_t, TaStatBuilder > fWienStatBuilderMap;
  
  map<TString,StatData> fChannelMap;
  
  for(int islug=1;islug<=94;islug++){
    TString file_name = Form("%s/%sslug%d%s.root",
			     input_path.Data(),
			     input_prefix.Data(),islug,input_suffix.Data());
    TFile* input_file = TFile::Open(file_name);
    if(input_file==NULL){
      cerr <<  file_name << " is not found and is skipped" << endl;
      continue;
    }
    
    TTree* mini_tree = (TTree*)input_file->Get(fTreeList[0]);
    if(mini_tree==NULL){
      cerr << " Error: " << fTreeList[0]
	   <<" Tree not found in "
	   << file_name << endl;
      continue;
    }
    Int_t nTree = fTreeList.size();
    for(int i=1;i<nTree;i++){
      cout << " -- Loading " << fTreeList[i] << endl;
      mini_tree->AddFriend(fTreeList[i]);
    }

    Int_t nEntries = mini_tree->GetEntries();
    Int_t run_number=0;
    Int_t mini_id=0;
    Int_t last_run_number=0;
    mini_tree->SetBranchAddress(run_counter,&run_number);
    mini_tree->SetBranchAddress(minirun_counter,&mini_id);
    RegisterBranchesAddress(mini_tree,fDeviceNameList,fChannelMap);
    TaRunInfo myRunInfo;
    SLUG_ARM myKey;
    TString detName = "";
    TString bcmName =fConfig->GetParameter("normalizing_bcm");
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
	last_run_number = run_number;
	if(maindet_switch.size()==3)
	  detName  = maindet_switch[ myRunInfo.GetArmFlag() ];
	
	if(fSlugStatBuilderMap.find(myKey)==fSlugStatBuilderMap.end()){
	  TaStatBuilder fStatBuilder;
	  fSlugStatBuilderMap.insert(make_pair(myKey,fStatBuilder));
	  fSlugInfoMap[myKey] = make_pair(myRunInfo.GetIHWPStatus(),
					  myRunInfo.GetWienMode());
	  fSlugSignMap[myKey] = myRunInfo.GetSign();
	}
      } // end if it is a new run number
      if(myRunInfo.GetRunFlag()=="Good"){
	fSlugStatBuilderMap[myKey].SetLabel(Form("slug%d.%d",run_number,mini_id));
	if(detName!=""){
	  fSlugStatBuilderMap[myKey].UpdateStatData("Adet",fChannelMap[detName]);
	  if(kWeighted)
	    fSlugStatBuilderMap[myKey].UpdateWeightingError(fChannelMap[detName]);
	}
	if(bcmName!="")
	  fSlugStatBuilderMap[myKey].UpdateStatData("Aq",fChannelMap[bcmName]);

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
	    if((*iter_dev).Contains("sr")
	       || (*iter_dev).Contains("tr")
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
  TString output_filename = Form("prex_grand_average_%s%s.root",
				 output_tag.Data(),output_suffix.Data());
  TFile *output_rf =  TFile::Open(output_filename,"RECREATE");
  TTree *fSlugTree = new TTree("slug","Slug Averages");
  Double_t fSlugID;
  Double_t fArmSlug;
  Double_t fSign;
  TBranch *fBranchSlug = fSlugTree->Branch("slug",&fSlugID);
  TBranch *fBranchArm = fSlugTree->Branch("arm_flag",&fArmSlug);
  TBranch *fBranchSign = fSlugTree->Branch("sign",&fSign);
  TaResult fSlugLog_beamline("average_by_slug_"+output_tag+output_suffix+".log");
  auto iter_slug = fSlugStatBuilderMap.begin();
  Int_t wienID = -1;
  TString last_wien_state="";
  vector<TString> wien_string;
  while(iter_slug!=fSlugStatBuilderMap.end()){
    fSlugID = ((*iter_slug).first).first;
    TString slug_label = Form("Slug%.0f",fSlugID);
    fArmSlug = ((*iter_slug).first).second;
    fSlugID += fArmSlug/10.0;
    if(fArmSlug==1)
      slug_label+="R";
    else if(fArmSlug==2)
      slug_label+="L";
    fSign = fSlugSignMap[(*iter_slug).first];
    Double_t mySpin = 1.0;
    if(kSignCorrection)
      mySpin=fSign;
    
    (*iter_slug).second.PullFitAllChannels(plot_dir+output_tag+"_"+slug_label+output_suffix+".pdf");
    (*iter_slug).second.FillTree(fSlugTree);
    fSlugTree->Fill();
    Int_t pittsID = fPittMap[(*iter_slug).first];
    TString IHWP_state = (fSlugInfoMap[(*iter_slug).first]).first;
    TString Wien_state = (fSlugInfoMap[(*iter_slug).first]).second;
    
    if(pittsID!=-1){
      fPittsStatBuilderMap[pittsID].SetLabel(slug_label);
      fPittsStatBuilderMap[pittsID].UpdateStatBuilderByIHWP( (*iter_slug).second,
							     IHWP_state,
							     mySpin);
    }
    if(Wien_state!=last_wien_state){
      wien_string.push_back(Wien_state);
      wienID++;
      last_wien_state = Wien_state;
    }
    cout << " Fill Wien StatBuilder with " << slug_label << endl;
    fWienStatBuilderMap[wienID].SetLabel(slug_label);
    fWienStatBuilderMap[wienID].UpdateStatBuilderByIHWP( (*iter_slug).second,
							 IHWP_state,
							 mySpin);
    iter_slug++;
  }
  
  TaStatBuilder fPittsStatBuilder;
  TaStatBuilder fPittsNullStatBuilder;
  map<Int_t,TaStatBuilder> fPittsNullStatMap;
  TTree *fPittsTree = new TTree("pitt","Pitts Averages");
  Int_t fPittsID;
  fPittsTree->Branch("pitt",&fPittsID);
  auto iter_pitts = fPittsStatBuilderMap.begin();
  while(iter_pitts!=fPittsStatBuilderMap.end()){
    fPittsID = (*iter_pitts).first;
    fPittsStatBuilder.SetLabel(Form("Pitt%d",fPittsID));
    (*iter_pitts).second.PullFitAllChannels(plot_dir+output_tag+Form("_Pitt%d",fPittsID)+output_suffix+".pdf");
    fPittsStatBuilder.UpdateStatBuilder((*iter_pitts).second);
    ((*iter_pitts).second).FillTree(fPittsTree);
    
    TaStatBuilder fNullStat =(*iter_pitts).second.GetNullStatBuilder();
    fPittsNullStatMap[fPittsID] = fNullStat;
    fPittsNullStatBuilder.SetLabel(Form("Pitt%d",fPittsID));
    fPittsNullStatBuilder.UpdateStatBuilder(fNullStat);
    fNullStat.FillTree(fPittsTree,"null_");
    fPittsTree->Fill();
    iter_pitts++;
  }
  fPittsStatBuilder.PullFitAllChannels(output_tag+"_pitts_pullfit"+output_suffix+".pdf");
  fPittsNullStatBuilder.PullFitAllChannels(output_tag+"_pitts_null_pullfit"+output_suffix+".pdf");

  TaResult fPittLog_beamline("average_by_pitt_"+output_tag+output_suffix+".log");
  
  TaStatBuilder fWienStatBuilder;
  map<Int_t,TaStatBuilder> fWienNullStateMap;
  TTree *fWienTree = new TTree("wien","Wien Averages");
  Int_t fWienID;
  fWienTree->Branch("wien",&fWienID);
  auto iter_wien = fWienStatBuilderMap.begin();
  while(iter_wien!=fWienStatBuilderMap.end()){
    fWienID = (*iter_wien).first;
    cout << fWienID << endl;
    fWienStatBuilder.SetLabel( wien_string[fWienID] );
    (*iter_wien).second.PullFitAllChannels(Form("./plots/%s_Wien_%s.pdf",
						output_tag.Data(),
						wien_string[fWienID].Data()));
    (*iter_wien).second.PullFitAllChannelsByIHWP("buff.pdf");
    
    fWienStatBuilder.UpdateStatBuilder((*iter_wien).second);
    (*iter_wien).second.FillTree(fWienTree);
    
    TaStatBuilder fNullStat = (*iter_wien).second.GetNullStatBuilder();
    fWienNullStateMap[fWienID] = fNullStat;
    fNullStat.FillTree(fWienTree,"null_");
    fWienTree->Fill();
    iter_wien++;
  }
  
  TaResult fWienLog_beamline("average_by_wien_"+output_tag+output_suffix+".log"); 
  fWienStatBuilder.PullFitAllChannels(output_tag+"_wien_pullfit"+output_suffix+".pdf");
  fWienStatBuilder.ReportLogByIHWP(fWienLog_beamline);
  TTree *fGrandTree = new TTree("grand", "Grand Averages");
  fWienStatBuilder.FillTree(fGrandTree);
  fGrandTree->Fill();
  
  fWienTree->Write();
  fPittsTree->Write();
  fSlugTree->Write();
  fGrandTree->Write();
  output_rf->Close();
  
  return 0;
}