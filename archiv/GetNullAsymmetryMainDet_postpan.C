#include "lib/TaRunStats.cc"
#include "lib/TaResult.cc"
#include "LoadNormalizationMap.C"
#include "lib/PlotPullFit.C"
void GetNullAsymmetryMainDet_postpan(Int_t arm_select);
void GetNullAsymmetryMainDet_postpan(){
  GetNullAsymmetryMainDet_postpan(-1);
  GetNullAsymmetryMainDet_postpan(0);
  GetNullAsymmetryMainDet_postpan(1);
  GetNullAsymmetryMainDet_postpan(2);
}
void GetNullAsymmetryMainDet_postpan(Int_t arm_select){
  TStopwatch tsw;
  TString detector="primary_det";

  TString in_cut[4]={"ihwp==1 && wien==1 && slug<=44",
		     "ihwp==1 && wien==-1 && slug<=44 ",
		     "ihwp==1 &&wien==1 &&slug>44 ",
		     "ihwp==1 && wien==-1&& slug>44"};
  
  TString out_cut[4]={"ihwp==-1&& wien==1 && slug<=44",
		      "ihwp==-1 && wien==-1 && slug<=44 ",
		      "ihwp==-1 && wien==1 &&slug>44 ",
		      "ihwp==-1 && wien==-1&& slug>44"};
  
  TString legend_txt[4]={"Flip-Right",
  			 "Flip-Left",
  			 "Flip-Right",
  			 "Flip-Left"};
  TString wien_text[4]={"RIGHT","LEFT",
  		       "RIGHT","LEFT"};

  Double_t *Anull_mean = new Double_t[4];
  Double_t *Anull_error = new Double_t[4];
  
  Double_t *Ain_mean = new Double_t[4];
  Double_t *Ain_error = new Double_t[4];
  
  Double_t *Aout_mean = new Double_t[4];
  Double_t *Aout_error = new Double_t[4];

  Color_t color_code[8]={kGreen,kRed,kMagenta,kBlue,
			 kGreen,kRed,kMagenta,kBlue };
  Int_t style_code[8]={20,21,22,23,20,21,22,23};

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

  TString outlog_filename = Form("output/null_asym_by_wien_maindet%s.log",
				 output_label.Data());
  TaResult *fReport = new TaResult(outlog_filename);

  vector<TString> header{"IHWP,Wien","Mean","Error", "chi2/NDF"};
  fReport->AddHeader(header);

  Int_t nWien = sizeof(in_cut)/sizeof(*in_cut);
  Double_t *grand_x = new Double_t[nWien];
  for(int iwien=0;iwien<nWien;iwien++){

    //// IN
    fReport->AddLine();
    fReport->AddStringEntry("IN,"+wien_text[iwien]);
    Int_t npt = grand_tree->Draw(Form("%s.mean/ppb:primary_error/ppb:slug",
				      detector.Data()),
				 in_cut[iwien].Data(),"goff");
    
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
      
    Ain_mean[iwien]=f1->GetParameter(0);
    Ain_error[iwien]=f1->GetParError(0);
    fReport->AddFloatEntry(Ain_mean[iwien]);
    fReport->AddFloatEntry(Ain_error[iwien]);
    fReport->AddStringEntry(Form("%.1f/%d",
				 f1->GetChisquare(),
				 f1->GetNDF()));

    //// OUT
    fReport->AddLine();
    fReport->AddStringEntry("OUT,"+wien_text[iwien]);

    Int_t npt_out = grand_tree->Draw(Form("%s.mean/ppb:primary_error/ppb:slug",
					  detector.Data()),
				     out_cut[iwien].Data(),"goff");
    
    Double_t *yout_ptr = grand_tree->GetV1();
    Double_t *youterr_ptr = grand_tree->GetV2();
    Double_t *xout_ptr = grand_tree->GetV3();
    vector<Double_t> xout_val(npt_out);
    vector<Double_t> yout_val(npt_out);
    vector<Double_t> yout_error(npt_out);
    for(int ipt=0;ipt<npt_out;ipt++){
      xout_val[ipt] = xout_ptr[ipt];
      yout_val[ipt] = yout_ptr[ipt];
      yout_error[ipt] = youterr_ptr[ipt];
    }
    TString plot_title_out = Form("%s: %s",detector.Data(),legend_txt[iwien].Data());
    TF1 *f1out = PlotPullFit(yout_val,yout_error,xout_val,plot_title_out);
      
    Aout_mean[iwien]=f1out->GetParameter(0);
    Aout_error[iwien]=f1out->GetParError(0);
    fReport->AddFloatEntry(Aout_mean[iwien]);
    fReport->AddFloatEntry(Aout_error[iwien]);
    fReport->AddStringEntry(Form("%.1f/%d",
				 f1out->GetChisquare(),
				 f1out->GetNDF()));
    //
    
    Anull_mean[iwien]=0.5*(Ain_mean[iwien]+Aout_mean[iwien]);
    Anull_error[iwien]= 0.5*(sqrt(pow(Ain_error[iwien],2)+pow(Aout_error[iwien],2)));
    grand_x[iwien]=iwien;
    fReport->AddLine();
    fReport->AddStringEntry("Null");
    fReport->AddFloatEntry(Anull_mean[iwien]);
    fReport->AddFloatEntry(Anull_error[iwien]);
    fReport->AddStringEntry(" ");
    fReport->InsertHorizontalLine();
  }
  gStyle->SetOptFit(1);
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  c1->cd();
  TString plot_title = Form("%s:Grand Wien Fit",detector.Data());
  TGraphErrors *mgall = new TGraphErrors(nWien,grand_x,Anull_mean,0,Anull_error);
  mgall->SetMarkerStyle(20);
  mgall->SetMarkerSize(2);
  mgall->SetMarkerColor(kBlue);
  mgall->SetLineWidth(2);
  mgall->Draw("AP");
  TH1F *hgall = mgall->GetHistogram();
  hgall->GetXaxis()->Set(nWien,-0.5,nWien-0.5);
  for(int i=0;i<nWien;i++)
    hgall->GetXaxis()->SetBinLabel(i+1,wien_text[i]);

  mgall->GetXaxis()->SetLabelSize(0.07);
  double ymax  = mgall->GetYaxis()->GetXmax();
  double ymin = mgall->GetYaxis()->GetXmin();
  mgall->GetYaxis()->SetRangeUser(ymin,ymax+0.1*(ymax-ymin));
  mgall->GetYaxis()->SetLabelSize(0.07);
  mgall->GetYaxis()->SetTitleSize(0.08);
  mgall->GetYaxis()->SetTitleOffset(1.0);
  mgall->SetTitle(";; Null Asymmetry (ppb)");
  TF1 *fpol0 = new TF1("fpol0","pol0",-1e3,1e3);
  mgall->Fit("fpol0");

  fReport->InsertHorizontalLine();
  fReport->AddLine();
  fReport->AddStringEntry("Average");
  fReport->AddFloatEntry(fpol0->GetParameter(0));
  fReport->AddFloatEntry(fpol0->GetParError(0));
  
  fReport->AddStringEntry(Form("%.1f/%d",
			       fpol0->GetChisquare(),
			       fpol0->GetNDF()));
  fReport->InsertHorizontalLine();
  
  gPad->SetBorderMode(0);
  gPad->SetLeftMargin(0.2);
  c1->SetGridx();
  c1->SetGridy();
  c1->SaveAs(Form("WienFit/%s_null_asym.pdf",detector.Data()));
  tsw.Print();
  
  fReport->Print();
  fReport->Close();
}

