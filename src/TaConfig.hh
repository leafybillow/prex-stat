#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "Rtypes.h"

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;

#ifndef __TaConfig_hh__
#define __TaConfig_hh__

class TaConfig{
public:
  TaConfig();
  virtual ~TaConfig(){};

  Bool_t ParseFile(TString fileName);
  TString GetParameter(TString key);
  vector<TString> GetParameterList(TString key);
  
private:
  TString configName;
  map< TString, TString> fConfigParameters;

  vector<TString>  ParseLine(TString, TString);
  vector<TString> ConvertToArray(TString input);
  
  ClassDef(TaConfig,0);
};

#endif
