/* evt.c for GAGG test at ENN July. 02, 2014 by cotemba */

//#define _DEBUG_EVT 1


void evt(void){

#if _DEBUG_EVT
  printk("Enter evt.c.\n");
#endif
  
  vme_read_intvector();
  rpv130_clear(RPV130ADR); // Disable interrupt
  
  init_event();

  // wait the conversion of MADC32
  delay_us();
  delay_us();

  //Set segment ID for MADC32 (device=6, focal=20, detector=6, module=32)
  init_segment(MKSEGID(RCNPENN,LSC,SSDE,MADC32));
  // Read MADC32 DATA
  madc32_segdata(MADC32ADR);
  end_segment();  	
  
  //Set segment ID for V775 (device=6, focal=20, detector=7, module=32)
  //  init_segment(MKSEGID(RCNPENN,LSC,SSDT,V775));
  //Read V775 DATA
  //  v7XX_segdata(V775ADR);
  //  end_segment();  	

  end_event();

}
#if _DEBUG_EVT
//printk("Exit from evt.c.\n");
#endif


