#include "lib/TaAccumulator.cc"
#include "lib/TaRunStats.cc"
#include "LoadNormalizationMap.C"
#include "lib/PlotPullFit.C"
#include "lib/TaResult.cc"
void GetBeamAsymmetries_postpan(Int_t arm_select);
void GetBeamAsymmetries_postpan(){
  GetBeamAsymmetries_postpan(-1);
  GetBeamAsymmetries_postpan(0);
  GetBeamAsymmetries_postpan(1);
  GetBeamAsymmetries_postpan(2);
}
void GetBeamAsymmetries_postpan(Int_t arm_select){
  TStopwatch tsw;
  
  TString beam_array[]={"Aq/ppb","cor_beam/ppb",
			"cor_bpm4aX/ppb","cor_bpm4eX/ppb",
			"cor_bpm4aY/ppb","cor_bpm4eY/ppb",
			"cor_bpmE/ppb",
			"diff_bpm4aX/nm","diff_bpm4eX/nm",
			"diff_bpm4aY/nm","diff_bpm4eY/nm",
			"diff_bpmE/nm"};

  
  Int_t nDev = sizeof(beam_array)/sizeof(*beam_array);
  
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

  TFile *inputRF = TFile::Open("rootfiles/prex_grand_average_postpan.root");
  TTree *grand_tree = (TTree*)inputRF->Get("grand");
  grand_tree->SetAlias("diff_bpmE","diff_bpm11X+diff_bpm12X");
  grand_tree->SetAlias("cor_bpmE","cor_bpm11X+cor_bpm12X");
  grand_tree->SetAlias("cor_beam","cor_bpm4aX+cor_bpm4eX+cor_bpm4aY+cor_bpm4eY+cor_bpm11X+cor_bpm12X");
  
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
  
  TString outlog_filename = Form("output/averages_by_wien_beamline%s.log",
				 output_label.Data());
  TaResult *fReport = new TaResult(outlog_filename);

  vector<TString> header{"IHWP,Wien",
			 "AQ(ppb)","Abeam","A4aX","A4eX",
			 "A4aY","A4eY","AE",
			 "D4aX(nm)","D4eX","D4aY","D4eY","DXE"};
  fReport->AddHeader(header);

  vector<Double_t> fBeamArray(nDev,0.0);
  Double_t total_weight=0.0;
  for(int iwien=0;iwien<8;iwien++){
    fReport->AddLine();
    fReport->AddStringEntry(Form("%s,%s",
				 ihwp_text[iwien].Data(),
				 wien_text[iwien].Data()));
    TString chname;
    for(int ich=0;ich<nDev;ich++){
      chname=beam_array[ich];
      Int_t npt = grand_tree->Draw(Form("sign*(%s):primary_error/ppb:slug",
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
      fReport->AddFloatEntry(f1->GetParameter(0));
      double this_weight;
      if(f1->GetParError(0)>0)
	this_weight =1.0/pow(f1->GetParError(0),2);
      else
	this_weight = 0.0;
      fBeamArray[ich] += this_weight*f1->GetParameter(0);
      if(ich==nDev-1){
	total_weight += this_weight;
	// fReport->AddFloatEntry(f1->GetParError(0));
      }

    }

  }
  fReport->InsertHorizontalLine();
  fReport->AddStringEntry("Average");
  for(int ich=0;ich<nDev;ich++)
    fReport->AddFloatEntry(fBeamArray[ich]/total_weight);
 
  fReport->InsertHorizontalLine();
  fReport->Print();
  fReport->Close();
  tsw.Print();
}

