void PlotByPolarity_beamoff(){
  gStyle->SetOptFit(1);
  TFile* neg_file = TFile::Open("prex_grand_average_beamoff_neg.root");
  TFile* pos_file = TFile::Open("prex_grand_average_beamoff_pos.root");
  TFile* normal_file = TFile::Open("prex_grand_average_beamoff_normal.root");
  TTree *neg_tree = (TTree*)neg_file->Get("slug");
  TTree *pos_tree = (TTree*)pos_file->Get("slug");
  TTree *normal_tree = (TTree*)pos_file->Get("slug");
  pos_tree->AddFriend(neg_tree,"neg");

  vector<TString> fDetectorNameList={"diff_usl","diff_usr","diff_dsl","diff_dsr",
				     "diff_atl1","diff_atl2","diff_atr1","diff_atr2",
				     "diff_bpm4aWS","diff_bpm4eWS",
				     "diff_bpm11WS","diff_bpm12WS","diff_bpm1WS",
				     "diff_bcm_an_us","diff_bcm_an_ds",
				     "diff_bcm_dg_us","diff_bcm_dg_ds",
				     "diff_cav4cQ","diff_bcm_an_ds3"};
  Int_t nDet = fDetectorNameList.size();
  TCanvas *c1 = new TCanvas("c1","c1",1000,600);
  c1->cd();
  TPad* p1 = new TPad("p1","",0,0.3,1,1);
  p1->Draw();
  TPad* p2 = new TPad("p2","",0,0,1,0.3);
  p2->Draw();
  p1->SetBottomMargin(0.0);
  p2->SetTopMargin(0.0);
  TString sign_cut[2] = {"sign>0 ","sign<0"};
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
      if( device_name.Contains("diff_") &&
	  (device_name.Contains("bcm") || device_name.Contains("Q")) ){
	rescale = 1e9; 
	unit = " (fA)";
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
      if( device_name.Contains("diff") &&
	  (device_name.Contains("l") || device_name.Contains("r"))){
	rescale = 1e9;
	unit = " (nV)";
      }

      TString draw_cmd = Form("%s*%f:%s.error*%f:slug",
			      device_name.Data(),rescale,
			      device_name.Data(),rescale);
      TString channel_cut = Form(" && %s.error>0 ",device_name.Data());
      
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

      p1->cd();
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
      
      p2->cd();
      normal_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_normal = new TGraphErrors(normal_tree->GetSelectedRows(),
						  normal_tree->GetV3(),normal_tree->GetV1(),
						  0,normal_tree->GetV2());
      ger_normal->SetMarkerStyle(20);
      ger_normal->SetMarkerColor(kBlack);
      ger_normal->SetLineColor(kBlack);
      ger_normal->Draw("ALP");
      ger_normal->SetTitle("");
      c1->Print("prex_grand_average_"+label[iplot]+"_beamoff.pdf");
    } // end of detector loop
    c1->Print("prex_grand_average_"+label[iplot]+"_beamoff.pdf]");
  }
}
