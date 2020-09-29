#include "TaRunStats.hh"
ClassImp(TaRunStats)
TaRunStats::TaRunStats(){
  run_number = 0;
  run_config = "Unknown";
  arm_flag = -1;
  run_flag = "Unknown";
  ihwp = "Unknown";
  wienMode = "Unknown";
}

TaRunStats::~TaRunStats(){}

void TaRunStats::ParseLine(char *input){

  TString input_string = TString((const char*)input);
  TString delim=",";
  Ssiz_t end_pt;
  TString this_entry;
  // Run Config
  end_pt = input_string.First(delim);
  this_entry = input_string(0,end_pt);
  run_config = this_entry;
  input_string.Remove(0,end_pt+1);
  // Run Flag
  end_pt = input_string.First(delim);
  this_entry = input_string(0,end_pt);
  run_flag = this_entry;
  input_string.Remove(0,end_pt+1);
  // IHWP
  end_pt = input_string.First(delim);
  this_entry = input_string(0,end_pt);
  ihwp = this_entry;
  input_string.Remove(0,end_pt+1);

  // Wien Mode
  end_pt = input_string.First(delim);
  this_entry = input_string(0,end_pt);
  wienMode = this_entry;
  input_string.Remove(0,end_pt+1);

  // Arm Flag
  arm_flag = input_string.Atoi();
  if(run_config=="ALLminusR" || run_config=="CH_LHRS")
    arm_flag=2;
}
