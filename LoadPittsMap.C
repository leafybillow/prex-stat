typedef pair<Int_t,Int_t> SLUG_ARM;
pair< SLUG_ARM,Int_t> ParsePittMap(TString);

map< SLUG_ARM,Int_t> LoadPittsMap(){

  map< SLUG_ARM , Int_t > fPittMap;

  ifstream mapfile;
  mapfile.open("pitts.map");
  TString sline;
  while(sline.ReadLine(mapfile)){
    fPittMap.insert(ParsePittMap(sline));
  }
  mapfile.close();
  return fPittMap;
}

pair< SLUG_ARM, Int_t> ParsePittMap(TString input){

  Ssiz_t length = input.Length();
  Ssiz_t dot_pos  = input.First(',');
  Ssiz_t colon_pos  = input.First(':');
  TString slug_str = input(0,dot_pos);
  TString arm_str = input(dot_pos+1,colon_pos-dot_pos);
  TString pitt_str = input(colon_pos+1,length-colon_pos-1);
  SLUG_ARM fslug_arm = make_pair(slug_str.Atoi(),arm_str.Atoi());

  return make_pair(fslug_arm,pitt_str.Atoi());
}
