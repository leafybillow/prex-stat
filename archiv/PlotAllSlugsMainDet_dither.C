#include "TaAccumulator.cc"
#include "TaRunStats.cc"
#include "LoadNormalizationMap.C"
#include "PlotPullFit.C"

void PlotAllSlugsMainDet_dither(){
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
  TString detector="dit_asym_us_avg";
  
  TFile *inputRF = TFile::Open("prex_dither_grand_fit.root");
  TTree *grand_tree = (TTree*)inputRF->Get("grand");
  
  TString spin_cut[4]={"ihwp==1 && wien==1",
		       "ihwp==-1 && wien==1 ",
		       "ihwp==1&&wien==-1",
		       "ihwp==-1&&wien==-1"};
  
  TString legend_txt[4]={"IHWP In, Flip-Right",
			 "IHWP Out, Flip-Right",
			 "IHWP In, Flip-Left",
			 "IHWP Out, Flip-Left"};
  
  Color_t color_code[4]={kGreen,kRed,kMagenta,kBlue};
  Int_t style_code[4]={20,21,22,23};

  grand_tree->Draw(">>elist","error>0 && slug!=39 && slug!=72 && slug!=74");
  TEventList* elist = (TEventList*)gDirectory->FindObject("elist");
  grand_tree->SetEventList(elist);
  FILE *report = fopen("Anull_dither.log","w");
  TMultiGraph *mge = new TMultiGraph();
  Int_t npt = grand_tree->Draw(Form("%s.mean/ppb:error/ppb:slug",
				    detector.Data()),"","goff");
  TF1* f1 =new TF1("f1","pol0",-100,100);
  Double_t *y_ptr = grand_tree->GetV1();
  Double_t *yerr_ptr = grand_tree->GetV2();
  Double_t *x_ptr = grand_tree->GetV3();
  gStyle->SetOptFit(1);
  TGraphErrors *gefit = new TGraphErrors(npt,x_ptr,y_ptr,0,yerr_ptr);
  mge->Add(gefit,"p");
  TLegend *leg = new TLegend(0.1,0.9,0.5,0.7);  
  for(int iwien=0;iwien<4;iwien++){
    Int_t this_npt = grand_tree->Draw(Form("%s.mean/ppb:error/ppb:slug",
  				      detector.Data()),
  				 spin_cut[iwien].Data(),"goff");
    
    Double_t *y_ptr2 = grand_tree->GetV1();
    Double_t *yerr_ptr2 = grand_tree->GetV2();
    Double_t *x_ptr2 = grand_tree->GetV3();
    TGraphErrors *this_ge = new TGraphErrors(this_npt,x_ptr2,y_ptr2,0,yerr_ptr2);
    this_ge->SetMarkerColor(color_code[iwien]);
    this_ge->SetMarkerStyle(style_code[iwien]);
    mge->Add(this_ge,"p");
    leg->AddEntry(this_ge,legend_txt[iwien],"p");
  }
  mge->Draw("A");
  // gefit->Fit("f1","QR","",0,96);  
  Double_t y_max = mge->GetYaxis()->GetXmax();
  Double_t y_min = mge->GetYaxis()->GetXmin();
  mge->GetYaxis()->SetRangeUser(y_min,y_max+0.2*(y_max-y_min));
  mge->GetYaxis()->SetTitle(" Asymmetry (ppb)");
  mge->GetYaxis()->SetTitleOffset(1.1);
  mge->GetXaxis()->SetTitle("Slug Number");
  leg->Draw("same");
  mge->SetTitle("Dithering Correction");
  gPad->Modified();
  gPad->Update();

  tsw.Print();
}

