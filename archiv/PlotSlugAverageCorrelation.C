void PlotSlugAverageCorrelation(){
  TString cut = "arm_flag==0 && diff_bpm12X==0";

  TString cmd[]={"sign*cor_bpm4aX/ppb:sign*cor_bpm4eX/ppb:slug",
		 "sign*cor_bpm4aX/ppb:sign*cor_bpm11X/ppb:slug",
		 "sign*cor_bpm4aX/ppb:sign*cor_bpm11X/ppb:slug",
		 "sign*cor_bpm4aY/ppb:sign*cor_bpm4eY/ppb:slug",
		 "diff_bpm4aX/nm:diff_bpm4eX/nm:slug",
		 "diff_bpm4aX/nm:diff_bpm11X/nm:slug",
		 "diff_bpm4eX/nm:diff_bpm11X/nm:slug",
		 "diff_bpm4aY/nm:diff_bpm4eY/nm:slug" };

  TString title[]={"correction_4aX_vs_4eX_by_slug",
		   "correction_4aX_vs_11X_by_slug",
		   "correction_4eX_vs_11X_by_slug",
		   "correction_4aY_vs_4eY_by_slug",
		   "diff_4aX_vs_4eX_by_slug",
		   "diff_4aX_vs_11X_by_slug",
		   "diff_4eX_vs_11X_by_slug",
		   "diff_4aY_vs_4eY_by_slug"};
  
  Int_t ncmd = sizeof(cmd)/sizeof(*cmd);
  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  c1->cd();
  c1->SetLeftMargin(0.15);
  c1->SetRightMargin(0.15);
  for(int i=0;i<ncmd;i++){
    grand->Draw(cmd[i],cut,"COLZ");
    c1->SaveAs(Form("sys-cor-plots/%s.pdf",title[i].Data()));
  }

}
