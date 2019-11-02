#ifndef __TaRunStat_hh_
#define __TaRunStat_hh_
class TaRunStats: public TObject{
public:
  TaRunStats();
  virtual ~TaRunStats();
  void ParseLine(char* input);
  inline TString GetRunConfig() const {return run_config;};
  inline TString GetRunFlag() const {return run_flag;};
  inline Int_t GetArmFlag() const {return arm_flag;};
  inline Int_t GetRunNumber() const {return run_number;};
  inline void SetRunNumber(Int_t in){run_number=in;};
  inline TString GetIHWPStatus() const {return ihwp;};
  inline TString GetWienMode() const {return wienMode;};
private:
  Int_t run_number;
  TString run_config;
  Int_t arm_flag;
  TString run_flag;
  TString ihwp;
  TString wienMode;

  ClassDef(TaRunStats,0);
};

#endif
