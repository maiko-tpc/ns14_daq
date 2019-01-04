void startup(void){
  int ich;
  short sval;
  int imem,icn;

  /* Startup Function */

  /* Define the interrupt level for the controller */
  vme_define_intlevel(INTLEVEL);


  /////////////////////
  // Initialize RPV130
  /////////////////////
  rpv130_clear(RPV130ADR); // Disable interrupt

  /////////////////////
  // Initialize TMB2
  /////////////////////
  for(imem=0;imem<TMB2_NMEM;imem++){
    tmb2_reset(tmb2adr[imem]);
    for(icn=0;icn<TMB2_NCN;icn++){
      tmb2_setdepth(tmb2adr[imem],icn,TMB2DEPTH);
      //      printk("%x:%d:imem:%d  icn:%d  depth:%x\n",tmb2adr[imem],
      //             TMB2DEPTH,imem,icn,
      //             tmb2_readdepth(tmb2adr[imem],icn));
    }
    tmb2_switchbuf(tmb2adr[imem]);
    delay_us();
    tmb2_clear(tmb2adr[imem],TMB2_CONT_CLALL0);
  }


  /////////////////////
  // Initialize MADC32
  /////////////////////
  evcnt_madc=0;
  madc32_resol_4k(MADC32ADR); //set resolution
  madc32_input_range(MADC32ADR, 0x1); //input range (0:4V, 1:10V, 2:8V)
  //  madc32_slc_off(MADC32ADR);
  //  madc32_skip_oorange(MADC32ADR);  //skip out of range values
  madc32_use_gg(MADC32ADR, 0x1);  // use internal gate
  madc32_hold_delay(MADC32ADR, 0, 0);  // gate delay 
  madc32_hold_width(MADC32ADR, 0, 100);  // gate width (*50 ns) 
  madc32_nim_busy(MADC32ADR, 0x1); // NIM output conf (output internal gate)
  madc32_multi_event(MADC32ADR, 3); //multi event mode 3


  madc32_irq_threshold(MADC32ADR, 0x144);
  madc32_max_transfer_data(MADC32ADR, 0x144);
  madc32_marking_type(MADC32ADR, 0x1);  //00->event counter, 01->time stamp
  madc32_NIM_gat1_osc(MADC32ADR, 0x1);  //0->gate1 input, 1->clock input
  madc32_ts_sources(MADC32ADR, 0x1);

  /* Set thresholds of MADC32 */
  for(ich=16; ich<32; ich++){
  //  for(ich=1; ich<32; ich++){
    madc32_threshold(MADC32ADR, ich, 0x1FFF); // supress ch16--ch31
  }
  
  ///////////////////////
  //// Initialize V775
  ///////////////////////
  v775_crate_sel(V775ADR, 0x0000); // crate select (see sec. 4.31)

  v775_conf1(V775ADR, 0x5800);   // Bit Set 2
  v775_conf2(V775ADR, ~0x5800);   // Bit Set 2
  v775_full_scale_range(V775ADR, 0x1E);
  ///
  sval=0x20;
  vwrite16(V775ADR+0x1010,&sval); // Set Control Register 1
    
  /* Set thresholds of V775 */
  for(ich=0; ich<17; ich++){
    v775_threshold(V775ADR, ich, 0x0); // thresholds are ZERO
  }

  /* Kill ch16--ch31 */
  for(ich=17; ich<32; ich++){
  //  for(ich=1; ich<32; ich++){
    v775_threshold(V775ADR, ich, 0x100); // supress these channels
  }
  

  ///////////////////////
  //// Initialize V560
  ///////////////////////
  sval=1;
  vwrite16(V560ADR+0x50,&sval);  // Scaler Clear
  vwrite16(V560ADR+0x54,&sval);  // VME VETO reset


  /* Start V775 */
  v7XX_clear(V775ADR);  // clear data, pointers, event counter and peak section
  v775_evt_cnt_rst(V775ADR); //event counter reset
 
  /* Start MADC */
  madc32_stop_acq(MADC32ADR);
  //  madc32_fifo_reset(MADC32ADR,1); //reset FIFO
  madc32_fifo_reset(MADC32ADR,0); 
  //  madc32_readout_reset(MADC32ADR,1); //reset FIFO
  madc32_readout_reset(MADC32ADR,0); 

  madc32_start_acq(MADC32ADR); //start MADC32
  madc32_reset_ctr_ab(MADC32ADR); // reset event counter or timestamp

  vread16(V775ADR+0x1024,&sval);   // V775 eventcounter read
  printk("V775 event counter:%d\n",sval);

  /* Start TMB2 */
  for(imem=0;imem<TMB2_NMEM;imem++){
    int delayloop=0;
    tmb2_start(tmb2adr[imem]);
    for(delayloop=0;delayloop<10000000;delayloop++){
      if((tmb2_readstat(tmb2adr[imem],0)&TMB2_STAT_RUN)) break;
      delay_us();
      tmb2_start(tmb2adr[imem]);
    }
  }

  /* Start DAQ */
  rpv130_enable(RPV130ADR);
  rpv130_output(RPV130ADR,OPSCASTOP|OPSCACLER|OPIWKRST);
  rpv130_output(RPV130ADR,OPBUSYCL|OPSCASTART);
  rpv130_level(RPV130ADR,OPDAQON);
  printk("DAQ start.\n");


}
