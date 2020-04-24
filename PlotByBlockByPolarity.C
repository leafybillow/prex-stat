void PlotByBlockByPolarity(){
  gStyle->SetOptFit(1);
  TFile* block0_pos_file = TFile::Open("prex_grand_average_pos.block0.root");
  TTree *block0_pos_tree = (TTree*)block0_pos_file->Get("slug");
  TFile* block0_neg_file = TFile::Open("prex_grand_average_neg.block0.root");
  TTree *block0_neg_tree = (TTree*)block0_neg_file->Get("slug");
  
  TFile* block1_pos_file = TFile::Open("prex_grand_average_pos.block1.root");
  TTree *block1_pos_tree = (TTree*)block1_pos_file->Get("slug");
  TFile* block1_neg_file = TFile::Open("prex_grand_average_neg.block1.root");
  TTree *block1_neg_tree = (TTree*)block1_neg_file->Get("slug");

  TFile* block2_pos_file = TFile::Open("prex_grand_average_pos.block2.root");
  TTree *block2_pos_tree = (TTree*)block2_pos_file->Get("slug");
  TFile* block2_neg_file = TFile::Open("prex_grand_average_neg.block2.root");
  TTree *block2_neg_tree = (TTree*)block2_neg_file->Get("slug");

  TFile* block3_pos_file = TFile::Open("prex_grand_average_pos.block3.root");
  TTree *block3_pos_tree = (TTree*)block3_pos_file->Get("slug");
  TFile* block3_neg_file = TFile::Open("prex_grand_average_neg.block3.root");
  TTree *block3_neg_tree = (TTree*)block3_neg_file->Get("slug");
  
  TFile* block0_file = TFile::Open("prex_grand_average_block0_newReg.root");
  TTree *block0_tree = (TTree*)block0_file->Get("slug");
  TFile* block1_file = TFile::Open("prex_grand_average_block1_newReg.root");
  TTree *block1_tree = (TTree*)block1_file->Get("slug");
  TFile* block2_file = TFile::Open("prex_grand_average_block2_newReg.root");
  TTree *block2_tree = (TTree*)block2_file->Get("slug");
  TFile* block3_file = TFile::Open("prex_grand_average_block3_newReg.root");
  TTree *block3_tree = (TTree*)block3_file->Get("slug");
  TTree* fTreeArray[4] = {block0_tree,block1_tree,block2_tree,block3_tree};
  
  vector<TString> fDetectorNameList={"reg_asym_us_avg","reg_asym_usr","reg_asym_usl",
				     "reg_asym_us_dd",
				     "asym_us_avg","asym_usr","asym_usl",
				     "asym_us_dd",
				     "asym_bcm_an_us","asym_bcm_an_ds",
  				     "asym_bcm_an_ds3","asym_bcm_dg_us",
  				     "asym_bcm_dg_ds","asym_cav4cQ",
				     "diff_bpm4aX","diff_bpm4eX",
				     "diff_bpm4aY","diff_bpm4eY",
				     "diff_bpmE"};
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
  
  TString sign_cut[2] = {"sign>0 && slug!=12","sign<0 && slug!=13"};
  TString label[2] = {"left_out_right_in","left_in_right_out"};

  Int_t nplots = sizeof(sign_cut)/sizeof(*sign_cut);
  for(int iplot=0;iplot<nplots;iplot++){
    c1->cd();
    c1->Print("prex_grand_average_byblock_"+label[iplot]+".pdf[");
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
	draw_cmd = Form("%s_block%d*%f:%s_block%d.error*%f:slug",
			device_name.Data(),iblk,rescale,
			device_name.Data(),iblk,rescale);
	channel_cut = Form(" && %s_block%d.error>0",device_name.Data(),iblk);
	if(device_name.Contains("bcm_an"))
	  channel_cut += "&& !(slug>=13 && slug<=21)";

	TTree *this_tree = fTreeArray[iblk];
	this_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
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
      mg_norm->SetTitle(device_name+unit+";slug;"+unit);
      double y_max = mg_norm->GetYaxis()->GetXmax();
      double y_min = mg_norm->GetYaxis()->GetXmin();
      mg_norm->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));
      leg->Draw("same");
      for(int iblk=0;iblk<4;iblk++){
	TF1 *f1 = new TF1("f1","[0]",0,30);
	TF1 *f2 = new TF1("f2","[0]",31,94);
	f1->SetLineColor(fColor[iblk]);
	f2->SetLineColor(fColor[iblk]);
	fGraphArray[iblk]->Fit("f1","QR","",0,30);
	Double_t par = f1->GetParameter(0);
	gPad->Update();
	TPaveStats* ps1 = (TPaveStats*)fGraphArray[iblk]->FindObject("stats")->Clone();
	ps1->SetName(Form("ps%d",ps_count++));
	ps1->SetTextColor(fColor[iblk]);
	ps1->SetX1NDC(0.1+iblk*0.7/4.0);
	ps1->SetX2NDC(0.1+(iblk+1)*0.7/4.0);
	ps1->SetY1NDC(0.8);
	ps1->SetY2NDC(0.9);
	
	fGraphArray[iblk]->Fit("f2","QR","same",31,94);
	gPad->Update();
	TPaveStats* ps2 = (TPaveStats*)fGraphArray[iblk]->FindObject("stats");
	ps2->SetName(Form("ps%d",ps_count++));
	ps2->SetTextColor(fColor[iblk]);
	ps2->SetX1NDC(0.1+iblk*0.7/4.0);
	ps2->SetX2NDC(0.1+(iblk+1)*0.7/4.0);
	ps2->SetY1NDC(0.8);
	ps2->SetY2NDC(0.7);

	f1->SetParameter(0,par);
	f1->Draw("same");
	ps1->Draw("same");
	// f2->Draw("same");
      }
      c1->Print("prex_grand_average_byblock_"+label[iplot]+".pdf");
      
      // for(int iblk=0;iblk<4;iblk++){
      // 	draw_cmd = Form("%s.block%d*%f:%s.block%d.error*%f:slug",
      // 			device_name.Data(),iblk,rescale,
      // 			device_name.Data(),iblk,rescale);
      // 	channel_cut = Form(" && %s.block%d.error>0",device_name.Data(),iblk);
      // 	if(device_name.Contains("bcm_an"))
      // 	  channel_cut += "&& !(slug>=13 && slug<=21)";

      // 	TTree *this_tree = fPosTreeArray[iblk];
      // 	this_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      // 	TGraphErrors *ger_pos_block = new TGraphErrors(this_tree->GetSelectedRows(),
      // 						       this_tree->GetV3(),this_tree->GetV1(),
      // 						       0,this_tree->GetV2());
      // 	ger_pos_block->SetMarkerStyle(20);
      // 	ger_pos_block->SetMarkerColor(fColor[iblk]);
      // 	ger_pos_block->SetLineColor(fColor[iblk]);
      // 	ger_pos_block->SetLineStyle(2);
      // 	mg->Add(ger_pos_block,"LP");
      // 	leg_pos->AddEntry(ger_pos_block,Form("block%d: pos",iblk));


      // 	this_tree = fNegTreeArray[iblk];
      // 	this_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      // 	TGraphErrors *ger_neg_block = new TGraphErrors(this_tree->GetSelectedRows(),
      // 						       this_tree->GetV3(),this_tree->GetV1(),
      // 						       0,this_tree->GetV2());
      // 	ger_neg_block->SetMarkerStyle(24);
      // 	ger_neg_block->SetMarkerColor(fColor[iblk]);
      // 	ger_neg_block->SetLineColor(fColor[iblk]);
      // 	ger_neg_block->SetLineStyle(2);
      // 	mg->Add(ger_neg_block,"LP");
      // 	leg_neg->AddEntry(ger_neg_block,"neg");
      // }
      // c1->cd();
      // mg->Draw("A");
      // mg->SetTitle(device_name+unit+";slug;"+unit);
      // y_max = mg->GetYaxis()->GetXmax();
      // y_min = mg->GetYaxis()->GetXmin();
      // mg->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));
      // leg_pos->Draw("same");
      // leg_neg->Draw("same");
      // c1->Print("prex_grand_average_byblock_"+label[iplot]+".pdf");
      
      for(int iblk=0;iblk<4;iblk++){
	c2->cd(iblk+1);
	TMultiGraph *mg_in_block = new TMultiGraph();
	TLegend *leg_block = new TLegend(0.9,0.6,1.0,0.9);	
	draw_cmd = Form("%s.block%d*%f:%s.block%d.error*%f:slug",
			device_name.Data(),iblk,rescale,
			device_name.Data(),iblk,rescale);
	channel_cut = Form(" && %s.block%d.error>0",device_name.Data(),iblk);
	if(device_name.Contains("bcm_an"))
	  channel_cut += "&& !(slug>=13 && slug<=21)";

	TTree *this_tree = fPosTreeArray[iblk];
	this_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
	TGraphErrors *ger_pos_block = new TGraphErrors(this_tree->GetSelectedRows(),
						       this_tree->GetV3(),this_tree->GetV1(),
						       0,this_tree->GetV2());
	ger_pos_block->SetMarkerStyle(20);
	ger_pos_block->SetMarkerColor(fColor[iblk]);
	ger_pos_block->SetLineColor(fColor[iblk]);
	ger_pos_block->SetLineStyle(2);

	this_tree = fNegTreeArray[iblk];
	this_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
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
	mg_in_block->GetXaxis()->SetLimits(0,95);
	double ymax = mg_in_block->GetYaxis()->GetXmax();
	double ymin = mg_in_block->GetYaxis()->GetXmin();
	mg_in_block->GetYaxis()->SetRangeUser(ymin,ymax + 0.5*(ymax-ymin));
	
	TF1 *f1pos = new TF1("f1pos","[0]",0,30);
	f1pos->SetLineColor(fColor[iblk]);
	TF1 *f2pos = new TF1("f2pos","[0]",31,94);
	f2pos->SetLineColor(fColor[iblk]);
	TF1 *f1neg = new TF1("f1neg","[0]",0,30);
	f1neg->SetLineColor(fColor[iblk]);
	TF1 *f2neg = new TF1("f2neg","[0]",31,94);
	f2neg->SetLineColor(fColor[iblk]);
	
	ger_pos_block->Fit("f1pos","QR0","",0,30);
	gPad->Update();
	TPaveStats* ps1 = (TPaveStats*)ger_pos_block->FindObject("stats")->Clone();
	ps1->SetName(Form("ps%d",ps_count++));
	ps1->SetX1NDC(0.1);
	ps1->SetX2NDC(0.5);
	ps1->SetY1NDC(0.75);
	ps1->SetY2NDC(0.9);

	ger_pos_block->Fit("f2pos","QR0","",31,94);
	gPad->Update();
	TPaveStats* ps2 = (TPaveStats*)ger_pos_block->FindObject("stats");
	ps2->SetName(Form("ps%d",ps_count++));
	ps2->SetX1NDC(0.5);
	ps2->SetX2NDC(0.9);
	ps2->SetY1NDC(0.75);
	ps2->SetY2NDC(0.9);

	f1pos->Draw("same");
	f2pos->Draw("same");
	ps1->Draw("");
	ps2->Draw("");
	
	ger_neg_block->Fit("f1neg","QR0","",0,30);
	gPad->Update();
	TPaveStats* ps3 = (TPaveStats*)ger_neg_block->FindObject("stats")->Clone();
	ps3->SetName(Form("ps%d",ps_count++));
	ps3->SetX1NDC(0.1);
	ps3->SetX2NDC(0.5);
	ps3->SetY1NDC(0.60);
	ps3->SetY2NDC(0.75);

	ger_neg_block->Fit("f2neg","QR0","",31,94);
	gPad->Update();
	TPaveStats* ps4 = (TPaveStats*)ger_neg_block->FindObject("stats");
	ps4->SetName(Form("ps%d",ps_count++));
	ps4->SetX1NDC(0.5);
	ps4->SetX2NDC(0.9);
	ps4->SetY1NDC(0.60);
	ps4->SetY2NDC(0.75);
	f1neg->Draw("same");
	f2neg->Draw("same");
	ps3->Draw("");
	ps4->Draw("");
	leg_block->AddEntry(ger_pos_block,"pos");
	leg_block->AddEntry(ger_neg_block,"neg");
	leg_block->Draw("same");
      }
      c2->Print("prex_grand_average_byblock_"+label[iplot]+".pdf");

    }
    c1->Print("prex_grand_average_byblock_"+label[iplot]+".pdf]");
    // c2->Print("prex_grand_average_byblock_"+label[iplot]+".pdf]");
  }
}
