void PlotByPolarity_beamoff(){
  gStyle->SetOptFit(1);
  gStyle->SetTitleFontSize(0.1);
  TFile* neg_file = TFile::Open("prex_grand_average_beamoff_neg.root");
  TFile* pos_file = TFile::Open("prex_grand_average_beamoff_pos.root");
  TFile* normal_file = TFile::Open("prex_grand_average_beamoff_normal.root");
  TFile* neutral_file = TFile::Open("prex_grand_average_beamoff_neutral.root");
  TTree *neg_tree = (TTree*)neg_file->Get("slug");
  TTree *pos_tree = (TTree*)pos_file->Get("slug");
  TTree *normal_tree = (TTree*)normal_file->Get("slug");
  TTree *neutral_tree = (TTree*)neutral_file->Get("slug");
  pos_tree->AddFriend(neg_tree,"neg");

  vector<TString> fDetectorNameList={"diff_usl","diff_usr","diff_dsl","diff_dsr",
				     "diff_atl1","diff_atl2","diff_atr1","diff_atr2",
				     "diff_bcm_norm",
				     "diff_bcm_an_us","diff_bcm_an_ds",
				     "diff_bcm_dg_us","diff_bcm_dg_ds",
				     "diff_cav4cQ","diff_bcm_an_ds3",
  				     "diff_bpm4aWS","diff_bpm4eWS",
				     "diff_bpm11WS","diff_bpm12WS","diff_bpm1WS"};
  Int_t nDet = fDetectorNameList.size();
  TCanvas *c1 = new TCanvas("c1","c1",1000,600);
  c1->cd();
  TPad* p1 = new TPad("p1","",0,0.4,1,1);
  p1->Draw();
  p1->SetGridy();
  TPad* p2 = new TPad("p2","",0,0,1,0.4);
  p2->Draw();
  p2->SetGridy();
  p1->SetBottomMargin(0.0);
  p2->SetTopMargin(0.0);
  // TString sign_cut[2] = {"sign>0 ","sign<0"};
  TString sign_cut[2] = {"(((wien==0 || wien==2)&&(ihwp==\"IN\")) ||((wien==1 || wien==3)&&(ihwp==\"OUT\")))","(((wien==0 || wien==2)&&(ihwp==\"OUT\")) ||((wien==1 || wien==3)&&(ihwp==\"IN\")))"};

  TString label[2] = {"left_out_right_in","left_in_right_out"};
  for(int iplot=0;iplot<2;iplot++){
    c1->cd();
    c1->Print("prex_grand_average_"+label[iplot]+"_beamoff.pdf[");
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
      if( device_name.Contains("diff_bpm") &&
	  (device_name.Contains("X") || device_name.Contains("Y")) ){
	rescale = 1e6;
	unit = " (nm)";
      }
      if( device_name.Contains("diff_bpm") &&
	  device_name.Contains("WS") ){
	rescale = 1e9; 
	unit = " (nV)";
      } // JAPAN wiresum: 
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
      if( device_name.Contains("diff") &&
	  (device_name.Contains("l") || device_name.Contains("r"))){
	rescale = 1e9;
	unit = " (nV)";
      }
      if( device_name.Contains("diff_") &&
	  (device_name.Contains("bcm") || device_name.Contains("Q")) ){
	rescale = 1e6; 
	unit = " (pA)";
      } 

      TString draw_cmd = Form("%s*%f:%s.error*%f:slug",
			      device_name.Data(),rescale,
			      device_name.Data(),rescale);
      TString channel_cut = Form(" && %s.error>0 && slug!=87 ",device_name.Data());
      if(device_name.Contains("bcm_an") || device_name.Contains("bcm_norm"))
	 channel_cut += "&& !(slug>=13 && slug<=20)";
      
      pos_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_pos = new TGraphErrors(pos_tree->GetSelectedRows(),
					       pos_tree->GetV3(),pos_tree->GetV1(),
					       0,pos_tree->GetV2());
      ger_pos->SetMarkerStyle(20);
      ger_pos->SetMarkerColor(kBlue);
      ger_pos->SetLineColor(kBlue);
      ger_pos->SetLineStyle(2);
      neg_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_neg = new TGraphErrors(neg_tree->GetSelectedRows(),
					       neg_tree->GetV3(),neg_tree->GetV1(),
					       0,neg_tree->GetV2());
      ger_neg->SetMarkerStyle(20);
      ger_neg->SetMarkerColor(kRed);
      ger_neg->SetLineColor(kRed);
      ger_neg->SetLineStyle(2);
      p1->cd();
      TMultiGraph* mg = new TMultiGraph();
      mg->Add(ger_pos,"LP");
      mg->Add(ger_neg,"LP");
      mg->Draw("A");
      mg->SetTitle("Beam-Off: "+device_name+unit+";slug;"+unit);
      double y_max = mg->GetYaxis()->GetXmax();
      double y_min = mg->GetYaxis()->GetXmin();
      mg->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));
      // mg->GetXaxis()->SetRangeUser(0,95);
      mg->GetXaxis()->SetLimits(0,95);
      TLegend *leg = new TLegend(0.9,0.9,0.7,0.7);
      leg->AddEntry(ger_pos,"polarity +");
      leg->AddEntry(ger_neg,"polarity -");
      leg->Draw("same");
      ger_pos->Fit("pol0");
      ger_pos->GetFunction("pol0")->SetLineColor(kBlue);
      
      ger_neg->Fit("pol0");
      ger_neg->GetFunction("pol0")->SetLineColor(kRed);
      p1->Update();
      TPaveStats* pspos = (TPaveStats*)ger_pos->FindObject("stats");
      pspos->SetTextColor(kBlue);
      pspos->SetX1NDC(0.1);
      pspos->SetY1NDC(0.7);
      pspos->SetX2NDC(0.4);
      pspos->SetY2NDC(0.9);

      TPaveStats* psneg = (TPaveStats*)ger_neg->FindObject("stats");
      psneg->SetTextColor(kRed);
      psneg->SetX1NDC(0.4);
      psneg->SetY1NDC(0.7);
      psneg->SetX2NDC(0.7);
      psneg->SetY2NDC(0.9);

      p2->cd();
      TLegend *leg_nu = new TLegend(0.9,1,0.7,0.8);
      neutral_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_neutral = new TGraphErrors(neutral_tree->GetSelectedRows(),
						  neutral_tree->GetV3(),neutral_tree->GetV1(),
						  0,neutral_tree->GetV2());
      ger_neutral->SetMarkerStyle(20);
      ger_neutral->SetMarkerColor(kBlack);
      ger_neutral->SetLineColor(kBlack);
      ger_neutral->Draw("ALP");
      y_max = ger_neutral->GetYaxis()->GetXmax();
      y_min = ger_neutral->GetYaxis()->GetXmin();
      ger_neutral->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));

      ger_neutral->GetXaxis()->SetLimits(0,95);
      ger_neutral->GetXaxis()->SetLabelSize(0.1);
      ger_neutral->GetYaxis()->SetLabelSize(0.05);
      ger_neutral->SetTitle("");
      leg_nu->AddEntry(ger_neutral,"neutral(pick-up)");
      leg_nu->Draw("same");
      ger_neutral->Fit("pol0");
      p2->Update();
      TPaveStats* psnu = (TPaveStats*)ger_neutral->FindObject("stats");
      psnu->SetTextColor(kBlack);
      psnu->SetX1NDC(0.1);
      psnu->SetY1NDC(0.75);
      psnu->SetX2NDC(0.4);
      psnu->SetY2NDC(1.0);

      c1->Print("prex_grand_average_"+label[iplot]+"_beamoff.pdf");
    } // end of detector loop
    c1->Print("prex_grand_average_"+label[iplot]+"_beamoff.pdf]");
  }
}
