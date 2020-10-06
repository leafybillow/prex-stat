/**********************************   
PREX-II Statistics Calculator
	author: Tao Ye	<tao.ye@stonybrook.edu>
	since August 2020
************************************/
#include "TaConfig.hh"
#include "TaRunInfo_v2.hh"
#include "TaResult.hh"
#include "TaStatBuilder.hh"
#include "UserFunction.cc"

#include "TFile.h"
#include "TError.h"
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
  gErrorIgnoreLevel = kError;
  
  TaConfig *fConfig= new TaConfig();
  fConfig->ParseFile(confFileName);
  TString format_text = fConfig->GetParameter("output_format") ;
  const char* output_format = (const char*)format_text;
  cout << "output_format : " << output_format << endl;
  
  TString plot_dir = fConfig->GetParameter("plot_dir");
  TString project_dir = fConfig->GetParameter("project_dir");
  cout << "project_dir : " << project_dir << endl;
  TString input_path = fConfig->GetParameter("input_path");

  TString input_format_text = fConfig->GetParameter("input_format");
  const char*  input_format = (const char*)input_format_text;
  cout << "input_format : " << input_format << endl;

  vector<TString> fTreeList = fConfig->GetParameterList("input_tree");

  TString friend_format = fConfig->GetParameter("friend_format");
  vector<TString> fFriendTreeList = fConfig->GetParameterList("friend_tree");
  TString run_counter = fConfig->GetParameter("run_counter");
  TString minirun_counter =fConfig->GetParameter("minirun_counter");
  
  TString sign_correction = fConfig->GetParameter("sign_correction");
  Bool_t kSignCorrection = kFALSE;
  if(sign_correction=="true")
    kSignCorrection = kTRUE;

  vector<TString> maindet_switch = fConfig->GetParameterList("maindet_switch");
  vector<TString> rawAdet_switch = fConfig->GetParameterList("rawAdet_switch");
  TString maindet_weighted = fConfig->GetParameter("maindet_weighted");
  Bool_t kWeighted = kFALSE;
  if(maindet_weighted=="true")
    kWeighted = kTRUE;

  Int_t slug_begin = (fConfig->GetParameter("slug_begin")).Atoi();
  Int_t slug_end = (fConfig->GetParameter("slug_end")).Atoi();
  if( slug_begin == 0)
    slug_begin = 1;
  if(slug_end ==0 )
    slug_end = 94;
  
  vector<TString> fDeviceNameList;
  if( (fConfig->GetParameter("device_list"))!="") {
    fDeviceNameList = fConfig->GetParameterList("device_list");
  }else{ // otherwise Union all channels into grand averages 
    for(int islug=slug_begin;islug<=slug_end;islug++){
      TString file_name = Form(input_format,islug);
      TFile* input_file = TFile::Open(input_path+file_name);
      if(input_file==NULL){
	cerr <<  input_path+file_name << " is not found and is skipped" << endl;
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
  
  vector<TString> fIVnames = fConfig->GetParameterList("iv_list");
  vector<TString> fDVnames = fConfig->GetParameterList("dv_list");
  vector<TString> dv_switch = fConfig->GetParameterList("dv_switch");

  Int_t nDV = fDVnames.size();
  Int_t nIV = fIVnames.size();
  map<TString , Double_t > fSlopeMap;
  vector<TString> fSlopeNames(nDV*nIV);
  for(int iiv=0;iiv<nIV;iiv++){
    if( find(fDeviceNameList.begin(),fDeviceNameList.end(), "diff_" + fIVnames[iiv] ) == fDeviceNameList.end())
      fDeviceNameList.push_back( "diff_"+fIVnames[iiv] ) ;
  }
  for(int idv=0;idv<nDV;idv++){
    for(int iiv=0;iiv<nIV;iiv++){
      fDeviceNameList.push_back( Form("corr_%s_%s",fDVnames[idv].Data(),fIVnames[iiv].Data()));
      fSlopeNames[idv*nIV+iiv]= Form("%s_%s",fDVnames[idv].Data(),fIVnames[iiv].Data());
      fSlopeMap[ fSlopeNames[idv*nIV+iiv] ] = 0.0;
    }
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

  map<SLUG_ARM, TaStatBuilder* > fSlugStatBuilderMap;
  map<Int_t, TaStatBuilder* > fPittsStatBuilderMap;
  map<Int_t,TaStatBuilder*> fPittsNullStatBuilderMap;
  map<Int_t, TaStatBuilder* > fWienStatBuilderMap;
  map<Int_t, TaStatBuilder* > fWienNullStatBuilderMap;

  map<TString,StatData> fChannelMap;

  for(int islug=slug_begin;islug<=slug_end;islug++){
    TString file_name = Form(input_format,islug);
    TFile* input_file = TFile::Open(input_path+file_name);
    if(input_file==NULL){
      cerr <<  " ++ " << input_path+file_name << " is not found and is skipped" << endl;
      continue;
    }
    TTree* mini_tree = (TTree*)input_file->Get(fTreeList[0]);
    if(mini_tree==NULL){
      cerr << " ++  Error: " << fTreeList[0]
	   <<" Tree not found in "
	   << file_name << endl;
      continue;
    }
    Int_t nTree = fTreeList.size();
    for(int i=1;i<nTree;i++){
      mini_tree->AddFriend(fTreeList[i]);
    }
    TFile* friend_file;
    if(friend_format!=""){
      cout << "Opening friend file:" 
	   << Form(friend_format.Data(),islug) << endl;
      friend_file = TFile::Open(Form((const char*)friend_format,islug));
      Int_t nFriendTree = fFriendTreeList.size();
      for(int i=0;i<nFriendTree;i++){
	TTree *friend_tree = (TTree*)friend_file->Get(fFriendTreeList[i]);
	if(friend_tree!=NULL)
	  mini_tree->AddFriend(friend_tree);
      }
    }else
      friend_file = 0;
    Int_t nEntries = mini_tree->GetEntries();
    Int_t run_number=0;
    Int_t mini_id=0;
    Int_t last_run_number=0;
    mini_tree->SetBranchAddress(run_counter,&run_number);
    mini_tree->SetBranchAddress(minirun_counter,&mini_id);
    RegisterBranchesAddress(mini_tree,fDeviceNameList,fChannelMap);
    for(int idv =0; idv<nDV;idv++){
      for(int iiv=0; iiv < nIV;iiv++){
	if( mini_tree->GetBranch(fSlopeNames[idv*nIV+iiv])==NULL){
	  cout << " ++ slope " << fSlopeNames[idv*nIV+iiv] << " is not found" << endl;
	  fSlopeMap[fSlopeNames[idv*nIV+iiv]] = 0.0;
	}
	else
	  mini_tree->SetBranchAddress( fSlopeNames[idv*nIV+iiv],
				       &fSlopeMap[fSlopeNames[idv*nIV+iiv]]);
      }
    }
    TaRunInfo myRunInfo;
    SLUG_ARM myKey;
    TString detName = "";
    TString rawAdet = "";
    TString bcmName =fConfig->GetParameter("normalizing_bcm");
    // -- Start here
    for(int ievt=0;ievt<nEntries;ievt++){
      mini_tree->GetEntry(ievt);
      if(fRunInfoMap.find(run_number)==fRunInfoMap.end()){
	cerr << "-- run info not found for run  "
	     << run_number << " and will skip "  << endl;
	continue;
      }
      
      // Scale Correction StatData from ivs
      for(int idv=0;idv<nDV;idv++){
	for(int iiv=0;iiv<nIV;iiv++){
	  TString slope_name = fSlopeNames[idv*nIV+iiv];
	  TString channel_name = "corr_"+slope_name;
	  TString iv_channel = "diff_"+fIVnames[iiv];
	  fChannelMap[channel_name] =  fChannelMap[iv_channel].MultiplyBySlope(fSlopeMap[slope_name]);
	}
      }
      if(run_number!=last_run_number){
	myRunInfo = fRunInfoMap[run_number];
	myKey = make_pair(myRunInfo.GetSlugNumber(),
			  myRunInfo.GetArmFlag());
	last_run_number = run_number;
	if(maindet_switch.size()==3)
	  detName  = maindet_switch[ myRunInfo.GetArmFlag() ];
	if(rawAdet_switch.size()==3)
	  rawAdet  = rawAdet_switch[ myRunInfo.GetArmFlag() ];
	
	if(fSlugStatBuilderMap.find(myKey)==fSlugStatBuilderMap.end()){
	  TaStatBuilder *fStatBuilder = new TaStatBuilder();
	  fSlugStatBuilderMap.insert(make_pair(myKey,fStatBuilder));
	  fSlugInfoMap[myKey] = make_pair(myRunInfo.GetIHWPStatus(),
					  myRunInfo.GetWienMode());
	  fSlugSignMap[myKey] = myRunInfo.GetSign();
	}
      } // end if it is a new run number
      
      if(myRunInfo.GetRunFlag()=="Good"){
	fSlugStatBuilderMap[myKey]->SetLabel(Form("%d.%d",run_number,mini_id));
	if(detName!=""){
	  if(kWeighted)
	    fSlugStatBuilderMap[myKey]->UpdateWeightingError(fChannelMap[detName]);
	  fSlugStatBuilderMap[myKey]->UpdateStatData("Adet",fChannelMap[detName]);
	  if(rawAdet!="")  
	    fSlugStatBuilderMap[myKey]->UpdateStatData("AdetRaw",fChannelMap[rawAdet]);

	  if(dv_switch.size()==3){
	    TString dvName  = dv_switch[ myRunInfo.GetArmFlag() ];
	    for(int iiv=0;iiv<nIV;iiv++){
	      TString slope_name = dvName +"_"+fIVnames[iiv];
	      TString iv_channel = "diff_"+fIVnames[iiv];
	      StatData crtn =  fChannelMap[iv_channel].MultiplyBySlope(fSlopeMap[slope_name]);
	      fSlugStatBuilderMap[myKey]->UpdateStatData("corr_Adet_"+fIVnames[iiv],
							 crtn);
	    }
	  }
	}
	if(bcmName!="")
	  fSlugStatBuilderMap[myKey]->UpdateStatData("Aq",fChannelMap[bcmName]);

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
	  fSlugStatBuilderMap[myKey]->UpdateStatData(*iter_dev,fChannelMap[*iter_dev]);
	  iter_dev++;
	}
      }else
	cout << " ** run " << run_number << " is not a good run " << endl;
    } // end of minirun loop inside a slug
    input_file->Close();
    if(friend_file!=0)
      friend_file->Close();
  }// end of slug loop
  TString output_filename = Form(output_format, "grand_average","root");
  TFile *output_rf =  TFile::Open(project_dir+output_filename,"RECREATE");
  TTree *fSlugTree = new TTree("slug","Slug Averages");
  Double_t fSlugID;
  Double_t fArmSlug;
  Double_t fSign;
  fSlugTree->Branch("slug",&fSlugID);
  fSlugTree->Branch("arm_flag",&fArmSlug);
  fSlugTree->Branch("sign",&fSign);
  TString fIHWP_cstr;
  TString fWienFlip_cstr;
  fSlugTree->Branch("ihwp",&fIHWP_cstr);
  fSlugTree->Branch("wien",&fWienFlip_cstr);

  auto iter_slug = fSlugStatBuilderMap.begin();
  Int_t wienID = -1;
  TString last_wien_state="";
  vector<TString> wien_string;
  TaStatBuilder fSlugStatBuilder;
  TaResult fSlugLog(project_dir+Form(output_format,
				     "average_by_slug","log"));
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
    Double_t mySpin = 1.0;
    if(kSignCorrection)
      mySpin=fSign;

    fIHWP_cstr = fSlugInfoMap[(*iter_slug).first].first;
    fWienFlip_cstr = fSlugInfoMap[(*iter_slug).first].second;

    (*iter_slug).second->SetPlotFileName(plot_dir+Form(output_format,
						       ("slug"+slug_label).Data(),
						       "pdf"));
    if(!kWeighted){
      (*iter_slug).second->PullFitAllChannels();
      (*iter_slug).second->FillTree(fSlugTree);
      fSlugTree->Fill();
    }
    fSlugStatBuilder.SetLabel(slug_label);
    fSlugStatBuilder.UpdateStatBuilder((*iter_slug).second,mySpin);
    iter_slug++;
  }

  if(kWeighted)
    fSlugStatBuilder.RescaleErrorBar();
  fSlugStatBuilder.ReloadChi2NDF();
  fSlugStatBuilder.PullFitAllChannels(project_dir+Form(output_format,
						       "slugs_pullfit","pdf"));
  fSlugStatBuilder.ReportLog(fSlugLog);
  // Builder Pitts and Wiens Statbuilders
  iter_slug = fSlugStatBuilderMap.begin();
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
    Double_t mySpin = 1.0;
    if(kSignCorrection)
      mySpin=fSign;

    fIHWP_cstr = fSlugInfoMap[(*iter_slug).first].first;
    fWienFlip_cstr = fSlugInfoMap[(*iter_slug).first].second;

    if(kWeighted){
      (*iter_slug).second->FillTree(fSlugTree);
      fSlugTree->Fill();
    }
    Int_t pittsID = fPittMap[(*iter_slug).first];
    TString IHWP_state = (fSlugInfoMap[(*iter_slug).first]).first;
    TString Wien_state = (fSlugInfoMap[(*iter_slug).first]).second;
    if(pittsID!=-1){
      if(fPittsStatBuilderMap.find(pittsID)==fPittsStatBuilderMap.end()){
	TaStatBuilder *fStatBuilder  = new TaStatBuilder();
	fPittsStatBuilderMap[pittsID] = fStatBuilder;
      }
      fPittsStatBuilderMap[pittsID]->SetLabel(slug_label);
      fPittsStatBuilderMap[pittsID]->UpdateStatBuilderByIHWP((*iter_slug).second,
							     IHWP_state,
							     mySpin);
      if(fPittsNullStatBuilderMap.find(pittsID)==fPittsNullStatBuilderMap.end()){
	TaStatBuilder *fStatBuilder  = new TaStatBuilder();
	fPittsNullStatBuilderMap[pittsID] = fStatBuilder;
      }
      fPittsNullStatBuilderMap[pittsID]->SetLabel(slug_label);
      fPittsNullStatBuilderMap[pittsID]->UpdateStatBuilderByIHWP((*iter_slug).second,
								 IHWP_state,1);  // no sign correction

    }
    if(Wien_state!=last_wien_state){
      wien_string.push_back(Wien_state);
      wienID++;
      last_wien_state = Wien_state;
    }
    cout << " -- Fill Wien StatBuilder with slug " << slug_label << endl;
    if(fWienStatBuilderMap.find(wienID)==fWienStatBuilderMap.end()){
      TaStatBuilder *fStatBuilder = new TaStatBuilder();
      fWienStatBuilderMap[wienID] = fStatBuilder;
    }
    fWienStatBuilderMap[wienID]->SetLabel(slug_label);
    fWienStatBuilderMap[wienID]->UpdateStatBuilderByIHWP((*iter_slug).second,
							 IHWP_state,
							 mySpin);

    if(fWienNullStatBuilderMap.find(wienID)==fWienNullStatBuilderMap.end()){
      TaStatBuilder *fStatBuilder = new TaStatBuilder();
      fWienNullStatBuilderMap[wienID] = fStatBuilder;
    }
    fWienNullStatBuilderMap[wienID]->SetLabel(slug_label);
    fWienNullStatBuilderMap[wienID]->UpdateStatBuilderByIHWP((*iter_slug).second,
							    IHWP_state,1); // no signed

    iter_slug++;
  }
  // END of building pitts and wien stats 
  TaStatBuilder fPittsStatBuilder;
  TaStatBuilder fPittsNullStatBuilder;

  TTree *fPittsTree = new TTree("pitt","Pitts Averages");
  Int_t fPittsID;
  fPittsTree->Branch("pitt",&fPittsID);
  auto iter_pitts = fPittsStatBuilderMap.begin();
  while(iter_pitts!=fPittsStatBuilderMap.end()){
    fPittsID = (*iter_pitts).first;
    fPittsStatBuilder.SetLabel(Form("%d",fPittsID));
    TString pitt_label = Form("Pitt%d",fPittsID);
    (*iter_pitts).second->PullFitAllChannels(plot_dir+Form(output_format,
							   pitt_label.Data(),"pdf"));
    fPittsStatBuilder.UpdateStatBuilder( (*iter_pitts).second );
    ((*iter_pitts).second)->FillTree(fPittsTree);
    
    TaStatBuilder* fNullStat =fPittsNullStatBuilderMap[fPittsID]->GetNullStatBuilder();
    fPittsNullStatBuilder.SetLabel(Form("%d",fPittsID));
    fPittsNullStatBuilder.UpdateStatBuilder(fNullStat);
    fNullStat->FillTree(fPittsTree,"null_");
    
    fPittsTree->Fill();
    iter_pitts++;
  }

  fPittsStatBuilder.PullFitAllChannels(project_dir+Form(output_format,"pitts_pullfit","pdf"));
  fPittsNullStatBuilder.PullFitAllChannels(project_dir+Form(output_format,"pitts_null_pullfit","pdf"));

  TaResult fPittLog(project_dir+Form(output_format,"average_by_pitt","log"));
  fPittsStatBuilder.ReportLog(fPittLog);
  TaResult fPittLog_null(project_dir+Form(output_format,"null_by_pitt","log"));
  fPittsNullStatBuilder.ReportLogByIHWP(fPittLog_null);
  
  TaStatBuilder fWienStatBuilder;
  TaStatBuilder fWienNullStatBuilder;

  TTree *fWienTree = new TTree("wien","Wien Averages");
  Int_t fWienID;
  fWienTree->Branch("wien",&fWienID);
  auto iter_wien = fWienStatBuilderMap.begin();
  while(iter_wien!=fWienStatBuilderMap.end()){
    fWienID = (*iter_wien).first;
    fWienStatBuilder.SetLabel( wien_string[fWienID] );
    TString wien_label = Form("Wien%d_%s",fWienID,wien_string[fWienID].Data());
    (*iter_wien).second->PullFitAllChannels(plot_dir+Form(output_format,
							  wien_label.Data(),"pdf"));
    (*iter_wien).second->PullFitAllChannelsByIHWP();
    fWienStatBuilder.UpdateStatBuilder((*iter_wien).second);
    (*iter_wien).second->FillTree(fWienTree);
    
    TaStatBuilder* fNullStat = fWienNullStatBuilderMap[fWienID]->GetNullStatBuilder(kFALSE);
    fWienNullStatBuilder.SetLabel( wien_string[fWienID] );
    fWienNullStatBuilder.UpdateStatBuilder(fNullStat);
    fNullStat->FillTree(fWienTree,"null_");
    fWienTree->Fill();
    iter_wien++;
  }

  fWienStatBuilder.PullFitAllChannels(project_dir+Form(output_format,
						       "wien_pullfit","pdf"));
  fWienNullStatBuilder.PullFitAllChannels(project_dir+Form(output_format,
							   "wien_null_pullfit","pdf"));

  TaResult fWienLog(project_dir+Form(output_format,"average_by_wien","log"));
  fWienStatBuilder.ReportLogByIHWP(fWienLog);
  TaResult fWienLog_null(project_dir+Form(output_format,"null_by_wien","log"));
  fWienNullStatBuilder.ReportLogByIHWP(fWienLog_null);

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
