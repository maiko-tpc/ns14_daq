/* evt.c for new SUBARU experiment Dec. 2014 */

#include "madc32.h"
#include "babirldrvcaen.h"
#include "tmb2.h"

#define _DEBUG_EVT 2
#define _DMA_V775 0
#define _DMA_MADC32 0
#define _DMA_TMB2 0

#define MAX_MULTI 32
#define LBUF_V775 1088
#define LBUF_MADC32 1026

#define FADC_DATA 0x2
#define TPC_DATA 0x3
#define FOOTER_DATA 0xffffffff

void evt(void){
  int i,ip,iev,np,icnt;

  /* MADC parameters */
  int nev_madc;    // Number of MADC32 events in the buffer
  short wcnt_madc; // Word counts of the buffer
  unsigned int nw_madc[MAX_MULTI];   // Word counts of each event
  int ievt_madc[MAX_MULTI];  // Event number of each event
  unsigned int time_madc[MAX_MULTI]; // Time stamp of each event
  unsigned int tmpdata_madc[LBUF_MADC32]; // MADC32 Data
  int ip_madc;  // Read pointer for MADC32 Data


  /* V775 parameters */
  int evcnt_v775; // Event counter read from 0x1024 and 0x1026
  short tmpevcnt_v775;
  int nev_v775; // Number of V775 events in the buffer
  short wcnt_v775; // Word counts of the buffer
  int nw_v775[MAX_MULTI]; // Number of hit channels in each event
  int ievt_v775[MAX_MULTI]; // Event number of each event
  unsigned int tmpdata_v775[LBUF_V775]; // V775 Data
  int ip_v775;  // Read pointer for V775 Data

  /* TMB2 parameters */
  int icn,imem;
  int nevmax_tmb2; /* Maximum Event number among the four encoders. */
  int nev_tmb2[TMB2_NMEM][TMB2_NCN]; /* Number of events for each module */
  int ip_tmb2[TMB2_NMEM][TMB2_NCN][MAX_MULTI]; /* Read pointer for each event */
  int ievt_tmb2[TMB2_NMEM][TMB2_NCN][MAX_MULTI]; /* Event number of each event */
  int depth[TMB2_NMEM][TMB2_NCN];
  size_t ccnt_tmb2,wcnt_tmb2;

#if _DEBUG_EVT > 0
  printk("\nEnter evt.c.\n");
#endif

  vme_read_intvector();
  rpv130_clear(RPV130ADR); // Disable interrupt


  /* Initialization *///////////////
  nev_madc=0;                     //
  ip_madc=0;                      //
  ip_v775=0;                      //
  wcnt_v775=0;                    //
  for(i=0; i<MAX_MULTI;i++){      //
    nw_madc[i]=0;                 //
    time_madc[i]=0;               //
    nw_v775[i]=0;                 //
    ievt_v775[i]=0;               //
    ievt_madc[i]=0;               //
    for(imem=0;imem<TMB2_NMEM;imem++){
      for(icn=0;icn<TMB2_NCN;icn++)
	ievt_tmb2[imem][icn][i]=0;
    }
  }                               //
                                  //
////////////////////////////////////

  
  ///////////////////////
  /// TMB2 Buffer change
  //////////////////////
  rpv130_level(RPV130ADR,(OPDAQON|OPTMB2BFCH)); 
  // Veto trigger during swithcing buffer
  for(imem=0;imem<TMB2_NMEM;imem++){
    tmb2_stop(tmb2adr[imem]); // stop memory
    while(1){
      if((tmb2_readstat(tmb2adr[imem],0)&TMB2_CONT_STOP)) break;
    }

    for(icn=0;icn<TMB2_NCN;icn++) {// Read depth
      depth[imem][icn]=tmb2_readcnt(tmb2adr[imem],icn);
#if _DEBUG_EVT > 0
      printk("TMB2:imem:%d  icn:%d  depth:%d\n",
	     imem,icn,depth[imem][icn]);
#endif
    }
    tmb2_reset(tmb2adr[imem]); // Reset counter
    tmb2_switchbuf(tmb2adr[imem]); // Switch buffer
    //    delay_us();
    tmb2_start(tmb2adr[imem]); // Restart Memory
  }


#if _DEBUG_EVT > 1
  printk("TMB2:Buffer has been switched.\n");
#endif
  rpv130_level(RPV130ADR,OPDAQON); 


  ///////////////////////
  //// Read MADC32
  ///////////////////////

  //  /* Wait for the conversion of MADC32 */
  //  delay_us();
  //  delay_us();

  /* Read number of words in MADC (all events) */
  vread16(MADC32ADR + MADC32_BUFFER_DATA_LENGTH, (short *)&wcnt_madc);
  wcnt_madc = wcnt_madc & 0x3fff;
 

#if _DEBUG_EVT > 1
  printk("MADC32: wcnt=%d \n", wcnt_madc);
#endif
  
  iev=0;
  /* Read MADC data to tmp buffer */
  tmpdata_madc[0]=1;
#if _DMA_MADC32 
  madc32_dmasegdata(MADC32ADR+MADC32_DATA,wcnt_madc,
		    &tmpdata_madc[0]);
#endif

  for(ip=0;ip<wcnt_madc;ip++){
#if _DMA_MADC32==0
    vread32(MADC32ADR + MADC32_DATA, (unsigned int *)&tmpdata_madc[ip]);
#endif
    /* Search for the event header and read number of words in the event */
    if((tmpdata_madc[ip]&0xc0000000) == 0x40000000){
      nw_madc[iev] = (tmpdata_madc[ip] & 0xfff)+1;

      // Word count in the MADC32 header does not include the header itself.
#if _DEBUG_EVT > 2
      printk("MADC32:%d:EHD:%08x NWORD=%d \n", 
	     iev, tmpdata_madc[ip],nw_madc[iev]);
#endif
    }
    
    //#if _DEBUG_EVT
    //    if((tmpdata_madc[ip]&0xc0000000) == 0){
    //      printk("MADC32:%d:EVT:%08x \n", iev, tmpdata_madc[ip]);
    //    }
    //#endif
    
    
    /* Search for end of event mark and read time stamp */
    if((tmpdata_madc[ip]&0xc0000000) == 0xc0000000){
      time_madc[iev] = (tmpdata_madc[ip] & 0x3fffffff);
      ievt_madc[iev]=evcnt_madc+iev;
#if _DEBUG_EVT > 2
      printk("MADC32:%d:EOE:%08x TIMESTAMP=%d  EVTCNT=%d\n", iev, 
	     tmpdata_madc[ip],time_madc[iev],ievt_madc[iev]);
#endif

#if _DEBUG_EVT > 1
      printk("MADC32:%d:NWORD=%d  TIMESTAMP=%d\n", 
	     iev, nw_madc[iev],time_madc[iev]);
#endif
      iev++;
    }
  }
  nev_madc=iev;
#if _DEBUG_EVT > 0
      printk("MADC32:NEV=%d  WCNT=%d  TIMESTAMP=%d\n", 
	     nev_madc,wcnt_madc,time_madc[nev_madc-1]);
#endif

  ///////////////////////
  //// Read V775
  ///////////////////////

  // V775 eventcounter
  vread16(V775ADR+V7XX_EVT_CNT_L,&tmpevcnt_v775);
  evcnt_v775=tmpevcnt_v775;
#if _DEBUG_EVT > 2
  printk("V775: EVTCounterL=%d\n",tmpevcnt_v775);
#endif
  vread16(V775ADR+V7XX_EVT_CNT_H,&tmpevcnt_v775);
  evcnt_v775+=tmpevcnt_v775*65536;;
#if _DEBUG_EVT > 2
  printk("V775: EVTCounterH=%d\n",tmpevcnt_v775);
#endif
#if _DEBUG_EVT > 1
  printk("V775: EVTCounter =%d\n",evcnt_v775);
#endif

  iev=0;ip=0;

#if _DMA_V775
  v7xx_dmaread(V775ADR,tmpdata_v775);
#endif

  do{
#if _DMA_V775 == 0
    vread32(V775ADR + V7XX_OUTBUFF, &tmpdata_v775[ip]);
#endif
    /* Search for evt header and read word count */
    if((tmpdata_v775[ip] & 0x07000000) == 0x02000000){
      nw_v775[iev] = ((tmpdata_v775[ip]>>8) & 0x3f)+2;
      // Ch count in the V775 header does not include the header itself.
#if _DEBUG_EVT > 2
      printk("V775:%d:EHD:%08x WCNT=%d\n",iev,tmpdata_v775[ip],nw_v775[iev]);
#endif
      wcnt_v775+=nw_v775[iev];
      ip++;
    }
    else{
      printk("V775:No header at the begining of the event.\n");
    }

    for(icnt=1;icnt<nw_v775[iev];icnt++){
#if _DMA_V775 == 0
      vread32(V775ADR + V7XX_OUTBUFF, &tmpdata_v775[ip]);
#endif
      if((tmpdata_v775[ip]&0x07000000) == 0){
#if _DEBUG_EVT > 2
	printk("V775:%d:EVT:%08x \n", iev, tmpdata_v775[ip]);
	
#endif
      }    
      /* Serch for evt footer and read the event counter */
      if((tmpdata_v775[ip] & 0x07000000) == 0x04000000){
	ievt_v775[iev]=tmpdata_v775[ip]&0xffffff;
#if _DEBUG_EVT > 2
	printk("V775:%d:EOE:%08x EVTCNT=%d\n",
	       iev,tmpdata_v775[ip],ievt_v775[iev]);
#endif
#if _DEBUG_EVT > 1
	printk("V775:%d:NWORD=%d\n",iev, nw_v775[iev]);
#endif
      }
      ip++;
    }
    iev++;
    //    printk("V775:%d:NWORD=%d\n",iev, nw_v775[iev]);
    //    printk("V775:ievt_v775[iev-1]:%d evcnt_v775:%d\n",
    //	   ievt_v775[iev-1],evcnt_v775);
    if(ip>=LBUF_V775) break;
  }while(ievt_v775[iev-1]<evcnt_v775);
  
  nev_v775=iev;
  if(wcnt_v775!=ip){
    printk("V775:Word count in the event header is incorrect. "
	   "wcnt_v775:%d  ip:%d\n",wcnt_v775,ip);
  }
  
#if _DEBUG_EVT > 0
  printk("V775:NEV=%d  WCNT=%d  EVCNT=%d\n", 
	 nev_v775,wcnt_v775,evcnt_v775);
#endif
  

  ///////////////////////
  //// Read TMB2
  ///////////////////////
  nevmax_tmb2=0;
  for(imem=0;imem<TMB2_NMEM;imem++){
    for(icn=0;icn<TMB2_NCN;icn++){
      int tmpct=0;
#if _DMA_TMB2
      tmpct=tmb2_dmasegdata(tmb2adr[imem],icn,
			    depth[imem][icn],tmb2data[imem][icn]);
#else
      tmpct=tmb2_segdata(tmb2adr[imem],icn,depth[imem][icn],
      			 tmb2data[imem][icn]);
#endif
#if _DEBUG_EVT > 2
      printk("TMB2: Read %x counts from mem:%d cn:%d depth:%x.\n",
             tmpct,imem,icn,depth[imem][icn]);
#if _DEBUG_EVT > 3
      if(1){
	int itmp;
	for(itmp=0;itmp<depth[imem][icn];itmp++)
	  printk("TMB2: imem:%d:icn%d:%d %08x\n",
		 imem,icn,itmp,tmb2data[imem][icn][itmp]);
      }
#endif
#endif
    }
  }

#if _DEBUG_EVT > 1
  printk("TMB2:Data formatting ...\n");
#endif

  for(imem=0;imem<TMB2_NMEM;imem++){
    for(icn=0;icn<TMB2_NCN;icn++) {
      iev=0;ip=0;
      nev_tmb2[imem][icn]=0;
      while(ip<depth[imem][icn]){
	int itmphd,itmpevn,itmpclk;
	int itmpdat,itpcdep;
	int *itmbd;
	/**** Header Data ****/
#if _DEBUG_EVT > 1
	printk("TMB2: %0d%0d Event Header (%08x) at %d.\n",
	       imem,icn,tmb2data[imem][icn][ip],ip);
#endif
	ip_tmb2[imem][icn][iev]=ip;   /* Start point of the current event */
	itmbd=tmb2data[imem][icn];
	itmphd=itmbd[ip++]; /* Read header word */
	itmpevn=itmbd[ip++]; /* Event number */
	itmpclk=itmbd[ip++]; /* CLK number */
	ievt_tmb2[imem][icn][iev]=(itmpevn&0xfffffff);
	if((itmphd!=1)||((itmpevn>>28)!=4)||((itmpclk>>28)!=05)) {
	  printk("TMB2:Header (%08x %08x %08x) for imem=%d and icn=%d "
		 "in evt=%d is unreliable.\n",
		 itmphd,itmpevn,itmpclk,
		 imem,icn,
		 itmpevn&0x0fffffff);
	}


	/**** FADC Data ****/
	itmphd=itmbd[ip++];
	if((itmphd&0xfffff)==FADC_DATA) {
#if _DEBUG_EVT > 2
	  printk("TMB2: %0d%0d FADC Header (%08x) at %d.\n",
		 imem,icn,itmbd[ip-1],ip-1);
#endif
	  if(itmphd!=FADC_DATA){
	    printk("TMB2:FADC Header  (%08x) for imem=%d and icn=%d "
		   "in evt=%d is unreliable, but accept ...\n",
		   itmphd,imem,icn,itmpevn&0x0fffffff);
	  }
	  ip+=FADCDEPTH;
	  itmphd=itmbd[ip++];
	}

	/**** TPC Data ****/
	if((itmphd&0xfffff)==TPC_DATA){
#if _DEBUG_EVT > 2
	printk("TMB2: %0d%0d TPC Header (%08x) at %d.\n",
	       imem,icn,itmbd[ip-1],ip-1);
#endif
	  if(itmphd!=TPC_DATA){
	    printk("TMB2:TPC Header   (%08x) for imem=%d and icn=%d "
		   "in evt=%d is unreliable, but accept ...\n",
		   itmphd,imem,icn,itmpevn&0x0fffffff);
	  }
	  for(itpcdep=0;itpcdep<TPCDEPTH;itpcdep++){
	    itmpdat=itmbd[ip++];
	    if((itmpdat&0xc0000000)==0xc0000000){
	      if((itmpdat&0x1fff)!=itpcdep){
		printk("TMB2:Inconsistent TPC Depth "
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

#if _DEBUG_EVT > 2
	printk("TMB2: %0d%0d Footer (%08x) at %d.\n",
	       imem,icn,itmbd[ip-1],ip-1);
#endif
	if(itmphd!=FOOTER_DATA){
	  printk("TMB2:Wrong footer "
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
	if(iev>nevmax_tmb2) nevmax_tmb2=iev;

      }
      nev_tmb2[imem][icn]=iev;
      ip_tmb2[imem][icn][iev]=depth[imem][icn];
    }
  }

#if _DEBUG_EVT > 0
  for(iev=0;iev<nevmax_tmb2;iev++){
    printk("TMB2:Event number: %d   MP:%x\n",
	   tmb2data[0][0][ip_tmb2[0][0][iev]+1]&0x0fffffff,mp);
#if _DEBUG_EVT > 1
    for(imem=0;imem<TMB2_NMEM;imem++){
      for(icn=0;icn<TMB2_NCN;icn++) {
	printk("TMB2: %0d%0d nev:%2d iev:%2d ip:%6x  length:%6x  Depth:%6x\n",
	       imem,icn,nev_tmb2[imem][icn],iev,ip_tmb2[imem][icn][iev],
	       ip_tmb2[imem][icn][iev+1]-ip_tmb2[imem][icn][iev],
	       depth[imem][icn]);
      }
    }
#endif
  }
#endif

  /////////////////////////////////////////////
  // Event consistency check
  
  if(nev_madc!=nev_v775){
    printk("V775:Numbers of events are different between "
	   "V775 (%d) and MADC32 (%d).\n",nev_v775,nev_madc);
  }

  for(imem=0;imem<TMB2_NMEM;imem++){
    for(icn=0;icn<TMB2_NCN;icn++){
      if(nev_tmb2[imem][icn]!=nev_madc){
	printk("TMB2:Number of events are different between "
	       "TMB2 (%d for MEM:%d CN:%d) and MADC32 (%d).\n",
	       nev_tmb2[imem][icn],imem,icn,nev_madc);
      }
    }
  }

#if _DEBUG_EVT > 0
  for(iev=0;iev<nev_madc;iev++){
    printk("EV%d: MADC32:%d  V775:%d  TMB2 ",
	   iev,ievt_madc[iev],ievt_v775[iev]);
      for(imem=0;imem<TMB2_NMEM;imem++){
	for(icn=0;icn<TMB2_NCN;icn++)
	  printk("%d%d:%d  ",imem,icn,ievt_tmb2[imem][icn][iev]);
      }
    printk("\n");
  }
#endif

  /* Put data to RIDF ********************************/

  for(iev=0; iev<nev_madc; iev++){
    evcnt_madc++;   
    init_event();
    //    printk("Event initialized! \n");
   
    /* Set seg ID of MADC (device=7, focal=19, detector=6, module=32) */
    init_segment(MKSEGID(NSBL01,GACKO,SSDE,MADC32));
    //    printk("segment initialized! \n");
   
   /* Put MADC DATA */
   //madc32_segdata(MADC32ADR);
    memcpy((unsigned int *)(data+mp),&tmpdata_madc[ip_madc],
	    4*nw_madc[iev]);
    np=2*nw_madc[iev];
    mp+=np;
    segmentsize+=np;

    //   printk("EVT:%d, TIME=%d \n", iev, time_madc[iev]);
    //   for(ip=0; ip<nw_madc[iev]; ip++){
    //      printk("Dumping data %d... \n", i);
    //     memcpy((unsigned int *)(data+mp), 
    //	    (unsigned int *)&tmpdata_madc[ip+count_madc], 4);
    //     mp += 2;
    //     segmentsize +=2;
    //   }    

    ip_madc+=nw_madc[iev];
    end_segment();  	


    /* Set seg ID of V775 (device=7, focal=19, detector=7, module=32) */
    init_segment(MKSEGID(NSBL01,GACKO,SSDT,V775));
    memcpy((unsigned int *)(data+mp),&tmpdata_v775[ip_v775],
	    4*nw_v775[iev]);
    np=2*nw_v775[iev];
    mp+=np;
    segmentsize+=np;
   
    /* Put V775 DATA */
    //  v7XX_segdata(V775ADR);
    //   while(1){
    //     memcpy((unsigned int *)(data+mp), 
    //	    (unsigned int *)&tmpdata_v775[count_v775], 4);
    //     mp += 2;
    //     segmentsize +=2;
    //     /* Serch for evt footer and read the event counter */
    //     if((tmpdata_v775[count_v775] & 0x04000000) == 0x04000000){
    //       count_v775++;
    //      break;
    //    }
    ip_v775+=nw_v775[iev];
    end_segment();  	

   
   /* Put TMB2 DATA */
    for(imem=0;imem<TMB2_NMEM;imem++){
      for(icn=0;icn<TMB2_NCN;icn++) {
	/* Set seg ID of MADC (device=7, focal=19, detector=44--47, module=50) */
	init_segment(MKSEGID(NSBL01,GACKO,(44+imem*2+icn),TMB2));
	if(nev_tmb2[imem][icn]<=iev) {
	  printk("TMB2:Event number for imem=%d and icn=%d in evt=%d "
		 "is smaller than the others.\n",
		 imem,icn,
		 ip_tmb2[imem][icn][nev_tmb2[imem][icn]-1]&0x0fffffff);
	  end_segment();  	
	  continue;
	}
	ccnt_tmb2=(ip_tmb2[imem][icn][iev+1]-ip_tmb2[imem][icn][iev])*4;
	wcnt_tmb2=ccnt_tmb2/2;

	memcpy((char *)(data+mp),
	       (char *)(tmb2data[imem][icn]+ip_tmb2[imem][icn][iev]),
	       ccnt_tmb2);
	mp +=wcnt_tmb2;
	segmentsize +=wcnt_tmb2;
#if _DEBUG_EVT > 1
	printk("TMB2:%d%d wc:%6d  mp:%6d  ",imem,icn,wcnt_tmb2,mp);
	printk("%08x  ",*(tmb2data[imem][icn]+ip_tmb2[imem][icn][iev]));
	printk("%08x  ",*(tmb2data[imem][icn]+ip_tmb2[imem][icn][iev]+1));
	printk("%08x  ",*(tmb2data[imem][icn]+ip_tmb2[imem][icn][iev]+2));
	printk("%08x\n",*(tmb2data[imem][icn]+ip_tmb2[imem][icn][iev]+3));
#endif
	end_segment();  	
      }
    }
    end_event();
  }

  ///////////////////
#if _DEBUG_EVT > 0
  printk("Exit from evt.c.\n");
#endif
}



