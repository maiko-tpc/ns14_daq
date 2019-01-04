/* evt.c for new SUBARU experiment Dec. 2014 */

#include "madc32.h"
#include "babirldrvcaen.h"

#define _DEBUG_EVT 0
#define MAX_MULTI 32

void evt(void){
  int i,ip,iev,np;

  /* MADC parameters */
  int nev_madc;    // Number of MADC32 events in the buffer
  short wcnt_madc; // Word counts of the buffer
  int nw_madc[MAX_MULTI];   // Word counts of each event
  int time_madc[MAX_MULTI]; // Time stamp of each event
  unsigned int tmpdata_madc[12800]; // MADC32 Data
  int ip_madc;  // Read pointer for MADC32 Data

  /* V775 parameters */
  int evcnt_v775; // Event counter read from 0x1024 nad 0x1026
  short tmpevcnt_v775;
  int nev_v775; // Number of V775 events in the buffer
  short wcnt_v775; // Word counts of the buffer
  unsigned int tmpdata_v775[12800]; // V775 Data
  int nw_v775[MAX_MULTI]; // Number of hit channels in each event
  int ievt_v775[MAX_MULTI]; // Event number of each event
  int ip_v775;  // Read pointer for V775 Data


  /* Initialization *///////////////
  nev_madc=0;                     //
  ip_madc=0;                   //
  ip_v775=0;
  wcnt_v775=0;
  for(i=0; i<MAX_MULTI;i++){      //
    nw_madc[i]=0;                 //
    time_madc[i]=0;               //
    nw_v775[i]=0;                //
    ievt_v775[i]=0;             //
  }                               //
                                  //
////////////////////////////////////

#if _DEBUG_EVT > 0
  printk("\nEnter evt.c.\n");
#endif

  vme_read_intvector();
  rpv130_clear(RPV130ADR); // Disable interrupt
  
  ///////////////////////
  //// Analyze MADC32
  ///////////////////////

  /* Wait for the conversion of MADC32 */
  delay_us();
  delay_us();

  /* Read number of words in MADC (all events) */
  vread16(MADC32ADR + MADC32_BUFFER_DATA_LENGTH, (short *)&wcnt_madc);
  wcnt_madc = wcnt_madc & 0x3fff;

#if _DEBUG_EVT > 1
  printk("MADC32: wcnt=%d \n", wcnt_madc);
#endif
  
  iev=0;
  /* Read MADC data to tmp buffer */
  for(ip=0;ip<wcnt_madc;ip++){
    vread32(MADC32ADR + MADC32_DATA, &tmpdata_madc[ip]);
    
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
#if _DEBUG_EVT > 2
      printk("MADC32:%d:EOE:%08x TIMESTAMP=%d\n", iev, 
	     tmpdata_madc[ip],time_madc[iev]);
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
  //// Analyze V775
  ///////////////////////

  // V775 eventcounter
  vread16(V775ADR+V7XX_EVT_CNT_L,&tmpevcnt_v775);
  evcnt_v775=tmpevcnt_v775;
#if _DEBUG_EVT > 2
  printk("\nV775: EVTCounterL=%d\n",tmpevcnt_v775);
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
  do{
    vread32(V775ADR + V7XX_OUTBUFF, 
	    &tmpdata_v775[ip]);
    /* Search for evt header and read word count */
    if((tmpdata_v775[ip] & 0x07000000) == 0x02000000){
      nw_v775[iev] = ((tmpdata_v775[ip]>>8) & 0x3f)+2;
      // Ch count in the V775 header does not include the header itself.
#if _DEBUG_EVT > 2
      printk("V775:%d:EHD:%08x WCNT=%d\n",iev,tmpdata_v775[ip],nw_v775[iev]);
#endif
      wcnt_v775+=nw_v775[iev];
    }

    //#if _DEBUG_EVT
    //    if((tmpdata_v775[ip]&0xc07000000) == 0){
    //      printk("V775:%d:EVT:%08x \n", iev, tmpdata_v775[ip]);
    //    }
    //#endif


    /* Serch for evt footer and read the event counter */
    if((tmpdata_v775[ip] & 0x07000000) == 0x04000000){
      ievt_v775[iev]=tmpdata_v775[ip]&0xffffff;
#if _DEBUG_EVT > 2
      printk("V775:%d:EOE:%08x EVTCNT=%d\n",iev,tmpdata_v775[ip],ievt_v775[iev]);
#endif
#if _DEBUG_EVT > 1
      printk("V775:%d:NWORD=%d\n",iev, nw_v775[iev]);
#endif
      iev++;
    }
    ip++;
  }while(ievt_v775[iev-1]<evcnt_v775);
  nev_v775=iev;
  if(wcnt_v775!=ip){
    printk("Word count in the event header is incorrect. "
	   "wcnt_v775:%d  ip:%d\n",wcnt_v775,ip);
  }

#if _DEBUG_EVT > 0
  printk("V775:NEV=%d  WCNT=%d  EVCNT=%d\n", 
	 nev_v775,wcnt_v775,evcnt_v775);
#endif

  if(nev_madc!=nev_v775){
    printk("Numbers of events are different between "
	   "V775 (%d) and MADC32 (%d).\n",nev_v775,nev_madc);
  }
 
  /* Put data to RIDF */
  for(iev=0; iev<nev_madc; iev++){
   
    init_event();
    //    printk("Event initialized! \n");
   
    /* Set seg ID of MADC (device=6, focal=20, detector=6, module=32) */
    init_segment(MKSEGID(RCNPENN,LSC,SSDE,MADC32));
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


    /* Set seg ID of V775 (device=6, focal=20, detector=7, module=32) */
    init_segment(MKSEGID(RCNPENN,LSC,SSDT,V775));
    memcpy((unsigned int *)(data+mp),&tmpdata_v775[ip_v775],
	    4*nw_v775[iev]);
    np=2*nw_v775[iev];
    mp+=np;
    segmentsize+=np;
   
    /* Put MADC DATA */
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
   
    end_event();
  }

#if _DEBUG_EVT > 0
  printk("Exit from evt.c.\n");
#endif
}



