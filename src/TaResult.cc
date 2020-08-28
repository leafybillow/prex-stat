#include "TaResult.hh"

ClassImp(TaResult);
ClassImp(TaRow);

TaResult::TaResult(TString filename_in){

  filename =  filename_in;
  nColumns=0;
}

void TaResult::Close(){

  logfile.close();
}
void TaResult::AddHeader(vector<TString> input){

  Int_t ncol = input.size();
  AddLine();
  for(int icol=0;icol<ncol;icol++){
    AddStringEntry(input[icol]);
  }
  InsertHorizontalLine();
}

void TaResult::AddFloatEntry(Double_t finput){

  TString format_input = Form("%.2f",finput);
  this_row.AddEntry(format_input);
}
void TaResult::AddChi2NDF(Double_t chi2 , Double_t ndf){

  TString text = Form("%.1f/%.0f",chi2,ndf);
  this_row.AddEntry(text);
}

void TaResult::AddLine(){
  if(this_row.size()!=0){
    fRows.push_back(this_row);
    if(this_row.size()>nColumns)
      nColumns = this_row.size();
    
     for(int icol=0;icol<this_row.size();icol++){
       if( column_wid.begin() + icol  == column_wid.end())
	column_wid.push_back( this_row.GetColumnWidth(icol) );
       else if( this_row.GetColumnWidth(icol) > column_wid[icol] )
	column_wid[icol] = this_row.GetColumnWidth(icol);
    }
    this_row.Clear();
  }
}
void TaResult::InsertHorizontalLine(){
  AddLine();
  AddStringEntry("hline");
  AddLine();
}

void TaResult::Print(){
  cout << __PRETTY_FUNCTION__ <<  ": opening " <<  filename << endl;
  
  logfile.open(filename.Data());
  ostream& out = logfile;
  AddLine();
  Int_t nrow = fRows.size();
  TString colum_sep = "|";
  for(int irow=0;irow<nrow;irow++){
    if(fRows[irow].size()==1 && fRows[irow].GetEntry(0)=="hline"){
      for(int icol=0;icol<nColumns;icol++)
	for(int i=0;i<column_wid[icol]+1;i++)
	  out<< "-";
      out << endl;
    }
    else{
      for(int icol=0;icol<nColumns;icol++){
	out<< setw(column_wid[icol]) << setiosflags(ios::left)
	   << Center(fRows[irow].GetEntry(icol) , column_wid[icol]);
	out << colum_sep;
      }
      out << endl;
    }
  }
}


TString TaResult::Center(TString input, Int_t col_width){

  Int_t string_length = input.Length();
  Int_t npad = floor( (col_width+1 -string_length)*0.5);
  TString a_single_space =" ";
  TString output;
  if(input.Contains(".")){
    npad = col_width-string_length-2;
  }

  for(int i=0;i<npad;i++)
    output +=a_single_space;

  output+=input;

  return output;
}
