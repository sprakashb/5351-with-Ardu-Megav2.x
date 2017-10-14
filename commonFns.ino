// These functions carry out taks either by pressing physical buttons or touch buttons

void vfo_sel()   // select vfo A/B/M when VFO button pressed
{
  if (vfo_M_sel)
  {
    vfo_M = vfo;
    vfo_A_sel = true;
    vfo_B_sel = false;
    vfo_M_sel = false;
    vfo = vfo_A;
    bfo = bfo_A;
    sideband = sb_A;
    if (sideband == USB)
      bfo_USB = bfo_A;
    else
      bfo_LSB = bfo_A;
  }
  else if (vfo_B_sel)
  {
    vfo_B = vfo; // save current value of vfo for use later
    if (!xch_M)
      read_ch();   // get data from memory channel
    else
      xch_M = 0;

    vfo_A_sel = false;
    vfo_B_sel = false;
    vfo_M_sel = true;  // select
    vfo = vfo_M;    // restore values
    bfo = bfo_M;
    sideband = sb_M;
    if (sideband == USB)
      bfo_USB = bfo_M;
    else
      bfo_LSB = bfo_M;
  }
  else if (vfo_A_sel)
  {
    vfo_A = vfo;
    vfo_A_sel = false;
    vfo_B_sel = true;
    vfo_M_sel = false;
    vfo = vfo_B;
    bfo = bfo_B;
    sideband = sb_B;
    if (sideband == USB)
      bfo_USB = bfo_B;
    else
      bfo_LSB = bfo_B;
  }
  set_vfo();
  display_vfo();
  display_frequency();
  save_frequency();
  display_bfo();
  set_bfo();
  display_sideband();
  set_band();                        // 2 new lines 24/8/17 Joe
  display_band();

}

void mem_decr()     // decrement mem ch no
{  
  old_band = bnd_count;
  memCh = memCh - 1;
  if (memCh <= 0)
    memCh = max_memory_ch;
  display_mem();
  changed_f=1;
}

void mem_incr()     // increment mem ch no
{  
  old_band = bnd_count;
  memCh = memCh + 1;
  if (memCh > max_memory_ch)
    memCh = 1;
  display_mem();
    changed_f=1;
}

void rx_tx_ptt()      // toggle the PTT_output pin touch and PTT_Input pin activated
{
  txstatus = !txstatus;
  if(Tx_timeout_mode)
      Tx_start_time = millis();
  if (txstatus)   // Tx mode
  {
    digitalWrite(PTT_Output, HIGH);
    displ_tx();
  }
  else
  {
    digitalWrite(PTT_Output, LOW);
    displ_rx();
  }
}

void band_decr()      //decrement band count
{
  old_band = bnd_count;
  bnd_count = bnd_count - 1;
  if (bnd_count < 0)
    bnd_count = 8;
  change_band();
  set_band();
  save_frequency();
}

void band_incr()       // increment band count
{
  old_band = bnd_count;
  bnd_count = bnd_count + 1;
  if (bnd_count > 8)
    bnd_count = 0;
  change_band();
  set_band();
  save_frequency();
}

void step_decr()      // decrement step size 1M to 1Hz
{
  step_index = step_index - 1;
  if (step_index < 0)
    step_index = 6;
  radix = step_sz[step_index];
  display_step();
}

void step_incr()      // incremet step size 1Hz to 1M
{
  step_index = step_index + 1;
  if (step_index > 6)
    step_index = 0;
  display_step();
  radix = step_sz[step_index];
}

void sideband_chg()     // change sidebands between USB/LSB and may be more in future
{
  if (sideband == LSB)
  {
    sideband = USB;
    bfo = bfo_USB;
  }
  else
  {
    sideband = LSB;
    bfo = bfo_LSB;
  }
  display_sideband();
  display_bfo();
  save_frequency();
  set_vfo();
}

void vfo_to_mem()       // transfer current vfo to M
{
  if (vfo_A_sel)     // when vfo A is selected its content transferred to current memory
  {
    vfo_M = vfo_A;
    bfo_M = bfo_A;
  }
  else if (vfo_B_sel)   // B -> MemCh
  {
    vfo_M = vfo_B;
    bfo_M = bfo_B;
  }
  xch_M = 1;
  display_frequency2();
}

void mem_to_vfo()
{
  if ( vfo_A_sel)   // when vfo A is working Mem goes to A and changes vfo freq
  {
    vfo_A = vfo_M;
    vfo = vfo_A;
    bfo_A = bfo_M;
    bfo = bfo_A;
  }
  else if ( vfo_B_sel)  // when vfo B is working Mem goes to B and changes vfo freq
  {
    vfo_B = vfo_M;
    vfo = vfo_B;
    bfo_B = bfo_M;
    bfo = bfo_B;
  }
  changed_f = 1;
}

void bfo_decr()       // decrement bfo freq
{
  bfo = bfo - radix;
  set_bfo();
  display_bfo();
  save_frequency();
  set_vfo();
}

void bfo_incr()
{
  bfo = bfo + radix;
  set_bfo();
  display_bfo();
  save_frequency();
  set_vfo();
}

void save()       // save on EEPROM
{
  // Change message during save on Button
  tft.drawRoundRect(svx, svy, svwd, svht, roundness, WHITE); // Save button outline
  tft.fillRoundRect(svx + 2, svy + 2, svwd - 4, svht - 4, roundness - 4, GREEN); //Save
  tft.setTextColor(YELLOW);
  tft.setCursor(svx + 8, svy + 10);
  tft.print("SVNG");

  if (vfo_M_sel)
    write_ch();
  else if (vfo_A_sel)
    write_vfo_A();
  else
    write_vfo_B();

  delay(200);  // test
  // Reset Save button
  tft.drawRoundRect(svx, svy, svwd, svht, roundness, MAGENTA); // Save button outline
  tft.fillRoundRect(svx + 2, svy + 2, svwd - 4, svht - 4, roundness - 4, RED); //Save
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(svx + 10, svy + 10);
  tft.print("SAVE");

}

int get_button(int x)
{
  if (digitalRead(x) == LOW)      // sw pressed
    delay(100);      // debounce
  else
    return 0;

  if (digitalRead(x) == LOW)     // check again to confirm
    return 1;
  else
    return 0;
}