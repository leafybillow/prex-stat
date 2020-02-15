typedef struct {
  TString ihwp,wien;
  Int_t sign} SlugInfo;

map< pair<Int_t,Int_t>, SlugInfo> LoadSlugList(){
  map< pair<Int_t,Int_t>,SlugInfo> fSlugListMap;
  
  return fSlugListMap;
}
