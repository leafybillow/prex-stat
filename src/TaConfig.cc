#include "TaConfig.hh"

#include "TSystemDirectory.h"
ClassImp(TaConfig);
using namespace std;
TaConfig::TaConfig(){}

Bool_t TaConfig::ParseFile(TString fileName){
#ifdef NOISY
  cout << __PRETTY_FUNCTION__ << endl;
#endif
  ifstream configFile;
  configName = fileName;
  cout << " -- Opening " << configName << endl;
  configFile.open(configName.Data());
  if(!configFile.is_open()){
    cerr << __PRETTY_FUNCTION__ 
	 << " Error: failed to open config file "
	 << configName << endl;
    return kFALSE;
  }
  TString comment = "#";
  TString sline;
  
  vector<TString> vecStr;
  
  while(sline.ReadLine(configFile) ){
    if(sline.Contains(comment)) 
      continue;
    sline.ReplaceAll(" ","");
    if( sline.Contains("+=")){
      sline.ReplaceAll("+=","=");
      vecStr = ParseLine(sline, "=");
      fConfigParameters[vecStr[0]] += vecStr[1];
    }else{
      vecStr = ParseLine(sline, "=");
      fConfigParameters[vecStr[0]] = vecStr[1];
    }
      
  } // end of line loop
  configFile.close();
  return kTRUE;
}

vector<TString> TaConfig::ParseLine(TString sline, TString delim){
  vector<TString> ret;
  if(sline.Contains(delim)){
    Int_t index = sline.First(delim);
    TString buff = sline(0,index);
    ret.push_back(buff);
    sline.Remove(0,index+1);
    sline.ReplaceAll(" ","");
    ret.push_back(sline);
  }
  return ret;
}

TString TaConfig::GetParameter(TString key){
  return fConfigParameters[key];
}

vector<TString> TaConfig::GetParameterList(TString key){
  return ConvertToArray( fConfigParameters[key] );
}

vector<TString> TaConfig::ConvertToArray(TString sline){
  vector<TString> fRet;
  
  if(sline.Contains(',')){
    TObjArray *token = sline.Tokenize(',');
    Int_t nEntries = token->GetEntries();
    for(int ievt=0;ievt<nEntries;ievt++){
      TString fEntry = (((TObjString*)(token->At(ievt)))->GetString());
      fRet.push_back(fEntry);
    }
  } else if(sline!="")
    fRet.push_back(sline);
  
  return fRet;
}
