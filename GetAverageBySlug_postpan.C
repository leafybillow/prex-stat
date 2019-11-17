#include "lib/TaAccumulator.cc"
#include "lib/TaRunInfo.cc"
#include "LoadNormalizationMap.C"
#include "LoadPittsMap.C"
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
  
  // TString det_array[]={"reg_asym_us_avg","reg_asym_usr","reg_asym_usl",
  // 		       "reg_asym_us_dd","reg_asym_left_dd","reg_asym_right_dd"};
  
  // TString rawdet_array[]={"asym_us_avg","asym_usr","asym_usl",
  // 			  "asym_us_dd","asym_left_dd","asym_right_dd"};

    
  TString det_array[]={"reg_asym_us_avg","reg_asym_usr","reg_asym_usl"};
  
  TString rawdet_array[]={"asym_us_avg","asym_usr","asym_usl"};

  Int_t nDet = sizeof(det_array)/sizeof(*det_array);
  
  TFile* output = TFile::Open("./rootfiles/prex_grand_average_postpan.root",
			      "RECREATE");
  TTree* grand_tree = new TTree("grand","grand summary tree");
  grand_tree->SetMarkerStyle(20);
  Int_t ihwp,wien,sign,arm_flag,pitt;
  Double_t slug;
  grand_tree->Branch("ihwp",&ihwp);
  grand_tree->Branch("wien",&wien);
  grand_tree->Branch("sign",&sign);
  grand_tree->Branch("slug",&slug);
  grand_tree->Branch("pitt",&pitt);
  grand_tree->Branch("arm_flag",&arm_flag);
  
  Double_t Aq,fBeamCorrection;
  Double_t fPrimaryDetector_mean;
  Double_t fPrimaryDetector_error;
  vector<Double_t> fDetector_mean(nDet);
  vector<Double_t> fRawDetector_mean(nDet);
  vector<Double_t> fDiffBPM(nBPM);
  vector<Double_t> fBPMCorrection(nBPM);
  
  grand_tree->Branch("primary_det",&fPrimaryDetector_mean);
  grand_tree->Branch("primary_error",&fPrimaryDetector_error);

  for(int i=0;i<nDet;i++){
    grand_tree->Branch(det_array[i],&fDetector_mean[i]);
    grand_tree->Branch(rawdet_array[i],&fRawDetector_mean[i]);
  }

  grand_tree->Branch("Aq",&Aq);
  
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

  map<SLUG_ARM,Int_t> fPittMap = LoadPittsMap();
  map< pair<Int_t,Int_t>, vector<Int_t> > fSlugMap;
  map< Int_t, TaRunInfo > fRunInfoMap;  
  for(int islug=1;islug<=94;islug++){
  
    TString info_filename = Form("./runlist/slug%d_info.list",islug);
    ifstream slug_info;
    slug_info.open(info_filename.Data());

    if(!slug_info.is_open()){
      cout << " slug info " << info_filename << " does not exist." << endl;
      continue;
    }
    
    TString sline;
    Int_t slug_id=-1;
    Int_t arm_flag=-1;
    TString ihwp_stat;
    TString wien_mode;

    while(sline.ReadLine(slug_info)){
      TaRunInfo fRunInfo;
      if(!fRunInfo.ParseLine(sline))
	continue;
      if(fRunInfo.GetRunFlag()=="Bad")
      	continue;
      slug_id = fRunInfo.GetSlugNumber();
      arm_flag = fRunInfo.GetArmFlag();
      Int_t run_number = fRunInfo.GetRunNumber();
      fSlugMap[make_pair(slug_id,arm_flag)].push_back(run_number);
      fRunInfoMap[run_number]=fRunInfo;
    }
    slug_info.close();
  }

  TaResult *fReport_main = new TaResult("output/averages_by_slug_maindet.log");
  TaResult *fReport_beam = new TaResult("output/averages_by_slug_beamline.log");
  
  vector<TString> header_main{"Slug","Adet(ppb)", "Error","chi2/NDF"};
  vector<TString> header_beam{"Slug","AQ(ppb)","Abeam",
			      "A4aX","A4eX","A4aY","A4eY","A11X","A12X",
			      "D4aX(nm)","D4eX","D4aY","D4eY","D11X","D12X"};
  fReport_main->AddHeader(header_main);
  fReport_beam->AddHeader(header_beam);


  auto iter_slug = fSlugMap.begin();
  typedef struct{Double_t mean,error,rms;} LEAFLIST;
  while(iter_slug!=fSlugMap.end()){
    Int_t mySlug = ((*iter_slug).first).first;
    Int_t myArmFlag = ((*iter_slug).first).second;
    vector<Int_t> runlet = (*iter_slug).second;
    TFile* input_file = TFile::Open(Form("./rootfiles/postpan/prexPrompt_slug%d.root",mySlug));
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

    TString arm_text="";
    if(myArmFlag==1)
      arm_text=".R";
    else if(myArmFlag==2)
      arm_text=".L";
    
    fReport_main->AddLine();
    fReport_main->AddStringEntry(Form("%d%s",mySlug,arm_text.Data()));
    
    fReport_beam->AddLine();
    fReport_beam->AddStringEntry(Form("%d%s",mySlug,arm_text.Data()));

    vector<LEAFLIST> fBCMArray(nBCM);
    vector<LEAFLIST> fBPMArray(nBPM);
    vector<Double_t> fDummy(nBPM);
    vector<vector<Double_t> > fSlopes(nDet,fDummy);
    vector<LEAFLIST> fDetectorArray(nDet);
    vector<LEAFLIST> fDetectorRawArray(nDet);
    Bool_t kMatch=kTRUE;
    Int_t run_number;
    Int_t mini_id;
    mini_tree->SetBranchAddress("run",&run_number);
    mini_tree->SetBranchAddress("mini",&mini_id);

    for(int idet=0;idet<nDet;idet++){
      TBranch *det_ptr = mini_tree->GetBranch(det_array[idet]);
      det_ptr->GetLeaf("mean")->SetAddress(&(fDetectorArray[idet]).mean);
      det_ptr->GetLeaf("err")->SetAddress(&(fDetectorArray[idet]).error);
      det_ptr->GetLeaf("rms")->SetAddress(&(fDetectorArray[idet]).rms);
      
      det_ptr = mini_tree->GetBranch(rawdet_array[idet]);
      det_ptr->GetLeaf("mean")->SetAddress(&(fDetectorRawArray[idet]).mean);
      det_ptr->GetLeaf("err")->SetAddress(&(fDetectorRawArray[idet]).error);
      det_ptr->GetLeaf("rms")->SetAddress(&(fDetectorRawArray[idet]).rms);
    }


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

    for(int idet=0;idet<nDet;idet++){
      for(int ibpm=0;ibpm<nBPM;ibpm++){
	TString raw_name = rawdet_array[idet];
	mini_tree->SetBranchAddress(raw_name+"_"+bpm_array[ibpm],
				    &fSlopes[idet][ibpm]);
      }
    }
    Int_t nevt = mini_tree->GetEntries();
    
    Double_t total_weight=0.0;
    Aq=0.0;
    fPrimaryDetector_error = 0.0;
    fPrimaryDetector_mean = 0.0;
    fBeamCorrection=0.0;
    
    for(int ibpm=0;ibpm<nBPM;ibpm++){
      fDiffBPM[ibpm]=0.0;
      fBPMCorrection[ibpm]=0.0;
    }

    vector<Double_t> y_val;
    vector<Double_t> y_error;
    vector<Double_t> x_val;
    TString ihwp_str,wien_str;
    for(int ievt=0;ievt<nevt;ievt++){
      mini_tree->GetEntry(ievt);

      if(find(runlet.begin(),runlet.end(),run_number)==runlet.end())
	continue;
      
      if(fRunInfoMap[run_number].GetArmFlag()!=myArmFlag ||
	 (fRunInfoMap[run_number].GetRunFlag()!="Good") )
	continue;

      ihwp_str = fRunInfoMap[run_number].GetIHWPStatus();
      wien_str = fRunInfoMap[run_number].GetWienMode();
	
      Int_t bcm_id;
      if(fBCMRunMap.find(run_number)!=fBCMRunMap.end()){
	bcm_id = fBCMRunMap[run_number];
	if(fDetectorArray[myArmFlag].error>0){
	  double weight = 1.0/pow(fDetectorArray[myArmFlag].error,2);
	  total_weight+=weight;
	  Aq += weight*fBCMArray[bcm_id].mean;
	  // fRawDetectorAvg +=weight*fDetectorRaw.mean;
	  for(int ibpm=0;ibpm<nBPM;ibpm++){
	    fDiffBPM[ibpm] += weight*fBPMArray[ibpm].mean;
	    fBPMCorrection[ibpm]+=weight*fSlopes[myArmFlag][ibpm]*fBPMArray[ibpm].mean;
	  }
	  fPrimaryDetector_mean += weight*fDetectorArray[myArmFlag].mean;
	  x_val.push_back(run_number+mini_id/10.0);
	  y_val.push_back(fDetectorArray[myArmFlag].mean * 1e9);
	  y_error.push_back(fDetectorArray[myArmFlag].error * 1e9);
	}
      }
      else
	cout << " Error:"  
	     << "run " << run_number
	     <<": Normalizing BCM index is not found in map " << endl;

    } // end of minirun loop
    
    if(total_weight>0){
      Aq = Aq/total_weight;
      fPrimaryDetector_mean =fPrimaryDetector_mean/total_weight;
      // fRawDetectorAvg = fRawDetectorAvg/total_weight;
      fPrimaryDetector_error = sqrt(1.0/total_weight);
      for(int ibpm=0;ibpm<nBPM;ibpm++){
	fDiffBPM[ibpm] = fDiffBPM[ibpm]/total_weight;
	fBPMCorrection[ibpm] = fBPMCorrection[ibpm]/total_weight;
	fBeamCorrection+=fBPMCorrection[ibpm];
      }
      // fBeamCorrection = fBeamCorrection/total_weight;
      slug = mySlug+myArmFlag/10.0;
      cout << "Slug:" << slug ;
      cout << "\t total error(ppb): " << fPrimaryDetector_error*1e9 ;
      cout << "\t Detector Mean(ppb): " << fPrimaryDetector_mean*1e9 ;
      cout << "\t Weighted Aq Mean(ppb): " << Aq*1e9 ;
      cout << endl;
      
      fReport_main->AddFloatEntry(fPrimaryDetector_mean*1e9);
      fReport_main->AddFloatEntry(fPrimaryDetector_error*1e9);
      fReport_beam->AddFloatEntry(Aq*1e9);
      fReport_beam->AddFloatEntry(fBeamCorrection*1e9);
      for(int ibpm=0;ibpm<nBPM;ibpm++)
	fReport_beam->AddFloatEntry(fBPMCorrection[ibpm]*1e9);
      for(int ibpm=0;ibpm<nBPM;ibpm++)
	fReport_beam->AddFloatEntry(fDiffBPM[ibpm]*1e6);
	
      TString plot_title = Form("%s slug%d pull fit",
				det_array[myArmFlag].Data(),mySlug);
      TF1* fpull =PlotPullFit(y_val,y_error,x_val,plot_title);
      fReport_main->AddStringEntry(Form("%.1f/%d",
					fpull->GetChisquare(),
					fpull->GetNDF()));
    } else{
      fPrimaryDetector_error=0.0;
    }
      
    input_file->Close();
    
    arm_flag = myArmFlag;
    
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
    sign = ihwp*wien;
    pitt = fPittMap[make_pair(mySlug,myArmFlag)];
    grand_tree->Fill();

    iter_slug++;
  } // end of slug loop
  

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
