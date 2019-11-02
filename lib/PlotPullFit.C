TF1* PlotPullFit(vector<Double_t> y_val, vector<Double_t> y_err,
		 vector<Double_t> x_val,
		 TString title){

  Int_t npt = x_val.size();
  Double_t *x_array = new Double_t[npt];
  Double_t *y_array = new Double_t[npt];
  Double_t *yerr_array = new Double_t[npt];
  for(int i=0;i<npt;i++){
    x_array[i]=i;
    y_array[i]=y_val[i];
    yerr_array[i]=y_err[i];
  }

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
    
  TCanvas c1("c1","c1",1100,600);
  c1.cd();
  c1.Clear();

  double ySep = 0.3;
  double xSep = 0.65;

  TPad p1("p1","",0,ySep,xSep,1);
  p1.Draw();
  TPad p2("p2","",0,0,xSep,ySep);
  p2.Draw();
  TPad p3("p3","",xSep,0,1,1);
  p3.Draw();
  
  p1.cd();
  p1.UseCurrentStyle();
  p1.SetBottomMargin(0);

  TF1 *f1 = new TF1("f1","pol0",-1e3,1e3);
  TF1 *fg = new TF1("fg","gaus",-1e3,1e3);
  TGraphErrors *tge = new TGraphErrors(npt,x_array,y_array,0,yerr_array);
  tge->Draw("AP");
  tge->SetMarkerStyle(20);
  tge->Fit("f1","Q");
  tge->SetTitle(title);

  Double_t fit_mean = f1->GetParameter(0);
  TH1F *htge = tge->GetHistogram();
  htge->GetXaxis()->Set(npt,-0.5,npt-0.5);
  for(int ibin=1;ibin<=npt;ibin++)
    htge->GetXaxis()->SetBinLabel(ibin,Form("%.1f",x_val[ibin-1]));
  TH1D hPull("hPull","",npt,-0.5,npt-0.5);
  TH1D hPull1D("hPull1D","1D pull distribution",30,-8,8);
  
  for(int i=0;i<npt;i++){
    double val = (y_array[i]-fit_mean)/yerr_array[i];
    int ibin = x_array[i]+1;
    hPull.SetBinContent(ibin,val);
    hPull.GetXaxis()->SetBinLabel(ibin,htge->GetXaxis()->GetBinLabel(ibin));
    hPull1D.Fill(val);
  }
    
  p2.cd();
  p2.SetTopMargin(0);
  p2.SetBottomMargin(0.3);
  hPull.GetYaxis()->SetLabelSize(0.1);
  hPull.GetXaxis()->SetLabelSize(0.1);
  hPull.SetFillColor(kGreen-3);
  hPull.SetStats(0);
  hPull.GetXaxis()->LabelsOption("v");
  hPull.DrawCopy("b");
  p2.SetGridx(1);
  p2.SetGridy(1);

  
  gStyle->SetOptFit(1);
  gStyle->SetOptStat("MRou");
  p3.cd();
  p3.UseCurrentStyle();
  fg->SetParameters(hPull1D.GetMaximum()*0.8,hPull1D.GetMean(),hPull1D.GetRMS());
  hPull1D.Fit(fg->GetName(),"Q");
  hPull1D.DrawCopy();
  fg->Draw("same");
  
  title.ReplaceAll(' ','_');
  title.ReplaceAll(",",'_');
  title.ReplaceAll(':','_');
  title.ReplaceAll("/","_");

  c1.SaveAs("/home/yetao/workarea/prex-statistics/SlugFit/"+title+".pdf");

  return f1;
}
