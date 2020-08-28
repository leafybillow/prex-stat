#ifndef __TARESULTS_HH__
#define __TARESULTS_HH__

#include "TString.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

class TaRow{
public:
  TaRow(){};
  virtual ~TaRow(){};
  void AddEntry(TString entry){
    fEntries.push_back(entry);
    if(entry.Length()>10)
      column_width.push_back(entry.Length());
    else
      column_width.push_back(10);
  }
  
  Ssiz_t GetColumnWidth(Int_t icol ){return column_width[icol];};
  
  void Clear(){
    fEntries.clear();
    column_width.clear();
  }

  Int_t size(){ return fEntries.size();};
  TString GetEntry(int ie) {
    if(ie>=size())
      return "(invalid)";
    else
      return fEntries[ie];};
private:
  vector<TString> fEntries;
  vector<Int_t> column_width;
  ClassDef(TaRow,0);
};

class TaResult{
public:
  TaResult(TString filename_in);
  virtual ~TaResult(){};
  void AddStringEntry(TString finput){ this_row.AddEntry(finput);};
  void AddFloatEntry(Double_t finput);
  void AddChi2NDF(Double_t chi2, Double_t ndf);
  void AddHeader(vector<TString> input);
  void InsertHorizontalLine();
  void AddLine();
  void Close();
  TString Center(TString,Int_t);
  void Print();
private:
  TString filename ;
  ofstream logfile;
  TaRow this_row;
  vector<TaRow> fRows;
  vector<Int_t> column_wid;
  Int_t nColumns;
  ClassDef(TaResult,0);
};



#endif
