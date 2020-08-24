#ifndef __TARESULTS_HH__
#define __TARESULTS_HH__

#include "TString.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

class TaRow{
public:
  TaRow(){ max_width=10;};
  virtual ~TaRow(){};
  void AddEntry(TString entry){
    fEntries.push_back(entry);
    if(entry.Length()>max_width)
      max_width = entry.Length();
  }
  
  Ssiz_t GetMaxWidth(){return max_width;};
  
  void Clear(){
    fEntries.clear();
    max_width=0;
  }

  Int_t size(){ return fEntries.size();};
  TString GetEntry(int ie) {
    if(ie>=size())
      return "n/a";
    else
      return fEntries[ie];};
private:
  vector<TString> fEntries;
  Int_t max_width;
  ClassDef(TaRow,0);
};

class TaResult{
public:
  TaResult(TString filename);
  virtual ~TaResult(){};
  void AddStringEntry(TString finput){ this_row.AddEntry(finput);};
  void AddFloatEntry(Double_t finput);
  void AddChi2NDF(Double_t chi2, Double_t ndf);
  void AddHeader(vector<TString> input);
  void InsertHorizontalLine();
  void AddLine();
  void Close();
  TString Center(TString);
  void Print();
private:
  ofstream logfile;
  TaRow this_row;
  vector<TaRow> fRows;
  Int_t column_wid;
  Int_t nColumns;
  ClassDef(TaResult,0);
};



#endif
