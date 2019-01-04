/* evt.c for new data (matsuoka V4.3) format Nov. 19, 2013 */

//#define _DEBUG_EVT 1

#define FADC_DATA 0x2
#define TPC_DATA 0x3
#define FOOTER_DATA 0xffffffff



void evt(void){
  /* Event */
  int icn,imem;
  int ip; /* Read pointer */
  int iev; /* Event number */
  int nev; /* Maximum Event number among the four encoders. */
  int tmb2nev[TMB2_NMEM][TMB2_NCN]; /* Number of events for each module */
  int tmb2rp[TMB2_NMEM][TMB2_NCN][100]; /* Read pointer for each event */
  
#if _DEBUG_EVT
  printk("Enter evt.c.\n");
#endif

  vme_read_intvector();
  rpv130_clear(RPV130ADR); // Disable interrupt
  //rpv130_reset(RPV130ADR); // Disable interrupt

  rpv130_level(RPV130ADR,OPBUFSW); // Veto trigger during swithcing buffer
  //  printk("Interupt!!\n");

  
  for(imem=0;imem<TMB2_NMEM;imem++){
    tmb2_stop(tmb2adr[imem]); // stop memory
    for(icn=0;icn<TMB2_NCN;icn++) {// Read depth
      depth[imem][icn]=tmb2_readcnt(tmb2adr[imem],icn);
      //      printk("imem:%d  icn:%d  depth:%d\n",
      //      	     imem,icn,depth[imem][icn]);
    }
    tmb2_reset(tmb2adr[imem]); // Reset counter
    tmb2_switchbuf(tmb2adr[imem]); // Switch buffer
    delay_us();
    tmb2_start(tmb2adr[imem]); // Restart Memory

  }

#if _DEBUG_EVT
  printk("Buffer has been switched.\n");
#endif

  rpv130_level(RPV130ADR,0);

#if _DEBUG_EVT
  printk("Trigger veto was cleared.\n");
#endif

  for(imem=0;imem<TMB2_NMEM;imem++){
    for(icn=0;icn<TMB2_NCN;icn++){
      int tmpct;
      tmpct=tmb2_dmasegdata(tmb2adr[imem],icn,
			    depth[imem][icn],tmb2data[imem][icn]);
#if _DEBUG_EVT
      printk("Read %5x counts from mem:%d cn:%d depth:%5x.\n",
	     tmpct,imem,icn,depth[imem][icn]);
      // tmb2_segdata(tmb2adr[imem],icn,depth[imem][icn],tmb2data[imem][icn]);
#endif

    }
  }
  
  /* Data formatting */
#if _DEBUG_EVT
  printk("Data formatting ...\n");
#endif

  nev=0;
  for(imem=0;imem<TMB2_NMEM;imem++){
    for(icn=0;icn<TMB2_NCN;icn++) {
      iev=0;ip=0;
      tmb2nev[imem][icn]=0;
      while(ip<depth[imem][icn]){
	int itmphd,itmpevn,itmpclk;
	int itmpdat,itpcdep;
	int *itmbd;
	/**** Header Data ****/
	//printk("Event Header at %d.\n",ip);
	tmb2rp[imem][icn][iev]=ip;   /* Start point of the current event */
	itmbd=tmb2data[imem][icn];
	itmphd=itmbd[ip++]; /* Read header word */
	itmpevn=itmbd[ip++]; /* Event number */
	itmpclk=itmbd[ip++]; /* CLK number */
	
	if((itmphd!=1)||((itmpevn>>28)!=4)||((itmpclk>>28)!=05)) {
	  printk("Header (%08x %08x %08x) for imem=%d and icn=%d "
		 "in evt=%d is unreliable.\n",
		 itmphd,itmpevn,itmpclk,
		 imem,icn,
		 itmpevn&0x0fffffff);
	}


	/**** FADC Data ****/
	//printk("FADC Header (%08x) at %d.\n",itmbd[ip],ip);
	itmphd=itmbd[ip++];
	if((itmphd&0xfffff)==FADC_DATA) {
	  if(itmphd!=FADC_DATA){
	    printk("FADC Header  (%08x) for imem=%d and icn=%d "
		   "in evt=%d is unreliable, but accept ...\n",
		   itmphd,imem,icn,itmpevn&0x0fffffff);
	  }
	  ip+=FADCDEPTH;
	  itmphd=itmbd[ip++];
	}

	//printk("TPC Header (%08x) at %d.\n",itmbd[ip-1],ip-1);
	/**** TPC Data ****/
	if((itmphd&0xfffff)==TPC_DATA){
	  if(itmphd!=TPC_DATA){
	    printk("TPC Header   (%08x) for imem=%d and icn=%d "
		   "in evt=%d is unreliable, but accept ...\n",
		   itmphd,imem,icn,itmpevn&0x0fffffff);
	  }
	  for(itpcdep=0;itpcdep<TPCDEPTH;itpcdep++){
	    itmpdat=itmbd[ip++];
	    if((itmpdat&0xc0000000)==0xc0000000){
	      if((itmpdat&0x1fff)!=itpcdep){
		printk("Inconsistent TPC Depth "
		       "(Count:%4x Read:%8x) for imem=%d and icn=%d "
		       "in evt=%d is unreliable.\n",
		       itpcdep,itmpdat,
		       imem,icn,
		       itmpevn&0x0fffffff);
	      }
	      break;
	    }
	  }
	  itmphd=itmbd[ip++];
	}

	//	printk("Footer (%08x) at %d.\n",itmbd[ip-1],ip-1);
	if(itmphd!=FOOTER_DATA){
	  printk("Wrong footer "
		 "(%08x) for imem=%d and icn=%d "
		 "in evt=%d is unreliable.\n",
		 itmphd, imem,icn, itmpevn&0x0fffffff);
	  if((itmphd&0xfffff)!=0xfffff){
	    while(ip<depth[imem][icn]){
	      if((itmbd[ip++]&0xfffff)==0xfffff) break;
	    }
	  }
	}
	iev++;
	if(iev>nev) nev=iev;

      }
      tmb2nev[imem][icn]=iev;
      tmb2rp[imem][icn][iev]=depth[imem][icn];
    }
  }

#if _DEBUG_EVT
  for(iev=0;iev<nev;iev++){
    printk("Event number: %d   MP:%x\n",
    	   tmb2data[0][0][tmb2rp[0][0][iev]+1]&0x0fffffff,mp);
    for(imem=0;imem<TMB2_NMEM;imem++){
      for(icn=0;icn<TMB2_NCN;icn++) {
	printk("  %0d%0d nev:%2d iev:%2d rp:%6x  length:%6x  Depth:%6x\n",
	       imem,icn,tmb2nev[imem][icn],iev,tmb2rp[imem][icn][iev],
	       tmb2rp[imem][icn][iev+1]-tmb2rp[imem][icn][iev],
	       depth[imem][icn]);
      }
    }
  }
#endif

#if _DEBUG_EVT
  printk("Writing to buffer ...\n");
#endif

  //////
  //  if(nev>1) nev=1;
  for(iev=0;iev<nev;iev++){
    size_t csize,wordcnt;
    init_event();

    for(imem=0;imem<TMB2_NMEM;imem++){
      for(icn=0;icn<TMB2_NCN;icn++) {
	init_segment(MKSEGID(RCNPEN,F2,44+imem*2+icn,TMB2));
	if(tmb2nev[imem][icn]<=iev) {
	  printk("Event number for imem=%d and icn=%d in evt=%d "
		 "is smaller than the others.\n",
		 imem,icn,
		 tmb2rp[imem][icn][tmb2nev[imem][icn]-1]&0x0fffffff);
	  end_segment();  	
	  continue;
	}
	csize=(tmb2rp[imem][icn][iev+1]-tmb2rp[imem][icn][iev])*4;
	wordcnt=csize/2;

	memcpy((char *)(data+mp),
	       (char *)(tmb2data[imem][icn]+tmb2rp[imem][icn][iev]),
	       csize);
	mp +=wordcnt;
	segmentsize +=wordcnt;
#if _DEBUG_EVT
	printk("%d%d wc:%6d  mp:%6d  ",imem,icn,wordcnt,mp);
	printk("%08x  ",*(tmb2data[imem][icn]+tmb2rp[imem][icn][iev]));
	printk("%08x  ",*(tmb2data[imem][icn]+tmb2rp[imem][icn][iev]+1));
	printk("%08x  ",*(tmb2data[imem][icn]+tmb2rp[imem][icn][iev]+2));
	printk("%08x\n",*(tmb2data[imem][icn]+tmb2rp[imem][icn][iev]+3));
#endif
	end_segment();  	
      }
    }
    end_event();
  }
#if _DEBUG_EVT
  printk("Exit from evt.c.\n\n");
#endif
}

