#include "TaAccumulator.cc"
#include "TaRunStats.cc"
#include "LoadNormalizationMap.C"
#include "PlotPullFit.C"

void GetAverageByWien_dither(){
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
  // TString detector="diff_bpm4aX";
  // TString raw_detector="asym_us_avg";
  
    TString spin_cut[8]={"ihwp==1 && wien==1 && slug<=44",
  		       "ihwp==-1 && wien==1 && slug<=44 ",
  		       "ihwp==1&&wien==-1 && slug<=44",
  		       "ihwp==-1&&wien==-1 && slug<=44",
  		       "ihwp==1 && wien==1 && slug>44",
  		       "ihwp==-1 && wien==1 && slug>44 ",
  		       "ihwp==1&&wien==-1 && slug>44",
  		       "ihwp==-1&&wien==-1 && slug>44"};
  
  TString legend_txt[8]={"IHWP In, First Flip-Right",
  			 "IHWP Out, First Flip-Right",
  			 "IHWP In, First Flip-Left",
  			 "IHWP Out, First Flip-Left",
  			 "IHWP In, Second Flip-Right",
  			 "IHWP Out, Second Flip-Right",
  			 "IHWP In, Second Flip-Left",
  			 "IHWP Out, Second Flip-Left"};
  
  // TString spin_cut[4]={"wien==1 && slug<=44",
  // 		       "wien==-1 && slug<=44 ",
  // 		       "wien==1 &&slug>44 ",
  // 		       "wien==-1&& slug>44"};
  
  // TString legend_txt[4]={"Flip-Right",
  // 			 "Flip-Left",
  // 			 "Flip-Right",
  // 			 "Flip-Left"};
  // TString wien_text[]={"RIGHT","LEFT",
  // 		       "RIGHT","LEFT"};

  TString ihwp_text[]={"IN","OUT","IN","OUT",
		     "IN","OUT","IN","OUT"};
  TString wien_text[]={"RIGHT","RIGHT","LEFT","LEFT",
  		     "RIGHT","RIGHT","LEFT","LEFT"};
  
  Color_t color_code[8]={kGreen,kRed,kMagenta,kBlue,
			 kGreen,kRed,kMagenta,kBlue };
  Int_t style_code[8]={20,21,22,23,20,21,22,23};

  TFile *inputRF = TFile::Open("prex_dither_grand_fit.root");
  TTree *grand_tree = (TTree*)inputRF->Get("grand");

  grand_tree->Draw(">>elist","error>0 && slug!=39 && slug!=72 && slug!=74");
  TEventList* elist = (TEventList*)gDirectory->FindObject("elist");
  grand_tree->SetEventList(elist);
  FILE *report = fopen("report_dither.log","w");
  Int_t nWien = sizeof(spin_cut)/sizeof(*spin_cut);
  Double_t *grand_y = new Double_t[nWien];
  Double_t *grand_yerr = new Double_t[nWien];
  Double_t *grand_x = new Double_t[nWien];
  for(int iwien=0;iwien<nWien;iwien++){
    fprintf(report,"%s,%s,",ihwp_text[iwien].Data(),wien_text[iwien].Data());
    Int_t npt = grand_tree->Draw(Form("spin*%s.mean/ppb:error/ppb:slug",
				      detector.Data()),
				 spin_cut[iwien].Data(),"goff");
    
    Double_t *y_ptr = grand_tree->GetV1();
    Double_t *yerr_ptr = grand_tree->GetV2();
    Double_t *x_ptr = grand_tree->GetV3();
    vector<Double_t> x_val(npt);
    vector<Double_t> y_val(npt);
    vector<Double_t> y_error(npt);
    for(int ipt=0;ipt<npt;ipt++){
      x_val[ipt] = x_ptr[ipt];
      y_val[ipt] = y_ptr[ipt];
      y_error[ipt] = yerr_ptr[ipt];
    }
    TString plot_title = Form("%s: %s",detector.Data(),legend_txt[iwien].Data());
    TF1 *f1 = PlotPullFit(y_val,y_error,x_val,plot_title);
    fprintf(report,"%.2f,%.2f,%.2f/%d\n",
	    f1->GetParameter(0),
	    f1->GetParError(0),
	    f1->GetChisquare(),
	    f1->GetNDF());
    
    grand_y[iwien]=f1->GetParameter(0);
    grand_yerr[iwien]=f1->GetParError(0);
    grand_x[iwien]=iwien;
  }
  gStyle->SetOptFit(1);
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  c1->cd();
  TString plot_title = Form("%s:Grand Wien Fit",detector.Data());
  // TGraphErrors *gone[nWien];
  // TMultiGraph *mgall = new TMultiGraph();
  TGraphErrors *mgall = new TGraphErrors(nWien,grand_x,grand_y,0,grand_yerr);
  // for(int i=0;i<nWien;i++){
  //   gone[i] = new TGraphErrors(1,&grand_x[i],&grand_y[i],0,&grand_yerr[i]);
  //   gone[i]->SetMarkerStyle(style_code[i]);
  //   gone[i]->SetMarkerColor(color_code[i]);
  //   mgall->Add(gone[i],"p");
  // }
  mgall->SetMarkerStyle(20);
  mgall->SetMarkerSize(2);
  mgall->SetMarkerColor(kBlue);
  mgall->SetLineWidth(2);
  mgall->Draw("AP");
  TH1F *hgall = mgall->GetHistogram();
  hgall->GetXaxis()->Set(nWien,-0.5,nWien-0.5);
  for(int i=0;i<nWien;i++)
    hgall->GetXaxis()->SetBinLabel(i+1,ihwp_text[i]+", "+wien_text[i]);
  
  mgall->GetXaxis()->SetLabelSize(0.07);
  double ymax  = mgall->GetYaxis()->GetXmax();
  double ymin = mgall->GetYaxis()->GetXmin();
  mgall->GetYaxis()->SetRangeUser(ymin,ymax+0.1*(ymax-ymin));
  mgall->GetYaxis()->SetLabelSize(0.07);
  mgall->GetYaxis()->SetTitleSize(0.08);
  mgall->GetYaxis()->SetTitleOffset(1.0);
  mgall->SetTitle(";; Averages (ppb)");
  mgall->Fit("pol0");
  gPad->SetBorderMode(0);
  gPad->SetLeftMargin(0.2);
  c1->SetGridx();
  c1->SetGridy();
  tsw.Print();
}

