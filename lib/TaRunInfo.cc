#include "TaRunInfo.hh"
ClassImp(TaRunInfo)
TaRunInfo::TaRunInfo(){
  run_number = -1;
  slug_number = -1;
  run_config = "Unknown";
  arm_flag = -1;
  run_flag = "Unknown";
  ihwp = "Unknown";
  wien = "Unknown";
  sign=0.0;
}

Bool_t TaRunInfo::ParseLine(TString sline){

  TObjArray *token = sline.Tokenize(',');

  if(token->GetEntries()!=7){
    cout << " -- Error: Not able to identify run info: " << endl;
    cout << sline << endl;
    return kFALSE;
  }

  run_number = (((TObjString*)(token->At(0)))->GetString()).Atoi();
  slug_number = (((TObjString*)(token->At(1)))->GetString()).Atoi();
  run_config = ((TObjString*)(token->At(2)))->GetString();
  run_flag = ((TObjString*)(token->At(3)))->GetString();
  ihwp = ((TObjString*)(token->At(4)))->GetString();
  wien = ((TObjString*)(token->At(5)))->GetString();
  arm_flag = (((TObjString*)(token->At(6)))->GetString()).Atoi();
  
  if(run_config=="ALLminusR" || run_config=="CH_LHRS")
    arm_flag=2;
  
  if( ihwp == "IN" )
    sign = 1;
  else if (ihwp == "OUT" )
    sign =-1;

  if( wien == "FLIP-RIGHT" )
    sign *= 1;
  else if ( wien == "FLIP-LEFT" )
    sign *=-1;

  return kTRUE;
}
