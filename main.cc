#include "LoadNormalizationMap.C"
#include "LoadPittsMap.C"
#include "lib/TaRunInfo_v2.cc"
#include "lib/TaAccumulator.cc"
#include "lib/TaStatBuilder.cc"

void main(){
  TString fDetectorNameList[]={"reg_asym_us_avg","reg_asym_usr","reg_asym_usl",
			       "reg_asym_us_dd",
			       "reg_asym_left_dd","reg_asym_right_dd"};
  
  TString fRawDetectorNameList[]={"asym_us_avg","asym_usr","asym_usl",
				  "asym_us_dd",
				  "asym_left_dd","asym_right_dd"};
  
  TString fBPMNameList[]={"diff_bpm4aX","diff_bpm4eX",
			  "diff_bpm4aY","diff_bpm4eY",
			  "diff_bpm11X","diff_bpm12X"};
  
  TString fBCMNameList[]={"asym_bcm_an_us","asym_bcm_an_ds",
			  "asym_bcm_an_ds3","asym_bcm_an_ds10",
			  "asym_bcm_dg_us","asym_bcm_dg_ds"};

  Int_t nBPM = sizeof(fBPMNameList)/sizeof(*fBPMNameList);
  Int_t nBCM = sizeof(fBPMNameList)/sizeof(*fBCMNameList);
  Int_t nDet = sizeof(fDetectorNameList)/sizeof(*fDetectorNameList);
  
  LoadNormalizationMap();
  map<SLUG_ARM,Int_t> fPittMap = LoadPittsMap();
  map<SLUG_ARM, TaStatBuilder > fSlugStatBuilderMap;
  map< Int_t, TaRunInfo > fRunInfoMap = LoadRunInfoMap();
  
  vector<StatData> fRawDetectorStatDataArray(nDet);
  vector<StatData> fDetectorStatDataArray(nDet);
  vector<StatData> fBPMStatDataArray(nBPM);
  vector<StatData> fBCMStatDataArray(nBCM);
  
  auto iter_slug = fSlugMap.begin();
  while(iter_slug!=fSlugMap.end()){
    Int_t mySlug = ((*iter_slug).first).first;
    Int_t myArmFlag = ((*iter_slug).first).second;
    vector<Int_t> runlist = (*iter_slug).second;
    TString file_name = Form("./rootfiles/postpan/prexPrompt_slug%d.root",mySlug);
    TFile* input_file = TFile::Open(file_name);
    if(input_file==NULL){
      cout <<  file_name << " is not found and is skipped" << endl;
      iter_slug++;
      continue;
    }
    TTree* mini_tree = (TTree*)input_file->Get("T");
    if(mini_tree==NULL){
      cout << " Error: Mini Tree not found in "
	   << input_file->GetName() << endl;
      iter_slug++;
      continue;
    }
    Int_t nEntries = mini_tree->GetEntries();
    Int_t run_number;
    mini_tree->SetBranchAddress("run",&run_number);
    for(int idet=0;idet<nDet;idet++){
      mini_tree->SetBranchAddress(fDetectorNameList[idet],&fDetectorStatDataArray[idet]);
      mini_tree->SetBranchAddress(fRawDetectorNameList[idet],&fRawDetectorStatDataArray[idet]);
    }
    for(int ibpm=0;ibpm<nBPM;ibpm++)
      mini_tree->SetBranchAddress(fBPMNameList[ibpm],&fBPMStatDataArray[ibpm]);
    for(int ibcm=0;ibcm<nBCM;ibcm++)
      mini_tree->SetBranchAddress(f
    for(int ievt=0;ievt<nEntries;ievt++){
      mini_tree->GetEntry(ievt);
      
    } // end of run loop inside a slug
    input_file->Close();
    iter_slug++;
  }// end of slug loop 
  
}
  
