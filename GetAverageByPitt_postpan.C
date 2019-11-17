#include "lib/TaAccumulator.cc"
#include "lib/TaRunStats.cc"
#include "LoadNormalizationMap.C"
#include "lib/PlotPullFit.C"
#include "lib/TaResult.cc"
void GetAverageByPitt_postpan(Int_t arm_select);
void GetAverageByPitt_postpan(){
  GetAverageByPitt_postpan(-1);
  // GetAverageByPitt_postpan(0);
  // GetAverageByPitt_postpan(1);
  // GetAverageByPitt_postpan(2);
}

void GetAverageByPitt_postpan(Int_t arm_select){
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
  
  
  TFile *inputRF = TFile::Open("rootfiles/prex_grand_average_postpan.root");
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
  grand_tree->Draw(">>elist","primary_error>0"+arm_cut);
  TEventList* elist = (TEventList*)gDirectory->FindObject("elist");
  grand_tree->SetEventList(elist);
  TString outlog_filename = Form("output/averages_by_pitt_maindet%s.log",
				 output_label.Data());
  TaResult *fReport = new TaResult(outlog_filename);
  vector<TString> header{"Pitt","Mean","Error", "chi2/NDF"};
  fReport->AddHeader(header);
  Int_t nPitts = 23;
  Double_t *grand_y = new Double_t[nPitts];
  Double_t *grand_yerr = new Double_t[nPitts];
  Double_t *grand_x = new Double_t[nPitts];
  
  for(int ipitt=0;ipitt<nPitts;ipitt++){
    fReport->AddLine();
    fReport->AddStringEntry(Form("%d",ipitt+1));

    Int_t npt = grand_tree->Draw(Form("sign*%s.mean/ppb:primary_error/ppb:slug",
				      detector.Data()),
				 Form("pitt==%d",ipitt+1),"goff");
    
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
    TString plot_title = Form("%s: Pitt %d",detector.Data(),ipitt+1);
    TF1 *f1 = PlotPullFit(y_val,y_error,x_val,plot_title);
    fReport->AddFloatEntry(f1->GetParameter(0));
    fReport->AddFloatEntry(f1->GetParError(0));
    fReport->AddStringEntry(Form("%.1f/%d",
				 f1->GetChisquare(),
				 f1->GetNDF()));
    fReport->AddLine();
    
    grand_y[ipitt]=f1->GetParameter(0);
    grand_yerr[ipitt]=f1->GetParError(0);
    grand_x[ipitt]=ipitt+1;
  }
  gStyle->SetOptFit(1);
  TCanvas *c1 = new TCanvas("c1","c1",1000,700);
  c1->cd();
  TString plot_title = Form("%s:Grand Pitts Fit",detector.Data());
  PlotPullFit(grand_y,grand_yerr,grand_x,nPitts,plot_title);
  TGraphErrors *mgall = new TGraphErrors(nPitts,grand_x,grand_y,0,grand_yerr);
  mgall->SetMarkerStyle(20);
  mgall->SetMarkerSize(2);
  mgall->SetMarkerColor(kBlue);
  mgall->SetLineWidth(2);
  mgall->Draw("AP");
  TH1F *hgall = mgall->GetHistogram();
  hgall->GetXaxis()->Set(nPitts,0.5,nPitts+0.5);
  for(int i=0;i<nPitts;i++)
    hgall->GetXaxis()->SetBinLabel(i+1,Form("%d",i+1));

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
  c1->SaveAs(Form("PittFits/%s_grand_average%s.pdf",
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

