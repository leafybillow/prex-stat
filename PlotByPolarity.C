void PlotByPolarity(){
  gStyle->SetOptFit(1);
  TFile* neg_file = TFile::Open("prex_grand_average_neg.root");
  TFile* pos_file = TFile::Open("prex_grand_average_pos.root");
  TTree *neg_tree = (TTree*)neg_file->Get("slug");
  TTree *pos_tree = (TTree*)pos_file->Get("slug");
  pos_tree->AddFriend(neg_tree,"neg");

  vector<TString> fDetectorNameList={"Adet",
				     "reg_asym_us_avg","reg_asym_us_dd","reg_asym_usl","reg_asym_usr",
				     "asym_us_avg","asym_us_dd","asym_usl","asym_usr",
				     "reg_asym_ds_avg","reg_asym_ds_dd","reg_asym_dsl","reg_asym_dsr",
				     "asym_ds_avg","asym_ds_dd","asym_dsl","asym_dsr",
				     "diff_bpm4aX","diff_bpm4eX","diff_bpm4aY","diff_bpm4eY",
				     "diff_bpm11X","diff_bpm12X","diff_bpmE",
				     "asym_bcm_an_us","asym_bcm_an_ds",
				     "asym_bcm_dg_us","asym_bcm_dg_ds",
				     "asym_battery1l","asym_battery2l",
				     "asym_battery1r","asym_battery2r",
				     "asym_ch_battery_1","asym_ch_battery_2",
				     "diff_battery1l","diff_battery2l",
				     "diff_battery1r","diff_battery2r",
				     "diff_ch_battery_1","diff_ch_battery_2" };
  Int_t nDet = fDetectorNameList.size();
  TCanvas *c1 = new TCanvas("c1","c1",1000,600);
  
  TString sign_cut[2] = {"sign>0","sign<0"};
  TString label[2] = {"left_out_right_in","left_in_right_out"};
  for(int iplot=0;iplot<2;iplot++){
    c1->cd();
    c1->Print("prex_grand_average_"+label[iplot]+".pdf[");
    for(int idet=0;idet<nDet;idet++){
      c1->Clear("D");
      

      TString device_name = fDetectorNameList[idet];
      Double_t rescale =1.0;
      TString unit="";
      if( device_name.Contains("asym")){
	rescale = 1e9;
	unit = " (ppb)";
      }
      if( device_name.Contains("Adet")){
	rescale = 1e9;
	unit = " (ppb)";
      }
      if( device_name.Contains("Aq")){
	rescale = 1e9;
	unit = " (ppb)";
      }
      if( device_name.Contains("diff_bpm")){
	rescale = 1e6;
	unit = " (nm)";
      }
      if( device_name.Contains("diff") &&
	  device_name.Contains("battery")){
	rescale = 1e9;
	unit = " (nV)";
      }
      if( device_name.Contains("diff") &&
	  device_name.Contains("ch_battery")){
	rescale = 76e-6*1e9;
	unit = " (nV)";
      }

      TString draw_cmd = Form("%s*%f:%s.error*%f:slug",
			      device_name.Data(),rescale,
			      device_name.Data(),rescale);
      TString channel_cut = Form(" && %s.error>0",device_name.Data());
      
      pos_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_pos = new TGraphErrors(pos_tree->GetSelectedRows(),
					       pos_tree->GetV3(),pos_tree->GetV1(),
					       0,pos_tree->GetV2());
      ger_pos->SetMarkerStyle(20);
      ger_pos->SetMarkerColor(kBlue);
      ger_pos->SetLineColor(kBlue);

      neg_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_neg = new TGraphErrors(neg_tree->GetSelectedRows(),
					       neg_tree->GetV3(),neg_tree->GetV1(),
					       0,neg_tree->GetV2());
      ger_neg->SetMarkerStyle(20);
      ger_neg->SetMarkerColor(kRed);
      ger_neg->SetLineColor(kRed);
  
      TMultiGraph* mg = new TMultiGraph();
      mg->Add(ger_pos,"LP");
      mg->Add(ger_neg,"LP");
      mg->Draw("A");
      mg->SetTitle(device_name+unit+";slug;"+unit);
      double y_max = mg->GetYaxis()->GetXmax();
      double y_min = mg->GetYaxis()->GetXmin();
      mg->GetYaxis()->SetRangeUser(y_min, y_max+0.5*(y_max-y_min));
      TLegend *leg = new TLegend(0.9,0.9,0.7,0.7);
      leg->AddEntry(ger_pos,"polarity +");
      leg->AddEntry(ger_neg,"polarity -");
      leg->Draw("same");
      
      c1->Print("prex_grand_average_"+label[iplot]+".pdf");
    }
    c1->Print("prex_grand_average_"+label[iplot]+".pdf]");
  }
}
