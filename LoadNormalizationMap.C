std::map<Int_t,Int_t> LoadNormalizationMap(){
  FILE* input = fopen("runlist/normalizing_bcm.txt","r");
  std::map<Int_t,Int_t> fBCMRunMap;
  char bcm_name[256];
  while(!feof(input)){
    Int_t run_number=0;
    Int_t bcm_index;
    fscanf(input,"%d,%d,%s\n",&run_number,&bcm_index,bcm_name);
    if(run_number!=0)
      fBCMRunMap[run_number]=bcm_index;
  }
  fclose(input);

  return fBCMRunMap;
}
