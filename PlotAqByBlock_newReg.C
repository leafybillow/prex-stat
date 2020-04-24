void PlotAqByBlock_newReg(){
  gStyle->SetOptFit(1);
  TFile* block0_file = TFile::Open("prex_grand_average_block0_newReg.root");
  TTree *block0_tree = (TTree*)block0_file->Get("slug");
  TFile* block1_file = TFile::Open("prex_grand_average_block1_newReg.root");
  TTree *block1_tree = (TTree*)block1_file->Get("slug");
  TFile* block2_file = TFile::Open("prex_grand_average_block2_newReg.root");
  TTree *block2_tree = (TTree*)block2_file->Get("slug");
  TFile* block3_file = TFile::Open("prex_grand_average_block3_newReg.root");
  TTree *block3_tree = (TTree*)block3_file->Get("slug");

  vector<TString> fDetectorNameList={"asym_bcm_an_us","asym_bcm_an_ds",
  				     "asym_bcm_an_ds3","asym_bcm_dg_us",
  				     "asym_bcm_dg_ds","asym_cav4cQ"};
  TTree* fTreeArray[4] = {block0_tree,block1_tree,block2_tree,block3_tree};
  Color_t fColor[4]={kBlack,kBlue,kRed,kMagenta};
  Int_t nDet = fDetectorNameList.size();
  TCanvas *c1 = new TCanvas("c1","c1",1000,600);
  
  TString sign_cut[] = {"ihwp==\"IN\"","ihwp==\"OUT\""};
  TString label[] = {"ihwp_in","ihwp_out"};

  Int_t nplots = sizeof(sign_cut)/sizeof(*sign_cut);
  for(int iplot=0;iplot<nplots;iplot++){
    c1->cd();
    c1->Print("prex_grand_average_byblock_"+label[iplot]+"_newReg.pdf[");
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
      TLegend *leg = new TLegend(0.9,0.9,0.8,0.7);
      vector<TGraphErrors*> fGraphArray(4);
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
	ger_block->SetMarkerStyle(20);
	ger_block->SetMarkerColor(fColor[iblk]);
	ger_block->SetLineColor(fColor[iblk]);
	ger_block->SetLineStyle(2);
	mg->Add(ger_block,"LP");
	leg->AddEntry(ger_block,Form("block%d",iblk));
	fGraphArray[iblk] = ger_block;
	
      }
      
      mg->Draw("A");
      mg->SetTitle(device_name+unit+";slug;"+unit);
      double y_max = mg->GetYaxis()->GetXmax();
      double y_min = mg->GetYaxis()->GetXmin();
      mg->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));
      Int_t ps_count =0;
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
      
      c1->Print("prex_grand_average_byblock_"+label[iplot]+"_newReg.pdf");
    }
    c1->Print("prex_grand_average_byblock_"+label[iplot]+"_newReg.pdf]");
  }
}
