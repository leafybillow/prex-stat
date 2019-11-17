#include "lib/TaRunInfo.cc"

void MakeSlugList(){
  
  map< pair<Int_t,Int_t>, pair<TString,TString> > fSlugMap;
  
  for(int islug=1;islug<=94;islug++){
    TString info_filename = Form("./runlist/slug%d_info.list",islug);
    ifstream slug_info;
    slug_info.open(info_filename.Data());
    
    if(!slug_info.is_open()){
      cout << " Fail to open slug info " << info_filename<< endl;
      continue;
    }
    
    TString sline;
    Int_t cur_slug=-1;
    Int_t arm_flag=-1;
    TString ihwp_stat;
    TString wien_mode;

    while(sline.ReadLine(slug_info)){
      TaRunInfo fRunInfo;
      fRunInfo.ParseLine(sline);
      if(fRunInfo.GetRunFlag()!="Good")
	continue;
      cur_slug = fRunInfo.GetSlugNumber();
      arm_flag = fRunInfo.GetArmFlag();
      ihwp_stat = fRunInfo.GetIHWPStatus();
      wien_mode = fRunInfo.GetWienMode();

      if(fSlugMap.find(make_pair(cur_slug,arm_flag))==fSlugMap.end()){
	fSlugMap[make_pair(cur_slug,arm_flag)]=make_pair(ihwp_stat,wien_mode);
      }else{
	if(ihwp_stat != fSlugMap[make_pair(cur_slug,arm_flag)].first )
	  cout << "-- Error : Inconsistent IHWP state in " << cur_slug  << endl;
	if(wien_mode != fSlugMap[make_pair(cur_slug,arm_flag)].second )
	  cout << "-- Error : Inconsistent Wien Mode in " << cur_slug  << endl;
      }
      
    } // end of run loop
    slug_info.close();
  } // end of slug loop

  ofstream slug_list;
  slug_list.open("slug_list.txt");
  auto iter = fSlugMap.begin();
  while(iter!=fSlugMap.end()){

    slug_list << ((*iter).first).first << "\t" ;
    slug_list<<  ((*iter).first).second << "\t";
    if( ((*iter).first).second==0)
      slug_list << "Both-arm" << "\t";
    else if( ((*iter).first).second==1)
      slug_list << "Right-arm" << "\t";
    else if( ((*iter).first).second==2)
      slug_list << "Left-arm" << "\t";
    
    slug_list << ((*iter).second).first << "\t"
	      << ((*iter).second).second << "\t";
    Int_t sign;
    if( ((*iter).second).first == "IN" )
      sign = 1;
    else if ( ((*iter).second).first == "OUT" )
      sign =-1;

    if( ((*iter).second).second == "FLIP-RIGHT" )
      sign *= 1;
    else if ( ((*iter).second).second == "FLIP-LEFT" )
      sign *=-1;

    if(sign==1)
      slug_list << "+" << endl;
    else if(sign==-1)
      slug_list << "-" << endl;
    iter++;
  }
  slug_list.close();
}
