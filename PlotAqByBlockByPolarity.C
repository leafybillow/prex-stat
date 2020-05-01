#include "device_list.hh"
void PlotAqByBlockByPolarity(){
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
  
  TFile* block0neutral_file = TFile::Open("prex_grand_average_neutral.block0.root");
  TTree *block0_nil_tree = (TTree*)block0neutral_file->Get("slug");
  TFile* block1neutral_file = TFile::Open("prex_grand_average_neutral.block1.root");
  TTree *block1_nil_tree = (TTree*)block1neutral_file->Get("slug");
  TFile* block2neutral_file = TFile::Open("prex_grand_average_neutral.block2.root");
  TTree *block2_nil_tree = (TTree*)block2neutral_file->Get("slug");
  TFile* block3neutral_file = TFile::Open("prex_grand_average_neutral.block3.root");
  TTree *block3_nil_tree = (TTree*)block3neutral_file->Get("slug");

  TFile* block0_file = TFile::Open("prex_grand_average_block0.root");
  TTree *block0_tree = (TTree*)block0_file->Get("slug");
  
  TFile* block1_file = TFile::Open("prex_grand_average_block1.root");
  TTree *block1_tree = (TTree*)block1_file->Get("slug");
  
  TFile* block2_file = TFile::Open("prex_grand_average_block2.root");
  TTree *block2_tree = (TTree*)block2_file->Get("slug");
  
  TFile* block3_file = TFile::Open("prex_grand_average_block3.root");
  TTree *block3_tree = (TTree*)block3_file->Get("slug");
  
  TTree* fTreeArray[4] = {block0_tree,block1_tree,block2_tree,block3_tree};

  TFile* neg_file = TFile::Open("prex_grand_average_neg.root");
  TFile* pos_file = TFile::Open("prex_grand_average_pos.root");
  TFile* neutral_file = TFile::Open("prex_grand_average_neutral.root");
  TTree *neg_tree = (TTree*)neg_file->Get("slug");
  TTree *pos_tree = (TTree*)pos_file->Get("slug");
  TTree *neutral_tree = (TTree*)neutral_file->Get("slug");
  
  TFile* normal_file = TFile::Open("prex_grand_average_norm.root");
  TTree *normal_tree = (TTree*)normal_file->Get("slug");

  TCanvas *c_norm = new TCanvas("c_norm","c_norm",1000,600);
  
  TCanvas *c_pol = new TCanvas("c_pol","c_pol",1000,600);
  c_pol->cd();
  TPad* p1 = new TPad("p1","",0,0.4,1,1);
  p1->Draw();
  p1->SetGridy();
  TPad* p2 = new TPad("p2","",0,0,1,0.4);
  p2->Draw();
  p2->SetGridy();
  p1->SetBottomMargin(0.0);
  p2->SetTopMargin(0.0);

  vector<TString> fDetectorNameList={"Aq",
				     "asym_bcm_an_us","asym_bcm_an_ds",
  				     "asym_bcm_an_ds3","asym_bcm_dg_us",
				     "dd_bcm_an_us_bcm_dg_us",
				     "dd_bcm_an_us_bcm_dg_ds",
				     "dd_bcm_an_ds_bcm_dg_us",
				     "dd_bcm_an_ds_bcm_dg_ds",
  				     "asym_bcm_dg_ds","asym_cav4cQ",
				     "asym_bpm4aWS","asym_bpm4eWS",
				     "asym_bpm4acWS","asym_bpm4ecWS",
				     "diff_ch_battery_1","diff_ch_battery_2",
				     "diff_bpm4aX","diff_bpm4eX",
				     "diff_bpm4aY","diff_bpm4eY",
				     "diff_bpmE",
				     "diff_battery1l","diff_battery2l",
				     "diff_battery1r","diff_battery2r"};


  TTree* fPosTreeArray[4] = {block0_pos_tree,block1_pos_tree,block2_pos_tree,block3_pos_tree};
  TTree* fNegTreeArray[4] = {block0_neg_tree,block1_neg_tree,block2_neg_tree,block3_neg_tree};
  TTree* fNullTreeArray[4] = {block0_nil_tree,block1_nil_tree,block2_nil_tree,block3_nil_tree};
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
  
  TString sign_cut[] = {"ihwp==\"IN\"","ihwp==\"OUT\""};
  TString label[] = {"ihwp_in","ihwp_out"};
  for(int iplot=0;iplot<2;iplot++){
    c1->Print("prex_slug_average_Aq_byblock_"+label[iplot]+".pdf[");
    
    for(int idet=0;idet<nDet;idet++){

      TString device_name = fDetectorNameList[idet];
      // >>> FIXME  : I don't have block level AT information at this point
      if(device_name.Contains("at") && !(device_name.Contains("battery")))
	continue;
      // <<<
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
      if( device_name.Contains("Aq") || device_name.Contains("dd")){
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
      TString channel_cut = Form(" %s.error>0",device_name.Data());

      if(device_name.Contains("bcm_an"))
        channel_cut += "&& !(slug>=13 && slug<=21)";
      if(device_name.Contains("cWS"))
        channel_cut += "&& !(slug==12)";
    
      Int_t nData = pos_tree->Draw(draw_cmd,sign_cut[iplot]+"&&"+channel_cut,"goff");
      if(nData==0)
	continue;
    
      c_norm->cd();
      c_norm->Clear("D");
      normal_tree->Draw(draw_cmd,sign_cut[iplot]+"&&"+channel_cut,"goff");
      TGraphErrors *ger_norm = new TGraphErrors(normal_tree->GetSelectedRows(),
						normal_tree->GetV3(),normal_tree->GetV1(),
					      0,normal_tree->GetV2());
    ger_norm->Draw("AP");
    ger_norm->SetMarkerStyle(20);
    ger_norm->Fit("pol0","Q");
    ger_norm->SetTitle(device_name+unit+";slug;"+unit);

    c_norm->Print("prex_slug_average_Aq_byblock_"+label[iplot]+".pdf");
    
    // Polarity plots
    c_pol->Clear("D");    
    TGraphErrors *ger_pos = new TGraphErrors(pos_tree->GetSelectedRows(),
					     pos_tree->GetV3(),pos_tree->GetV1(),
					     0,pos_tree->GetV2());
    ger_pos->SetMarkerStyle(20);
    ger_pos->SetMarkerColor(kBlue);
    ger_pos->SetLineColor(kBlue);
    ger_pos->SetLineStyle(2);
      
    neg_tree->Draw(draw_cmd,sign_cut[iplot]+"&&"+channel_cut,"goff");
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
    mg->SetTitle(device_name+unit+";slug;"+unit);
    double y_max = mg->GetYaxis()->GetXmax();
    double y_min = mg->GetYaxis()->GetXmin();
    mg->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));
    TLegend *leg = new TLegend(0.9,0.9,0.7,0.7);
    leg->AddEntry(ger_pos,"polarity +");
    leg->AddEntry(ger_neg,"polarity -");
    leg->Draw("same");

    ger_pos->Fit("pol0","Q");
    ger_pos->GetFunction("pol0")->SetLineColor(kBlue);
      
    ger_neg->Fit("pol0","Q");
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
    mg->GetXaxis()->SetLimits(0,95);

    p2->cd();
    TLegend *leg_nu = new TLegend(0.9,1,0.7,0.8);
    neutral_tree->Draw(draw_cmd,sign_cut[iplot]+"&&"+channel_cut,"goff");
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
    ger_neutral->Fit("pol0","Q");
    p2->Update();
      
    TPaveStats* psnu = (TPaveStats*)ger_neutral->FindObject("stats");
    psnu->SetTextColor(kBlack);
    psnu->SetX1NDC(0.1);
    psnu->SetY1NDC(0.75);
    psnu->SetX2NDC(0.4);
    psnu->SetY2NDC(1.0);

    c_pol->Print("prex_slug_average_Aq_byblock_"+label[iplot]+".pdf");        
    TLegend *leg_pos = new TLegend(0.83,0.9,0.7,0.7);
    TLegend *leg_neg = new TLegend(0.9,0.9,0.83,0.7);
    Int_t ps_count =0;

    // blocks
    
    TMultiGraph* mg_norm = new TMultiGraph();
    TLegend *leg_norm = new TLegend(0.9,0.9,0.8,0.7);
    vector<TGraphErrors*> fGraphArray(4);
    c1->cd();
    for(int iblk=0;iblk<4;iblk++){
      draw_cmd = Form("%s.block%d*%f:%s.block%d.error*%f:slug",
		      device_name.Data(),iblk,rescale,
		      device_name.Data(),iblk,rescale);
      channel_cut = Form("%s.block%d.error>0",device_name.Data(),iblk);
      
      if(device_name.Contains("bcm_an"))
        channel_cut += "&& !(slug>=13 && slug<=21)";
      if(device_name.Contains("cWS"))
        channel_cut += "&& !(slug==12)";

      TTree *this_tree = fTreeArray[iblk];
      this_tree->Draw(draw_cmd,sign_cut[iplot]+"&&"+channel_cut,"goff");
      TGraphErrors *ger_block = new TGraphErrors(this_tree->GetSelectedRows(),
						 this_tree->GetV3(),this_tree->GetV1(),
						 0,this_tree->GetV2());
      ger_block->SetMarkerStyle(21);
      ger_block->SetMarkerColor(fColor[iblk]);
      ger_block->SetLineColor(fColor[iblk]);
      ger_block->SetLineStyle(2);
      mg_norm->Add(ger_block,"LP");
      leg_norm->AddEntry(ger_block,Form("block%d",iblk));
      fGraphArray[iblk] = ger_block;
    }
    mg_norm->Draw("A");
    mg_norm->SetTitle(device_name+unit+";slug;"+unit);
    y_max = mg_norm->GetYaxis()->GetXmax();
    y_min = mg_norm->GetYaxis()->GetXmin();
    mg_norm->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));
    leg_norm->Draw("same");
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
    }
    c1->Print("prex_slug_average_Aq_byblock_"+label[iplot]+".pdf");    
    c2->Clear("D");
    for(int iblk=0;iblk<4;iblk++){
      c2->cd(iblk+1);
      TMultiGraph *mg_in_block = new TMultiGraph();
      TLegend *leg_block = new TLegend(0.9,0.6,1.0,0.9);
      draw_cmd = Form("%s.block%d*%f:%s.block%d.error*%f:slug",
		      device_name.Data(),iblk,rescale,
		      device_name.Data(),iblk,rescale);
      channel_cut = Form("%s.block%d.error>0",device_name.Data(),iblk);
      
      if(device_name.Contains("bcm_an"))
        channel_cut += "&& !(slug>=13 && slug<=21)";
      if(device_name.Contains("cWS"))
        channel_cut += "&& !(slug==12)";

      TTree *this_tree = fPosTreeArray[iblk];
      this_tree->Draw(draw_cmd,sign_cut[iplot]+"&&"+channel_cut,"goff");
      TGraphErrors *ger_pos_block = new TGraphErrors(this_tree->GetSelectedRows(),
						     this_tree->GetV3(),this_tree->GetV1(),
						     0,this_tree->GetV2());
      ger_pos_block->SetMarkerStyle(20);
      ger_pos_block->SetMarkerColor(fColor[iblk]);
      ger_pos_block->SetLineColor(fColor[iblk]);
      ger_pos_block->SetLineStyle(2);

      this_tree = fNegTreeArray[iblk];
      this_tree->Draw(draw_cmd,sign_cut[iplot]+"&&"+channel_cut,"goff");
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
    c2->Print("prex_slug_average_Aq_byblock_"+label[iplot]+".pdf");    
    c2->Clear("D");
    // Neutral pick-up in blocks
    for(int iblk=0;iblk<4;iblk++){
      c2->cd(iblk+1);
      draw_cmd = Form("%s.block%d*%f:%s.block%d.error*%f:slug",
		      device_name.Data(),iblk,rescale,
		      device_name.Data(),iblk,rescale);
      channel_cut = Form("%s.block%d.error>0",device_name.Data(),iblk);
      
      if(device_name.Contains("bcm_an"))
        channel_cut += "&& !(slug>=13 && slug<=21)";
      if(device_name.Contains("cWS"))
        channel_cut += "&& !(slug==12)";

      TTree *this_tree = fNullTreeArray[iblk];
      this_tree->Draw(draw_cmd,sign_cut[iplot]+"&&"+channel_cut,"goff");
      TGraphErrors *ger_block = new TGraphErrors(this_tree->GetSelectedRows(),
						 this_tree->GetV3(),this_tree->GetV1(),
						 0,this_tree->GetV2());
      ger_block->SetMarkerStyle(21);
      ger_block->SetMarkerColor(fColor[iblk]);
      ger_block->SetLineColor(fColor[iblk]);
      ger_block->SetLineStyle(2);
      ger_block->Draw("ALP");
      ger_block->SetTitle(device_name+Form(".block%d Neutral Pick-up",iblk)+unit+";slug;"+unit);
      y_max = ger_block->GetYaxis()->GetXmax();
      y_min = ger_block->GetYaxis()->GetXmin();
      ger_block->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));
      ger_block->Fit("pol0","Q");
    }

    c2->Print("prex_slug_average_Aq_byblock_"+label[iplot]+".pdf");
    }
    c1->Print("prex_slug_average_Aq_byblock_"+label[iplot]+".pdf]");
  }
}
