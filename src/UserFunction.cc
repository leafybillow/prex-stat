#include "TaRunInfo_v2.hh"
#include "TaStatBuilder.hh"
typedef pair<Int_t,Int_t> SLUG_ARM;
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


map<Int_t, TaRunInfo> LoadRunInfoMap(){
  map< Int_t, TaRunInfo > fRunInfoMap;  
  for(int islug=0;islug<=94;islug++){
    TString info_filename = Form("./runlist/slug%d_info.list",islug);
    ifstream slug_info;
    slug_info.open(info_filename.Data());

    if(!slug_info.is_open()){
      cout << " slug info " << info_filename << " does not exist." << endl;
      continue;
    }
    
    TString sline;
    while(sline.ReadLine(slug_info)){
      TaRunInfo fRunInfo;
      if(!fRunInfo.ParseLine(sline))
	continue;
      if(fRunInfo.GetRunFlag()=="Bad")
      	continue;
      
      Int_t run_number = fRunInfo.GetRunNumber();
      fRunInfoMap[run_number]=fRunInfo;
    }
    slug_info.close();
  }
  return fRunInfoMap;
}


void RegisterBranchesAddress(TTree *aTree,
			     vector<TString> namelist,
			     map<TString,StatData> &fMap){
  StatData fStatDataZero;
  auto iter = namelist.begin();
  while(iter!=namelist.end()){
    if(fMap.find(*iter)==fMap.end()){
      fMap.insert(make_pair(*iter,fStatDataZero));
    }
    TBranch *aBranch = aTree->GetBranch(*iter);
    if(aBranch!=NULL){
      fMap[*iter].RegisterAddressByName(aBranch);
    }else{
      fMap[*iter] = fStatDataZero;
    }
    iter++;
  }

}


vector<TString> ExtractBranchList(TTree* fTree){
  vector<TString> fRet;
  auto fBranchList = fTree->GetListOfBranches();
  Int_t nbr = fBranchList->GetEntries();
  for(int ibr =0; ibr<nbr;ibr++){
    TBranch* fbranch = dynamic_cast<TBranch*> (fBranchList->At(ibr));
    if(fbranch->GetLeaf("mean")==NULL)
      continue;
    if(fbranch->GetLeaf("rms")==NULL)
      continue;
    if(fbranch->GetLeaf("err")==NULL)
      continue;
    if( ( fbranch->GetName() ).Contains("block"))
      continue;
    fRet.push_back( fbranch->GetName() );
    
  }
  return fRet;
}

void UnionBranchList(vector<TString> &dest, vector<TString> input){
  auto iter = input.begin();
  while(iter!=input.end()){
    if(find(dest.begin(),dest.end(), *iter) == dest.end())
      dest.push_back( *iter);
    iter++;
  }
  
}
