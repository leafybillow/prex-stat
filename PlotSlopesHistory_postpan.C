#include "TaAccumulator.cc"
#include "TaRunStats.cc"
#include "LoadNormalizationMap.C"
#include "PlotPullFit.C"

void PlotSlopesHistory_postpan(){
  TStopwatch tsw;

  TFile* input = TFile::Open("./prex_postpan_slopes.root");
  TTree* slopes_tree = (TTree*)input->Get("grand");
  
  TString detector="asym_us_avg_";
  TString bpm_array[]={"diff_bpm4aX","diff_bpm4eX",
		       "diff_bpm4aY","diff_bpm4eY",
		       "diff_bpm11X","diff_bpm12X"};
  for(int ibpm=0;ibpm<4;ibpm++){
    TString chname = detector+bpm_array[ibpm];
    TCanvas c1("c1","c1",800,300);
    c1.cd();
    c1.SetGridx();
    Int_t npt1= slopes_tree->Draw(Form("%s/(ppm/um):Entry$",chname.Data()),
			       "arm_flag==0","goff");
    Double_t *bpm_val = slopes_tree->GetV1();
    Double_t *bpm_slug = slopes_tree->GetV2();
    TGraph *g1 = new TGraph(npt1,bpm_slug,bpm_val);
    g1->SetMarkerStyle(7);
    g1->Draw("AP");
    g1->SetTitle(";;Slopes (ppm/um)");
    g1->GetYaxis()->SetTitleSize(0.09);
    g1->GetYaxis()->SetLabelSize(0.08);
    g1->GetYaxis()->SetTitleOffset(0.4);
    TPaveText *pt = new TPaveText(0.6,0.95,0.95,0.75,"blNDC");
    pt->AddText(chname);
    pt->Draw();
    c1.SaveAs(Form("%s_history.pdf",chname.Data()));
  }

  TCanvas c1("c1","c1",800,300);
  c1.cd();
  c1.SetGridx();
  TString chname;
  chname = detector+"diff_bpm11X";
  Int_t npt11X= slopes_tree->Draw(Form("%s/(ppm/um):Entry$",chname.Data()),
				  Form("arm_flag==0 && %s!=0",chname.Data()),"goff");
  Double_t *bpm11X_val = slopes_tree->GetV1();
  Double_t *bpm11X_slug = slopes_tree->GetV2();
  TGraph *g11X = new TGraphErrors(npt11X,bpm11X_slug,bpm11X_val);
  g11X->SetMarkerStyle(7);
  g11X->SetMarkerColor(kMagenta);

  chname = detector+"diff_bpm12X";
  Int_t npt12X= slopes_tree->Draw(Form("%s/(ppm/um):Entry$",chname.Data()),
				  Form("arm_flag==0 && %s!=0",chname.Data()),"goff");
  Double_t *bpm12X_val = slopes_tree->GetV1();
  Double_t *bpm12X_slug = slopes_tree->GetV2();
  TGraphErrors *g12X = new TGraphErrors(npt12X,bpm12X_slug,bpm12X_val);
  g12X->SetMarkerStyle(7);
  g12X->SetMarkerColor(kBlue);

  TMultiGraph *mge_mean = new TMultiGraph();
  mge_mean->Add(g11X,"p");
  mge_mean->Add(g12X,"p");
  mge_mean->Draw("A");
  mge_mean->SetTitle(";;Slopes (ppm/um)");
  mge_mean->GetYaxis()->SetTitleSize(0.09);
  mge_mean->GetYaxis()->SetLabelSize(0.08);
  mge_mean->GetYaxis()->SetTitleOffset(0.45);

  TPaveText *pt = new TPaveText(0.6,1,1,0.75,"blNDC");
  pt->AddText("asym_us_avg_diff_bpm12X"); ((TText*)pt->GetListOfLines()->Last())->SetTextColor(kBlue);
  pt->AddText("asym_us_avg_diff_bpm11X");((TText*)pt->GetListOfLines()->Last())->SetTextColor(kMagenta);
  pt->Draw();

  c1.SaveAs(Form("asym_us_avg_diff_dispersive_slope_history.pdf"));

  tsw.Print();
}

