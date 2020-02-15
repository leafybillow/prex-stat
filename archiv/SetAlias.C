void SetAlias(){

  T->SetAlias("us_avg_4aX","asym_us_avg_diff_bpm4aX");
  T->SetAlias("us_avg_4eX","asym_us_avg_diff_bpm4eX");
  T->SetAlias("us_avg_11X","asym_us_avg_diff_bpm11X");
  T->SetAlias("us_avg_12X","asym_us_avg_diff_bpm12X");
  T->SetAlias("us_avg_4aY","asym_us_avg_diff_bpm4aY");
  T->SetAlias("us_avg_4eY","asym_us_avg_diff_bpm4eY");

  
  T->SetAlias("usl_4aX","asym_usl_diff_bpm4aX");
  T->SetAlias("usl_4eX","asym_usl_diff_bpm4eX");
  T->SetAlias("usl_11X","asym_usl_diff_bpm11X");
  T->SetAlias("usl_12X","asym_usl_diff_bpm12X");
  T->SetAlias("usl_4aY","asym_usl_diff_bpm4aY");
  T->SetAlias("usl_4eY","asym_usl_diff_bpm4eY");

  
  T->SetAlias("usr_4aX","asym_usr_diff_bpm4aX");
  T->SetAlias("usr_4eX","asym_usr_diff_bpm4eX");
  T->SetAlias("usr_11X","asym_usr_diff_bpm11X");
  T->SetAlias("usr_12X","asym_usr_diff_bpm12X");
  T->SetAlias("usr_4aY","asym_usr_diff_bpm4aY");
  T->SetAlias("usr_4eY","asym_usr_diff_bpm4eY");

  T->SetAlias("A_4aX","asym_us_avg_diff_bpm4aX*diff_bpm4aX");
  T->SetAlias("A_4eX","asym_us_avg_diff_bpm4eX*diff_bpm4eX");
  T->SetAlias("A_11X","asym_us_avg_diff_bpm11X*diff_bpm11X");
  T->SetAlias("A_12X","asym_us_avg_diff_bpm12X*diff_bpm12X");
  T->SetAlias("A_4aY","asym_us_avg_diff_bpm4aY*diff_bpm4aY");
  T->SetAlias("A_4eY","asym_us_avg_diff_bpm4eY*diff_bpm4eY");
  
  T->SetAlias("D4aX","diff_bpm4aX");
  T->SetAlias("D4eX","diff_bpm4eX");
  T->SetAlias("D4aY","diff_bpm4aY");
  T->SetAlias("D4eY","diff_bpm4eY");
  T->SetAlias("D11X","diff_bpm11X");
  T->SetAlias("D12X","diff_bpm12X");
}
