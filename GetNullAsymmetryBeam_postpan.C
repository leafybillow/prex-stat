#include "TaAccumulator.cc"
#include "TaRunStats.cc"
#include "LoadNormalizationMap.C"
#include "PlotPullFit.C"

void GetNullAsymmetryBeam_postpan(){
  TStopwatch tsw;
  
  TString beam_array[]={"diff_bpm4aX/nm","diff_bpm4eX/nm",
			"diff_bpm4aY/nm","diff_bpm4eY/nm",
			"diff_bpmE/nm","Aq/ppb","cor_beam/ppb",
			"cor_bpm4aX/ppb","cor_bpm4eX/ppb",
			"cor_bpm4aY/ppb","cor_bpm4eY/ppb",
			"cor_bpmE/ppb"};
  
  Int_t nDev = sizeof(beam_array)/sizeof(*beam_array);

  TString detector="reg_asym_us_avg";
  
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

  TFile *inputRF = TFile::Open("prex_grand_postpan_fit.root");
  TTree *grand_tree = (TTree*)inputRF->Get("grand");
  grand_tree->SetAlias("diff_bpmE","diff_bpm11X+diff_bpm12X");
  grand_tree->SetAlias("cor_bpmE","cor_bpm11X+cor_bpm12X");
  grand_tree->SetAlias("cor_beam","cor_bpm4aX+cor_bpm4eX+cor_bpm4aY+cor_bpm4eY+cor_bpm11X+cor_bpm12X");
  grand_tree->Draw(">>elist","error>0");
  TEventList* elist = (TEventList*)gDirectory->FindObject("elist");
  grand_tree->SetEventList(elist);
  FILE *report = fopen("NullAsymBeam.log","w");
  fprintf(report,"\t\t");
  for(int ich=0;ich<nDev;ich++)
    fprintf(report,"%s,",beam_array[ich].Data());
  fprintf(report,"\n");

  vector<Double_t> fBeamAnull(nDev);
  vector<Double_t> fBeamAnullError(nDev);
  Double_t total_weight;
  
  for(int iwien=0;iwien<8;iwien++){
    fprintf(report,"%s,%s,",ihwp_text[iwien].Data(),wien_text[iwien].Data());
    TString chname;
    for(int ich=0;ich<nDev;ich++){
      chname=beam_array[ich];
      Int_t npt = grand_tree->Draw(Form("%s:error/ppb:slug",
					chname.Data()),
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
      TString plot_title = Form("%s: %s",chname.Data(),legend_txt[iwien].Data());
      TF1 *f1 = PlotPullFit(y_val,y_error,x_val,plot_title);
      fprintf(report,"%.2f,",f1->GetParameter(0));
      if(ich==nDev-1)
	fprintf(report,"\n");

      if(iwien%2==0){
	fBeamAnull[ich]=0.5*f1->GetParameter(0);
	fBeamAnullError[ich]=f1->GetParError(0);
      }
      else{
	fBeamAnull[ich]+=0.5*f1->GetParameter(0);
	fBeamAnullError[ich] = sqrt(pow(fBeamAnullError[ich],2)+pow(f1->GetParError(0),2))/2;
      }
    }
    if(iwien%2==1){
      for(int ich=0;ich<nDev;ich++){
	fprintf(report,"%.2f,",fBeamAnull[ich]);
	if(ich==nDev-1)
	  fprintf(report,"\n");
      }
    }
  }
  fclose(report);
  tsw.Print();
}

