void clear(void){

  //  madc32_clear(MADC32ADR);
  //  madc32_readout_reset(MADC32ADR,1);
  madc32_readout_reset(MADC32ADR,0);
  //  madc32_start_acq(MADC32ADR);

  rpv130_enable(RPV130ADR);
  //  delay_us();
  //  delay_us();
  //  delay_us();
  //  delay_us();
  //  delay_us();
  //  delay_us();
  //  rpv130_output(RPV130ADR,OPIWKCLE);
  rpv130_output(RPV130ADR,OPBUSYCL);

  //  rpv130_output(RPV130ADR,OPBUSYCL2);


  // Muticlient case, you must send the end-of-busy to trigger circuits
  // this clearing function must be in here not in evt.c
  // rpv130_pulse(RPVADDR, 1);
}
