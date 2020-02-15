#include "lib/TaAccumulator.cc"
#include "lib/TaRunInfo_v2.cc"
#include "LoadNormalizationMap.C"
#include "LoadPittsMap.C"
#include "lib/PlotPullFit.C"
#include "lib/TaResult.cc"

void MergeMiniTreeWithInfo(){
  
  TStopwatch tsw;
  TString postpan_dir="./rootfiles/postpan/";

  TFile *output = TFile::Open("./rootfiles/prex_merge_postpan.root","RECREATE");
  TTree* output_tree = new TTree("mini","mini");
  TList* list = new TList();

  for(int i=1;i<=94;i++){
    TFile *this_file = TFile::Open(postpan_dir+
				   Form("prexPrompt_slug%d.root",
					i));
    cout << this_file->GetName() << endl;
    if(this_file!=NULL){
      TTree *this_tree = (TTree*)this_file->Get("T");
      if(this_tree!=NULL)
	list->Add(this_tree);
    }
  }
  output->cd();
  output_tree = TTree::MergeTrees(list);
  
  map< Int_t, TaRunInfo > fRunInfoMap;  
  
  TString info_filename = "./all_production_target.list";
  ifstream slug_info;
  slug_info.open(info_filename.Data());

  if(!slug_info.is_open()){
    cout << " slug info " << info_filename << " does not exist." << endl;
    return;
  }
  TString sline;    
  while(sline.ReadLine(slug_info)){
    TaRunInfo fRunInfo;
    if(!fRunInfo.ParseLine(sline))
      continue;
    Int_t run_number = fRunInfo.GetRunNumber();
    fRunInfoMap[run_number]=fRunInfo;
  }
  slug_info.close();


  Int_t run_number;
  Int_t slug_number;
  Int_t arm_flag;
  Int_t sign;
  Int_t target_id;
  Bool_t isGood;
  output_tree->SetBranchAddress("run",&run_number);
  TBranch *bArmFlag = output_tree->Branch("arm_flag",&arm_flag);
  TBranch *bSlug = output_tree->Branch("slug",&slug_number);
  TBranch *bGood = output_tree->Branch("isGood",&isGood);
  TBranch *bSign = output_tree->Branch("sign",&sign);
  TBranch *bTarget = output_tree->Branch("target",&target_id);
  Int_t nEntries = output_tree->GetEntries();
  
  for(int ievt=0;ievt<nEntries;ievt++){
    output_tree->GetEntry(ievt);
    TaRunInfo this_info = fRunInfoMap[run_number];

    slug_number  = this_info.GetSlugNumber();
    arm_flag  = this_info.GetArmFlag();
    sign = this_info.GetSign();
    target_id = this_info.GetTargetID();
    if(this_info.GetRunFlag()=="Good" && slug_number!=0)
      isGood = kTRUE;
    else
      isGood = kFALSE;

    bArmFlag->Fill();
    bSlug->Fill();
    bGood->Fill();
    bSign->Fill();
    bTarget->Fill();
  }
  
  output_tree->Write();
  output->Close();

  

}
