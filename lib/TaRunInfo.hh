#ifndef __TaRunInfo_hh_
#define __TaRunInfo_hh_
class TaRunInfo: public TObject{
public:
  TaRunInfo();
  virtual ~TaRunInfo(){};
  Bool_t ParseLine(TString);
  inline TString GetRunConfig() const {return run_config;};
  inline TString GetRunFlag() const {return run_flag;};
  inline Int_t GetArmFlag() const {return arm_flag;};
  inline Int_t GetRunNumber() const {return run_number;};
  inline Int_t GetSlugNumber() const {return slug_number;};
  inline TString GetIHWPStatus() const {return ihwp;};
  inline TString GetWienMode() const {return wien;};
  inline Int_t GetSign() const {return sign;};
private:
  Int_t run_number;
  Int_t slug_number;
  TString run_config;
  Int_t arm_flag;
  TString run_flag;
  TString ihwp;
  TString wien;
  Int_t sign;
  ClassDef(TaRunInfo,0);
};

#endif
