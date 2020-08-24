void PlotAqByBlockByPolarity_pitt(){
  gStyle->SetOptFit(1);
  TFile* block0_pos_file = TFile::Open("prex_grand_average_pos.block0.root");
  TTree *block0_pos_tree = (TTree*)block0_pos_file->Get("pitt");
  TFile* block0_neg_file = TFile::Open("prex_grand_average_neg.block0.root");
  TTree *block0_neg_tree = (TTree*)block0_neg_file->Get("pitt");
  
  TFile* block1_pos_file = TFile::Open("prex_grand_average_pos.block1.root");
  TTree *block1_pos_tree = (TTree*)block1_pos_file->Get("pitt");
  TFile* block1_neg_file = TFile::Open("prex_grand_average_neg.block1.root");
  TTree *block1_neg_tree = (TTree*)block1_neg_file->Get("pitt");

  TFile* block2_pos_file = TFile::Open("prex_grand_average_pos.block2.root");
  TTree *block2_pos_tree = (TTree*)block2_pos_file->Get("pitt");
  TFile* block2_neg_file = TFile::Open("prex_grand_average_neg.block2.root");
  TTree *block2_neg_tree = (TTree*)block2_neg_file->Get("pitt");

  TFile* block3_pos_file = TFile::Open("prex_grand_average_pos.block3.root");
  TTree *block3_pos_tree = (TTree*)block3_pos_file->Get("pitt");
  TFile* block3_neg_file = TFile::Open("prex_grand_average_neg.block3.root");
  TTree *block3_neg_tree = (TTree*)block3_neg_file->Get("pitt");

  TFile* block0_file = TFile::Open("prex_grand_average_block0.root");
  TTree *block0_tree = (TTree*)block0_file->Get("pitt");
  
  TFile* block1_file = TFile::Open("prex_grand_average_block1.root");
  TTree *block1_tree = (TTree*)block1_file->Get("pitt");
  
  TFile* block2_file = TFile::Open("prex_grand_average_block2.root");
  TTree *block2_tree = (TTree*)block2_file->Get("pitt");
  
  TFile* block3_file = TFile::Open("prex_grand_average_block3.root");
  TTree *block3_tree = (TTree*)block3_file->Get("pitt");
  
  TTree* fTreeArray[4] = {block0_tree,block1_tree,block2_tree,block3_tree};
  
  vector<TString> fDetectorNameList={"Aq","asym_bcm_an_us","asym_bcm_an_ds",
  				     "asym_bcm_an_ds3","asym_bcm_dg_us",
  				     "asym_bcm_dg_ds","asym_cav4cQ",
				     "asym_bpm4aWS","asym_bpm4eWS",
				     "diff_ch_battery_1","diff_ch_battery_2",
				     "diff_bpm4aX","diff_bpm4eX",
				     "diff_bpm4aY","diff_bpm4eY",
				     "diff_bpmE",
				     "diff_battery1l","diff_battery2l",
				     "diff_battery1r","diff_battery2r"};



  TTree* fPosTreeArray[4] = {block0_pos_tree,block1_pos_tree,block2_pos_tree,block3_pos_tree};
  TTree* fNegTreeArray[4] = {block0_neg_tree,block1_neg_tree,block2_neg_tree,block3_neg_tree};
  Color_t fColor[4]={kBlack,kBlue,kRed,kMagenta};
  Int_t nDet = fDetectorNameList.size();
  TCanvas *c1 = new TCanvas("c1","c1",1000,600);
  TCanvas *c2 = new TCanvas("c2","c2",1000,600);
  c2->Divide(2,2);
  c2->cd(1);
  gPad->SetRightMargin(0.0);
  gPad->SetBottomMargin(0.05);
  c2->cd(2);
  gPad->SetRightMargin(0.0);
  gPad->SetBottomMargin(0.05);
  c2->cd(3);
  gPad->SetRightMargin(0.0);
  gPad->SetBottomMargin(0.05);
  c2->cd(4);
  gPad->SetRightMargin(0.0);
  gPad->SetBottomMargin(0.05);
  c1->cd();
  c1->Print("prex_pitt_average_Aq_byblock.pdf[");
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

    TString draw_cmd;
    TString channel_cut;

    TMultiGraph* mg = new TMultiGraph();
    TLegend *leg_pos = new TLegend(0.83,0.9,0.7,0.7);
    TLegend *leg_neg = new TLegend(0.9,0.9,0.83,0.7);
    Int_t ps_count =0;
      
    TMultiGraph* mg_norm = new TMultiGraph();
    TLegend *leg = new TLegend(0.9,0.9,0.8,0.7);
    vector<TGraphErrors*> fGraphArray(4);
    c1->cd();
    for(int iblk=0;iblk<4;iblk++){
      draw_cmd = Form("%s.block%d*%f:%s.block%d.error*%f:pitt",
		      device_name.Data(),iblk,rescale,
		      device_name.Data(),iblk,rescale);
      channel_cut = Form("%s.block%d.error>0",device_name.Data(),iblk);
      // if(device_name.Contains("battery") || device_name.Contains("WS")){
      //   draw_cmd = Form("%s.block%d*%f:%s.block%d.error*%f:pitt",
      // 		  device_name.Data(),iblk,rescale,
      // 		  device_name.Data(),iblk,rescale);
      //   channel_cut = Form(" && %s.block%d.error>0",device_name.Data(),iblk);
      // }
      if(device_name.Contains("bcm_an"))
        channel_cut += "&& !(pitt>=3 && pitt<=5)";

      TTree *this_tree = fTreeArray[iblk];
      this_tree->Draw(draw_cmd,channel_cut,"goff");
      TGraphErrors *ger_block = new TGraphErrors(this_tree->GetSelectedRows(),
						 this_tree->GetV3(),this_tree->GetV1(),
						 0,this_tree->GetV2());
      ger_block->SetMarkerStyle(21);
      ger_block->SetMarkerColor(fColor[iblk]);
      ger_block->SetLineColor(fColor[iblk]);
      ger_block->SetLineStyle(2);
      mg_norm->Add(ger_block,"LP");
      leg->AddEntry(ger_block,Form("block%d",iblk));
      fGraphArray[iblk] = ger_block;
    }
    mg_norm->Draw("A");
    mg_norm->SetTitle(device_name+unit+";pitt;"+unit);
    double y_max = mg_norm->GetYaxis()->GetXmax();
    double y_min = mg_norm->GetYaxis()->GetXmin();
    mg_norm->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));
    leg->Draw("same");
    for(int iblk=0;iblk<4;iblk++){
      TF1 *f1 = new TF1("f1","[0]",0,25);
      f1->SetLineColor(fColor[iblk]);
      fGraphArray[iblk]->Fit("f1","QR","",0,25);
      Double_t par = f1->GetParameter(0);
      gPad->Update();
      TPaveStats* ps1 = (TPaveStats*)fGraphArray[iblk]->FindObject("stats");
      ps1->SetName(Form("ps%d",ps_count++));
      ps1->SetTextColor(fColor[iblk]);
      ps1->SetX1NDC(0.1+iblk*0.7/4.0);
      ps1->SetX2NDC(0.1+(iblk+1)*0.7/4.0);
      ps1->SetY1NDC(0.8);
      ps1->SetY2NDC(0.9);

      f1->SetParameter(0,par);
      f1->Draw("same");
      ps1->Draw("same");
      // f2->Draw("same");
    }
    c1->Print("prex_pitt_average_Aq_byblock.pdf");
    
    c2->Clear("D");
    for(int iblk=0;iblk<4;iblk++){
      c2->cd(iblk+1);
      TMultiGraph *mg_in_block = new TMultiGraph();
      TLegend *leg_block = new TLegend(0.9,0.6,1.0,0.9);
      draw_cmd = Form("%s.block%d*%f:%s.block%d.error*%f:pitt",
		      device_name.Data(),iblk,rescale,
		      device_name.Data(),iblk,rescale);
      channel_cut = Form("%s.block%d.error>0",device_name.Data(),iblk);
      
      if(device_name.Contains("bcm_an"))
	channel_cut += "&& !(pitt>=3 && pitt<=5)";

      TTree *this_tree = fPosTreeArray[iblk];
      this_tree->Draw(draw_cmd,channel_cut,"goff");
      TGraphErrors *ger_pos_block = new TGraphErrors(this_tree->GetSelectedRows(),
						     this_tree->GetV3(),this_tree->GetV1(),
						     0,this_tree->GetV2());
      ger_pos_block->SetMarkerStyle(20);
      ger_pos_block->SetMarkerColor(fColor[iblk]);
      ger_pos_block->SetLineColor(fColor[iblk]);
      ger_pos_block->SetLineStyle(2);

      this_tree = fNegTreeArray[iblk];
      this_tree->Draw(draw_cmd,channel_cut,"goff");
      TGraphErrors *ger_neg_block = new TGraphErrors(this_tree->GetSelectedRows(),
						     this_tree->GetV3(),this_tree->GetV1(),
						     0,this_tree->GetV2());
      ger_neg_block->SetMarkerStyle(24);
      ger_neg_block->SetMarkerColor(fColor[iblk]);
      ger_neg_block->SetLineColor(fColor[iblk]);
      ger_neg_block->SetLineStyle(2);
	
      mg_in_block->Add(ger_pos_block,"LP");
      mg_in_block->Add(ger_neg_block,"LP");
      mg_in_block->Draw("A");
      mg_in_block->GetXaxis()->SetLimits(0,25);
      double ymax = mg_in_block->GetYaxis()->GetXmax();
      double ymin = mg_in_block->GetYaxis()->GetXmin();
      mg_in_block->GetYaxis()->SetRangeUser(ymin,ymax + 0.5*(ymax-ymin));
	
      TF1 *f1pos = new TF1("f1pos","[0]",0,25);
      f1pos->SetLineColor(fColor[iblk]);
      TF1 *f1neg = new TF1("f1neg","[0]",0,25);
      f1neg->SetLineColor(fColor[iblk]);
	
      ger_pos_block->Fit("f1pos","QR0","",0,25);
      gPad->Update();
      TPaveStats* ps1 = (TPaveStats*)ger_pos_block->FindObject("stats");
      ps1->SetName(Form("ps%d",ps_count++));
      ps1->SetX1NDC(0.1);
      ps1->SetX2NDC(0.5);
      ps1->SetY1NDC(0.75);
      ps1->SetY2NDC(0.9);

      f1pos->Draw("same");
      ps1->Draw("");
	
      ger_neg_block->Fit("f1neg","QR0","",0,25);
      gPad->Update();
      TPaveStats* ps3 = (TPaveStats*)ger_neg_block->FindObject("stats");
      ps3->SetName(Form("ps%d",ps_count++));
      ps3->SetX1NDC(0.1);
      ps3->SetX2NDC(0.5);
      ps3->SetY1NDC(0.60);
      ps3->SetY2NDC(0.75);
      
      f1neg->Draw("same");
      ps3->Draw("");

      leg_block->AddEntry(ger_pos_block,"pos");
      leg_block->AddEntry(ger_neg_block,"neg");
      leg_block->Draw("same");
    }
    c2->Print("prex_pitt_average_Aq_byblock.pdf");

  }
  c1->Print("prex_pitt_average_Aq_byblock.pdf]");

}
