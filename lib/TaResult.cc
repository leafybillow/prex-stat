#include "TaResult.hh"
#include <iomanip>
ClassImp(TaResult);
ClassImp(TaRow);

TaResult::TaResult(TString filename){
  logfile.open(filename.Data());
  column_wid=10;
  nColumns=0;
  cout << __PRETTY_FUNCTION__ <<  ": opening " <<  filename << endl;
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
void TaResult::AddLine(){
  if(this_row.size()!=0){
    fRows.push_back(this_row);
    if(this_row.size()>nColumns)
      nColumns = this_row.size();
    if(this_row.GetMaxWidth()>column_wid)
      column_wid = this_row.GetMaxWidth();
    this_row.Clear();
  }
}
void TaResult::InsertHorizontalLine(){
  AddLine();
  AddStringEntry("hline");
  AddLine();
}

void TaResult::Print(){
  ostream& out = logfile;
  AddLine();
  Int_t nrow = fRows.size();
  TString colum_sep = "|";
  for(int irow=0;irow<nrow;irow++){
    if(fRows[irow].size()==1 && fRows[irow].GetEntry(0)=="hline"){
      for(int i=0;i<nColumns*(column_wid+1);i++)
	out<< "-";
      out << endl;
    }
    else{
      for(int icol=0;icol<nColumns;icol++){
	out<< setw(column_wid) << setiosflags(ios::left)
	       << Center(fRows[irow].GetEntry(icol));
	out << colum_sep;
      }
      out << endl;
    }
  }
}


TString TaResult::Center(TString input){

  Int_t string_length = input.Length();
  Int_t npad = floor( (column_wid+1 -string_length)*0.5);
  TString a_single_space =" ";
  TString output;
  if(input.Contains(".")){
    npad = column_wid-string_length-2;
  }

  for(int i=0;i<npad;i++)
    output +=a_single_space;

  output+=input;

  return output;
}
