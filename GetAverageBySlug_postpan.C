#include "lib/TaAccumulator.cc"
#include "lib/TaRunStats.cc"
#include "LoadNormalizationMap.C"
#include "lib/PlotPullFit.C"
#include "lib/TaResult.cc"
void GetAverageBySlug_postpan(){
  TStopwatch tsw;
  std::map<Int_t,Int_t> fBCMRunMap = LoadNormalizationMap();
  TString bcm_array[]={"asym_bcm_an_us","asym_bcm_an_ds",
		       "asym_bcm_an_ds3","asym_bcm_an_ds10",
		       "asym_bcm_dg_us","asym_bcm_dg_ds",
		       "asym_unser"};
  
  TString bpm_array[]={"diff_bpm4aX","diff_bpm4eX",
		       "diff_bpm4aY","diff_bpm4eY",
		       "diff_bpm11X","diff_bpm12X"};
  
  Int_t nBPM = sizeof(bpm_array)/sizeof(*bpm_array);
  Int_t nBCM = sizeof(bcm_array)/sizeof(*bcm_array);
  TString detector="reg_asym_us_avg";
  TString raw_detector="asym_us_avg";
  TFile* output = TFile::Open("./rootfiles/prex_grand_average_postpan.root",
			      "RECREATE");
  TTree* grand_tree = new TTree("grand","grand summary tree");
  grand_tree->SetMarkerStyle(20);
  Int_t ihwp,wien,spin,slug,arm_flag;
  grand_tree->Branch("ihwp",&ihwp);
  grand_tree->Branch("wien",&wien);
  grand_tree->Branch("spin",&spin);
  grand_tree->Branch("slug",&slug);
  grand_tree->Branch("arm_flag",&arm_flag);
  Double_t Aq,fDetectorAvg, fRawDetectorAvg,fBeamCorrection;
  vector<Double_t> fDiffBPM(nBPM);
  vector<Double_t> fBPMCorrection(nBPM);
  grand_tree->Branch("Aq",&Aq);
  Double_t weighting_error;
  grand_tree->Branch("error",&weighting_error);
  grand_tree->Branch(detector,&fDetectorAvg);
  grand_tree->Branch(raw_detector,&fRawDetectorAvg);
  
  for(int ibpm=0;ibpm<nBPM;ibpm++){
    grand_tree->Branch(bpm_array[ibpm],&fDiffBPM[ibpm]);
    TString name_buff = bpm_array[ibpm];
    name_buff.ReplaceAll("diff","cor");
    grand_tree->Branch(name_buff,&fBPMCorrection[ibpm]);
  }


  typedef struct{Double_t ppm,ppb,um,nm;} UNIT;
  UNIT parity_scale;
  parity_scale.ppm=1e-6;
  parity_scale.ppb=1e-9;
  parity_scale.um=1e-3;
  parity_scale.nm=1e-6;
  grand_tree->Branch("unit",&parity_scale,"ppm/D:ppb:um:nm");
  
  TaResult *fReport_main = new TaResult("output/averages_by_slug_maindet.log");
  TaResult *fReport_beam = new TaResult("output/averages_by_slug_beamline.log");
  
  vector<TString> header_main{"Slug","Adet(ppb)", "Error","chi2/NDF"};
  vector<TString> header_beam{"Slug","AQ(ppb)","Abeam",
			      "A4aX","A4eX","A4aY","A4eY","A11X","A12X",
			      "D4aX(nm)","D4eX","D4aY","D4eY","D11X","D12X"};
  fReport_main->AddHeader(header_main);
  fReport_beam->AddHeader(header_beam);
  for(int islug=1;islug<=94;islug++){
    fReport_main->AddLine();
    fReport_main->AddStringEntry(Form("%d",islug));
    fReport_beam->AddLine();
    fReport_beam->AddStringEntry(Form("%d",islug));
    slug  = islug;
    TString info_filename = Form("./runlist/slug%d_info.list",slug);
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
    TFile* input_file = TFile::Open(Form("./rootfiles/postpan/prexPrompt_slug%d.root",slug));
    if(input_file==NULL){
      cout << " will skip to next one" << endl;
      continue;
    }

    TTree* mini_tree = (TTree*)input_file->Get("T");
    if(mini_tree==NULL){
      cout << " Error: Mini Tree not found in "
	   << input_file->GetName() << endl;
      continue;
    }
    
    typedef struct{Double_t mean,error,rms;} LEAFLIST;
    vector<LEAFLIST> fBCMArray(nBCM);
    vector<LEAFLIST> fBPMArray(nBPM);
    vector<Double_t> fSlopes(nBPM);
    LEAFLIST fDetector;
    LEAFLIST fDetectorRaw;
    Bool_t kMatch=kTRUE;
    Int_t run_number;
    Int_t mini_id;
    mini_tree->SetBranchAddress("run",&run_number);
    mini_tree->SetBranchAddress("mini",&mini_id);
    TBranch *det_ptr = mini_tree->GetBranch(detector);
    det_ptr->GetLeaf("mean")->SetAddress(&fDetector.mean);
    det_ptr->GetLeaf("err")->SetAddress(&fDetector.error);
    det_ptr->GetLeaf("rms")->SetAddress(&fDetector.rms);

    TBranch *raw_ptr = mini_tree->GetBranch(raw_detector);
    raw_ptr->GetLeaf("mean")->SetAddress(&fDetectorRaw.mean);
    raw_ptr->GetLeaf("err")->SetAddress(&fDetectorRaw.error);
    raw_ptr->GetLeaf("rms")->SetAddress(&fDetectorRaw.rms);

    for(int ibcm=0;ibcm<nBCM;ibcm++){
      TBranch* this_branch=mini_tree->GetBranch(bcm_array[ibcm]);
      if(this_branch!=NULL){
	this_branch->GetLeaf("mean")->SetAddress(&(fBCMArray[ibcm].mean));
	this_branch->GetLeaf("err")->SetAddress(&(fBCMArray[ibcm].error));
	this_branch->GetLeaf("rms")->SetAddress(&(fBCMArray[ibcm].rms));
      }
      else{
	fBCMArray[ibcm].mean=0.0;
	fBCMArray[ibcm].error=-1.0;
	fBCMArray[ibcm].rms=-1.0;
      }
    }

    for(int ibpm=0;ibpm<nBPM;ibpm++){
      TBranch* this_branch=mini_tree->GetBranch(bpm_array[ibpm]);
      if(this_branch!=NULL){
	this_branch->GetLeaf("mean")->SetAddress(&(fBPMArray[ibpm].mean));
	this_branch->GetLeaf("err")->SetAddress(&(fBPMArray[ibpm].error));
	this_branch->GetLeaf("rms")->SetAddress(&(fBPMArray[ibpm].rms));
      }
      else{
	fBPMArray[ibpm].mean=0.0;
	fBPMArray[ibpm].error=-1.0;
	fBPMArray[ibpm].rms=-1.0;
      }
    }

    for(int ibpm=0;ibpm<nBPM;ibpm++){
      TString raw_name = detector;
      raw_name.ReplaceAll("reg_","");
      mini_tree->SetBranchAddress(raw_name+"_"+bpm_array[ibpm],
				  &fSlopes[ibpm]);
    }
    Int_t nevt = mini_tree->GetEntries();
    
    Double_t total_weight=0.0;
    Aq=0.0;
    fDetectorAvg=0.0;
    fRawDetectorAvg=0.0;
    for(int ibpm=0;ibpm<nBPM;ibpm++){
      fDiffBPM[ibpm]=0.0;
      fBPMCorrection[ibpm]=0.0;
    }
    fBeamCorrection=0.0;
    vector<Double_t> y_val;
    vector<Double_t> y_error;
    vector<Double_t> x_val;
    for(int ievt=0;ievt<nevt;ievt++){
      mini_tree->GetEntry(ievt);

      if(fRunStatsMap[run_number].GetArmFlag()!=0 ||
	 (fRunStatsMap[run_number].GetRunFlag()!="Good"))
	continue;
      
      Int_t bcm_id;
      if(fBCMRunMap.find(run_number)!=fBCMRunMap.end()){
	bcm_id = fBCMRunMap[run_number];
	if(fDetector.error>0){
	  double weight = 1.0/pow(fDetector.error,2);
	  total_weight+=weight;
	  Aq += weight*fBCMArray[bcm_id].mean;
	  fRawDetectorAvg +=weight*fDetectorRaw.mean;
	  for(int ibpm=0;ibpm<nBPM;ibpm++){
	    fDiffBPM[ibpm] += weight*fBPMArray[ibpm].mean;
	    fBPMCorrection[ibpm]+=weight*fSlopes[ibpm]*fBPMArray[ibpm].mean;
	  }
	  fDetectorAvg += weight*fDetector.mean;
	  x_val.push_back(run_number+mini_id/10.0);
	  y_val.push_back(fDetector.mean * 1e9);
	  y_error.push_back(fDetector.error * 1e9);
	}
      }
      else
	cout << " Error:"  
	     << "run " << run_number
	     <<": Normalizing BCM index is not found in map " << endl;

    } // end of minirun loop
    
    if(total_weight>0){
      Aq = Aq/total_weight;
      fDetectorAvg =fDetectorAvg/total_weight;
      fRawDetectorAvg = fRawDetectorAvg/total_weight;
      weighting_error = sqrt(1.0/total_weight);
      for(int ibpm=0;ibpm<nBPM;ibpm++){
	fDiffBPM[ibpm] = fDiffBPM[ibpm]/total_weight;
	fBPMCorrection[ibpm] = fBPMCorrection[ibpm]/total_weight;
	fBeamCorrection+=fBPMCorrection[ibpm];
      }
      // fBeamCorrection = fBeamCorrection/total_weight;
      cout << "Slug:" << slug ;
      cout << "\t total error(ppb): " << weighting_error*1e9 ;
      cout << "\t Detector Mean(ppb): " << fDetectorAvg*1e9 ;
      cout << "\t Weighted Aq Mean(ppb): " << Aq*1e9 ;
      cout << endl;
      
      fReport_main->AddFloatEntry(fDetectorAvg*1e9);
      fReport_main->AddFloatEntry(weighting_error*1e9);
      fReport_beam->AddFloatEntry(Aq*1e9);
      fReport_beam->AddFloatEntry(fBeamCorrection*1e9);
      for(int ibpm=0;ibpm<nBPM;ibpm++)
	fReport_beam->AddFloatEntry(fBPMCorrection[ibpm]*1e9);
      for(int ibpm=0;ibpm<nBPM;ibpm++)
	fReport_beam->AddFloatEntry(fDiffBPM[ibpm]*1e6);
	
      TString plot_title = Form("%s slug%d pull fit",detector.Data(),slug);
      TF1* fpull =PlotPullFit(y_val,y_error,x_val,plot_title);
      fReport_main->AddStringEntry(Form("%.1f/%d",
				       fpull->GetChisquare(),
				       fpull->GetNDF()));
    }
    else{
      weighting_error=0.0;
    }
      
    input_file->Close();
    arm_flag = 0;  // FIXME
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
  fReport_main->InsertHorizontalLine();
  fReport_main->Print();
  fReport_main->Close();
  fReport_beam->InsertHorizontalLine();
  fReport_beam->Print();
  fReport_beam->Close();

  tsw.Print();
}

