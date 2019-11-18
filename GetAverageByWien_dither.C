#include "lib/TaAccumulator.cc"
#include "lib/TaRunStats.cc"
#include "LoadNormalizationMap.C"
#include "lib/PlotPullFit.C"
#include "lib/TaResult.cc"
void GetAverageByWien_dither(Int_t arm_select);
void GetAverageByWien_dither(){
  GetAverageByWien_dither(-1);
  GetAverageByWien_dither(0);
  GetAverageByWien_dither(1);
  GetAverageByWien_dither(2);
}

void GetAverageByWien_dither(Int_t arm_select){
  TString slug_cut = "&& slug!=39 && slug!=72 && slug!=74";
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
  TString detector="primary_det";
  
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
  

  TString ihwp_text[]={"IN","OUT","IN","OUT",
		     "IN","OUT","IN","OUT"};
  TString wien_text[]={"RIGHT","RIGHT","LEFT","LEFT",
  		     "RIGHT","RIGHT","LEFT","LEFT"};
  
  Color_t color_code[8]={kGreen,kRed,kMagenta,kBlue,
			 kGreen,kRed,kMagenta,kBlue };
  Int_t style_code[8]={20,21,22,23,20,21,22,23};

  TFile *inputRF = TFile::Open("rootfiles/prex_grand_average_dither.root");
  TTree *grand_tree = (TTree*)inputRF->Get("grand");

  TString arm_cut="";
  TString output_label="";
  if(arm_select==0){
    arm_cut = "&&arm_flag==0";
    output_label = "_both-arm";
  }
  if(arm_select==1){
    arm_cut = "&&arm_flag==1";
    output_label = "_right-arm";
  }
  if(arm_select==2){
    arm_cut = "&&arm_flag==2";
    output_label = "_left-arm";
  }
  grand_tree->Draw(">>elist","primary_error>0"+arm_cut+slug_cut);
  TEventList* elist = (TEventList*)gDirectory->FindObject("elist");
  grand_tree->SetEventList(elist);
  TString outlog_filename = Form("output/averages_by_wien_maindet%s_dither.log",
				 output_label.Data());
  TaResult *fReport = new TaResult(outlog_filename);
  vector<TString> header{"IHWP,Wien","Mean","Error", "chi2/NDF"};
  fReport->AddHeader(header);
  Int_t nWien = sizeof(spin_cut)/sizeof(*spin_cut);
  Double_t *grand_y = new Double_t[nWien];
  Double_t *grand_yerr = new Double_t[nWien];
  Double_t *grand_x = new Double_t[nWien];
  for(int iwien=0;iwien<nWien;iwien++){
    fReport->AddLine();
    fReport->AddStringEntry(Form("%s,%s",
				 ihwp_text[iwien].Data(),
				 wien_text[iwien].Data()));

    Int_t npt = grand_tree->Draw(Form("sign*%s.mean/ppb:primary_error/ppb:slug",
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
    fReport->AddFloatEntry(f1->GetParameter(0));
    fReport->AddFloatEntry(f1->GetParError(0));
    fReport->AddStringEntry(Form("%.1f/%d",
				 f1->GetChisquare(),
				 f1->GetNDF()));
    fReport->AddLine();
    
    grand_y[iwien]=f1->GetParameter(0);
    grand_yerr[iwien]=f1->GetParError(0);
    grand_x[iwien]=iwien;
  }
  gStyle->SetOptFit(1);
  TCanvas *c1 = new TCanvas("c1","c1",1000,700);
  c1->cd();
  TString plot_title = Form("%s:Grand Wien Fit",detector.Data());
  TGraphErrors *mgall = new TGraphErrors(nWien,grand_x,grand_y,0,grand_yerr);
  mgall->SetMarkerStyle(20);
  mgall->SetMarkerSize(2);
  mgall->SetMarkerColor(kBlue);
  mgall->SetLineWidth(2);
  mgall->Draw("AP");
  TH1F *hgall = mgall->GetHistogram();
  hgall->GetXaxis()->Set(nWien,-0.5,nWien-0.5);
  for(int i=0;i<nWien;i++)
    hgall->GetXaxis()->SetBinLabel(i+1,ihwp_text[i]+", "+wien_text[i]);

  mgall->GetXaxis()->SetLabelSize(0.05);
  double ymax  = mgall->GetYaxis()->GetXmax();
  double ymin = mgall->GetYaxis()->GetXmin();
  mgall->GetYaxis()->SetRangeUser(ymin,ymax+0.1*(ymax-ymin));
  mgall->GetYaxis()->SetLabelSize(0.07);
  mgall->GetYaxis()->SetTitleSize(0.08);
  mgall->GetYaxis()->SetTitleOffset(1.0);
  mgall->SetTitle(";; Averages (ppb)");
  TF1 *f_avg = new TF1("f_avg","pol0",-10,10);
  mgall->Fit("f_avg");
  gPad->SetBorderMode(0);
  gPad->SetLeftMargin(0.2);
  c1->SetGridx();
  c1->SetGridy();
  c1->SaveAs(Form("WienFit/%s_grand_average%s.pdf",
		  detector.Data(),output_label.Data()));
  tsw.Print();
  fReport->AddLine();
  fReport->InsertHorizontalLine();
  fReport->AddStringEntry("Average");
  fReport->AddFloatEntry(f_avg->GetParameter(0));
  fReport->AddFloatEntry(f_avg->GetParError(0));
  fReport->AddStringEntry(Form("%.1f/%d",
			       f_avg->GetChisquare(),
			       f_avg->GetNDF()));
  fReport->InsertHorizontalLine();
  fReport->Print();
  fReport->Close();
}

