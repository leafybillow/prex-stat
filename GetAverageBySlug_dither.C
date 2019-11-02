#include "TaAccumulator.cc"
#include "TaRunStats.cc"
#include "LoadNormalizationMap.C"
#include "PlotPullFit.C"

void GetAverageBySlug_dither(){
  TStopwatch tsw;
  TString detector="dit_asym_us_avg";
  TString raw_detector="asym_us_avg";
  TFile* output = TFile::Open("./prex_dither_grand_fit.root","RECREATE");
  TTree* grand_tree = new TTree("grand","grand summary tree");
  grand_tree->SetMarkerStyle(20);
  Int_t ihwp,wien,spin,slug,arm_flag;
  grand_tree->Branch("ihwp",&ihwp);
  grand_tree->Branch("wien",&wien);
  grand_tree->Branch("spin",&spin);
  grand_tree->Branch("slug",&slug);
  grand_tree->Branch("arm_flag",&arm_flag);
  Double_t fDetectorAvg, fRawDetectorAvg;
  Double_t weighting_error;
  grand_tree->Branch("error",&weighting_error);
  grand_tree->Branch(detector,&fDetectorAvg);
  grand_tree->Branch(raw_detector,&fRawDetectorAvg);

  typedef struct{Double_t ppm,ppb,um,nm;} UNIT;
  UNIT parity_scale;
  parity_scale.ppm=1e-6;
  parity_scale.ppb=1e-9;
  parity_scale.um=1e-3;
  parity_scale.nm=1e-6;
  grand_tree->Branch("unit",&parity_scale,"ppm/D:ppb:um:nm");

  for(int islug=1;islug<=94;islug++){
    slug  = islug;
    TString info_filename = Form("./prex-runlist/slug_info/slug%d_info.list",slug);
    FILE* slug_info = fopen(info_filename.Data(),"r");
    if(slug_info==NULL){
      cout << " slug info does not exist." << endl;
      continue;
    }
    std::map<Int_t,TaRunStats> fRunStatsMap;
    TString wien_str, ihwp_str;
    while(!feof(slug_info)){
      int run_number=0;
      char input[256];
      fscanf(slug_info,"%d,%d,%[^\n]",
	     &run_number,&slug,input);
      if(run_number!=0){
	TaRunStats fRunStats;
	fRunStats.ParseLine(input);
	fRunStats.SetRunNumber(run_number);
	fRunStatsMap[run_number]=fRunStats;
	ihwp_str=fRunStats.GetIHWPStatus();
	wien_str=fRunStats.GetWienMode();
      }
    }
    
    fclose(slug_info);
    TFile* input_file = TFile::Open(Form("./dit/prex_ditsum_slug%d.root",slug));
    if(input_file==NULL){
      cout << " will skip to next one" << endl;
      continue;
    }

    TTree* mini_tree = (TTree*)input_file->Get("dits");
    if(mini_tree==NULL){
      cout << " Error: Mini Tree not found in "
	   << input_file->GetName() << endl;
      continue;
    }
    
    typedef struct{Double_t mean,error,rms;} LEAFLIST;
    LEAFLIST fDetector;
    LEAFLIST fDetectorRaw;
    Bool_t kMatch=kTRUE;
    Int_t run_number;
    Int_t mini_id;
    mini_tree->SetBranchAddress("run",&run_number);
    // mini_tree->SetBranchAddress("mini",&mini_id);
    
    mini_tree->SetBranchAddress(detector+"_mean",&fDetector.mean);
    mini_tree->SetBranchAddress(detector+"_rms",&fDetector.rms);
    mini_tree->SetBranchAddress(detector+"_mean_error",&fDetector.error);

    mini_tree->SetBranchAddress(raw_detector+"_mean",&fDetectorRaw.mean);
    mini_tree->SetBranchAddress(raw_detector+"_rms",&fDetectorRaw.rms);
    mini_tree->SetBranchAddress(raw_detector+"_mean_error",&fDetectorRaw.error);

    Int_t nevt = mini_tree->GetEntries();
    
    Double_t total_weight=0.0;
    fDetectorAvg=0.0;
    fRawDetectorAvg=0.0;
    vector<Double_t> y_val;
    vector<Double_t> y_error;
    vector<Double_t> x_val;
    for(int ievt=0;ievt<nevt;ievt++){
      mini_tree->GetEntry(ievt);

      if(fRunStatsMap[run_number].GetArmFlag()!=0 ||
	 (fRunStatsMap[run_number].GetRunFlag()!="Good"))
	continue;
      
      Int_t bcm_id;
      if(fDetector.error>0){
	double weight = 1.0/pow(fDetector.error,2);
	total_weight+=weight;
	fRawDetectorAvg +=weight*fDetectorRaw.mean;

	fDetectorAvg += weight*fDetector.mean;
	x_val.push_back(run_number);
	y_val.push_back(fDetector.mean);
	y_error.push_back(fDetector.error);
      }

    } // end of minirun loop
    
    if(total_weight>0){
      fDetectorAvg =fDetectorAvg/total_weight;
      fRawDetectorAvg = fRawDetectorAvg/total_weight;
      weighting_error = sqrt(1.0/total_weight);
      cout << "Slug:" << slug ;
      cout << "\t total error(ppb): " << weighting_error*1e9 ;
      cout << "\t Detector Mean(ppb): " << fDetectorAvg*1e9 ;
      cout << endl;
      TString plot_title = Form("%s slug%d pull fit",detector.Data(),slug);
      PlotPullFit(y_val,y_error,x_val,plot_title);
    }
    else{
      weighting_error=0.0;
    }
      
    input_file->Close();
    arm_flag = fRunStatsMap[run_number].GetArmFlag(); 
    if(ihwp_str=="IN")
      ihwp=1;
    else if(ihwp_str=="OUT")
      ihwp=-1;
    else
      ihwp=0;
    if(wien_str=="FLIP-RIGHT")
      wien=1;
    else if(wien_str=="FLIP-LEFT")
      wien=-1;
    else
      wien=0;
    spin = ihwp*wien;
    grand_tree->Fill();
  }
  output->cd();
  grand_tree->Write();
  output->Close();
  tsw.Print();
}

