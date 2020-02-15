#include "SetAlias.C"

void PlotByTarget(){
  Int_t target[]={2,3,10,9,8,5,7,6};
  TString target_name[]={"208Pb2","208Pb3","208Pb10","208Pb9",
			 "208Pb8","208Pb5","208Pb7","208Pb6"};
  Int_t ntgt = sizeof(target)/sizeof(*target);

  SetAlias();
  TString cmd[] = {"us_avg_4aX/(ppm/um) : us_avg_4eX/(ppm/um):run",
		   "us_avg_4eX/(ppm/um):us_avg_11X/(ppm/um):run",
		   "us_avg_4aX/(ppm/um):us_avg_11X/(ppm/um):run",
		   "us_avg_4aY/(ppm/um):us_avg_4eY/(ppm/um):run",
		   "us_avg_4aX/(ppm/um):us_avg_4aY/(ppm/um):run",
		   "us_avg_4eX/(ppm/um):us_avg_4eY/(ppm/um):run",
		   "A_4eX/ppm:A_4aX/ppm:run",
		   "A_4eX/ppm:A_11X/ppm:run",
		   "A_4aX/ppm:A_11X/(ppm):run",
		   "A_4aY/(ppm):A_4eY/(ppm):run",
		   "A_4aX/(ppm):A_4aY/(ppm):run",
		   "A_4eX/(ppm):A_4eY/(ppm):run",
		   "D4eX/nm:D4aX/nm:run",
		   "D4eX/nm:D11X/nm:run",
		   "D4aX/nm:D11X/(nm):run",
		   "D4aY/(nm):D4eY/(nm):run",
		   "D4aX/(nm):D4aY/(nm):run",
		   "D4eX/(nm):D4eY/(nm):run" };


  
  
  TString title[] = {"slope_us_avg_4aX_vs_4eX",
		     "slope_us_avg_4eX_vs_11X",
		     "slope_us_avg_4aX_vs_11X",
		     "slope_us_avg_4aY_vs_4eY",
		     "slope_us_avg_4aX_vs_4aY",
		     "slope_us_avg_4eX_vs_4eY",
		     "correction_4aX_vs_4eX",
  		     "correction_4eX_vs_11X",
		     "correction_4aX_vs_11X",
		     "correction_4aY_vs_4eY",
		     "correction_4aX_vs_4aY",
		     "correction_4eX_vs_4eY",
		     "diff_4aX_vs_4eX",
  		     "diff_4eX_vs_11X",
		     "diff_4aX_vs_11X",
		     "diff_4aY_vs_4eY",
		     "diff_4aX_vs_4aY",
		     "diff_4eX_vs_4eY" };
  
  T->SetMarkerSize(0.7);
  
  TString cut = " isGood && arm_flag==0 && D11X.err>0";
  TCanvas *c1 = new TCanvas("c1","c1",1600,900);
  
  c1->Divide(4,2);
  Int_t nplot = sizeof(cmd)/sizeof(*cmd);
  for(int i=0;i<nplot;i++){
    c1->Clear("D");
    for(int itgt=0;itgt<ntgt;itgt++){
      c1->cd(itgt+1);

      T->Draw(cmd[i],cut + Form("&& target==%d",target[itgt]),"COLZ");
      TH2F *htemp = (TH2F*)gPad->FindObject("htemp");
      if(htemp!=NULL){
	htemp->SetTitle("");
	htemp->GetXaxis()->SetTitleSize(0.05);
	htemp->GetXaxis()->SetLabelSize(0.04);
	htemp->GetYaxis()->SetTitleSize(0.05);
	htemp->GetYaxis()->SetLabelSize(0.04);
	htemp->GetZaxis()->SetLabelSize(0.05);
      }
      gPad->SetRightMargin(0.2);
      gPad->SetLeftMargin(0.15);
    
      TPaveText *tpt = new TPaveText(0.55,0.9,0.85,0.99,"NDC");
      tpt->AddText(target_name[itgt]);
      tpt->Draw("same");
    }
    c1->SaveAs(Form("sys-cor-plots/%s.pdf",title[i].Data()));
  }
}
