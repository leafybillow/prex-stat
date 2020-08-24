#include "device_list.hh"
void PlotByBlock_newReg(){
  gStyle->SetOptFit(1);
  TFile* block0_file = TFile::Open("prex_grand_average_block0_newReg.root");
  TTree *block0_tree = (TTree*)block0_file->Get("slug");
  TFile* block1_file = TFile::Open("prex_grand_average_block1_newReg.root");
  TTree *block1_tree = (TTree*)block1_file->Get("slug");
  TFile* block2_file = TFile::Open("prex_grand_average_block2_newReg.root");
  TTree *block2_tree = (TTree*)block2_file->Get("slug");
  TFile* block3_file = TFile::Open("prex_grand_average_block3_newReg.root");
  TTree *block3_tree = (TTree*)block3_file->Get("slug");

  // vector<TString> fDetectorNameList={"asym_bcm_an_us","asym_bcm_an_ds",
  // 				     "asym_bcm_an_ds3","asym_bcm_dg_us",
  // 				     "asym_bcm_dg_ds","asym_cav4cQ"};

  vector<TString> fDetectorNameList={"Adet","Aq",
  				     "reg_asym_us_avg","reg_asym_usr","reg_asym_usl",
  				     "reg_asym_us_dd","reg_asym_ds_avg",
  				     "asym_us_avg","asym_usr","asym_usl",
  				     "asym_us_dd","asym_ds_avg",
  				     "asym_bcm_an_us","asym_bcm_an_ds",
  				     "asym_bcm_an_ds3","asym_bcm_dg_us",
  				     "asym_bcm_dg_ds",
  				     "diff_bpm4aX","diff_bpm4eX",
  				     "diff_bpm4aY","diff_bpm4eY",
  				     "diff_bpmE","diff_bpm12X"};

  Int_t nDet = fDetectorNameList.size();
  TCanvas *c1 = new TCanvas("c1","c1",1000,600);
  
  // TString sign_cut[] = {"ihwp==\"IN\"","ihwp==\"OUT\""};
  // TString label[] = {"ihwp_in","ihwp_out"};
  TString sign_cut[] = {"sign>0 && slug!=12","sign<0 && slug!=13"};
  TString label[] = {"left_out_right_in","left_in_right_out"};

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
      TLegend *leg = new TLegend(0.9,0.9,0.7,0.7);
      // block 0
      draw_cmd = Form("%s_block%d*%f:%s_block%d.error*%f:slug",
		      device_name.Data(),0,rescale,
		      device_name.Data(),0,rescale);
      channel_cut = Form(" && %s_block%d.error>0",device_name.Data(),0);
      if(device_name.Contains("bcm_an"))
	channel_cut += "&& !(slug>=13 && slug<=21)";

      block0_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_block0 = new TGraphErrors(block0_tree->GetSelectedRows(),
						  block0_tree->GetV3(),block0_tree->GetV1(),
						  0,block0_tree->GetV2());
      ger_block0->SetMarkerStyle(20);
      ger_block0->SetMarkerColor(kBlue);
      ger_block0->SetLineColor(kBlue);
      mg->Add(ger_block0,"LP");
      leg->AddEntry(ger_block0,"block0");
      // block1
      draw_cmd = Form("%s_block%d*%f:%s_block%d.error*%f:slug",
		      device_name.Data(),1,rescale,
		      device_name.Data(),1,rescale);
      channel_cut = Form(" && %s_block%d.error>0",device_name.Data(),1);
      if(device_name.Contains("bcm_an"))
	channel_cut += "&& !(slug>=13 && slug<=21)";

      block1_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_block1 = new TGraphErrors(block1_tree->GetSelectedRows(),
						  block1_tree->GetV3(),block1_tree->GetV1(),
						  0,block1_tree->GetV2());
      ger_block1->SetMarkerStyle(20);
      ger_block1->SetMarkerColor(kRed);
      ger_block1->SetLineColor(kRed);
      mg->Add(ger_block1,"LP");
      leg->AddEntry(ger_block1,"block1");
      
      // block2
      draw_cmd = Form("%s_block%d*%f:%s_block%d.error*%f:slug",
		      device_name.Data(),2,rescale,
		      device_name.Data(),2,rescale);
      channel_cut = Form(" && %s_block%d.error>0",device_name.Data(),2);
      if(device_name.Contains("bcm_an"))
	channel_cut += "&& !(slug>=13 && slug<=21)";
      block2_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_block2 = new TGraphErrors(block2_tree->GetSelectedRows(),
						  block2_tree->GetV3(),block2_tree->GetV1(),
						  0,block2_tree->GetV2());
      ger_block2->SetMarkerStyle(20);
      ger_block2->SetMarkerColor(kBlack);
      ger_block2->SetLineColor(kBlack);
      mg->Add(ger_block2,"LP");
      leg->AddEntry(ger_block2,"block2");
      
      // block3
      draw_cmd = Form("%s_block%d*%f:%s_block%d.error*%f:slug",
		      device_name.Data(),3,rescale,
		      device_name.Data(),3,rescale);
      channel_cut = Form(" && %s_block%d.error>0",device_name.Data(),3);
      if(device_name.Contains("bcm_an"))
	channel_cut += "&& !(slug>=13 && slug<=21)";
      block3_tree->Draw(draw_cmd,sign_cut[iplot]+channel_cut,"goff");
      TGraphErrors *ger_block3 = new TGraphErrors(block3_tree->GetSelectedRows(),
						  block3_tree->GetV3(),block3_tree->GetV1(),
						  0,block3_tree->GetV2());
      ger_block3->SetMarkerStyle(20);
      ger_block3->SetMarkerColor(kMagenta);
      ger_block3->SetLineColor(kMagenta);
      mg->Add(ger_block3,"LP");
      leg->AddEntry(ger_block3,"block3");
      
      mg->Draw("A");
      mg->SetTitle(device_name+unit+";slug;"+unit);
      double y_max = mg->GetYaxis()->GetXmax();
      double y_min = mg->GetYaxis()->GetXmin();
      mg->GetYaxis()->SetRangeUser(y_min, y_max+0.33*(y_max-y_min));

      leg->Draw("same");
      
      c1->Print("prex_grand_average_byblock_"+label[iplot]+"_newReg.pdf");
    }
    c1->Print("prex_grand_average_byblock_"+label[iplot]+"_newReg.pdf]");
  }
}
