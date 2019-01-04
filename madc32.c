#include "madc32.h"

void madc32_readout_reset(unsigned int maddr,short val){
  vwrite16(maddr+MADC32_READOUT_RESET, &val);
}

void madc32_fifo_reset(unsigned int maddr,short val){
  vwrite16(maddr+MADC32_FIFO_RESET, &val);
}



void madc32_clear(unsigned int maddr){
  short val;
  val = 1;
  vwrite16(maddr+MADC32_READOUT_RESET, &val);
  vwrite16(maddr+MADC32_FIFO_RESET, &val);
}

void madc32_reset_ctr_ab(unsigned int maddr){
  short val;
  val = 0x1;
  vwrite16(maddr+MADC32_RESET_CTR_AB, &val);
}

void madc32_start_acq(unsigned int maddr){
  short val;
  val = 1;
  vwrite16(maddr+MADC32_START_ACQ, &val);
}

void madc32_stop_acq(unsigned int maddr){
  short val;

  val = 0;
  vwrite16(maddr+MADC32_START_ACQ, &val);
}

void madc32_irq_level(unsigned int maddr, short val){

  vwrite16(maddr+MADC32_IRQ_LEVEL, &val);
}

void madc32_resol_2k(unsigned int maddr){
  short val;
  val = 0;
  vwrite16(maddr+MADC32_ADC_RESOLUTION, &val);
}

void madc32_resol_4k(unsigned int maddr){
  short val;
  val = 1;
  vwrite16(maddr+MADC32_ADC_RESOLUTION, &val);
}

void madc32_resol_4khires(unsigned int maddr){
  short val;
  val = 2;
  vwrite16(maddr+MADC32_ADC_RESOLUTION, &val);
}

void madc32_resol_8k(unsigned int maddr){
  short val;
  val = 3;
  vwrite16(maddr+MADC32_ADC_RESOLUTION, &val);
}

void madc32_resol_8khires(unsigned int maddr){
  short val;
  val = 4;
  vwrite16(maddr+MADC32_ADC_RESOLUTION, &val);
}

// set threshold by cotemba
void madc32_threshold(unsigned int maddr,unsigned int ch, short val){

  vwrite16(maddr+MADC32_THRESHOLD+ch*2, &val);
}


void madc32_input_range(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_INPUT_RANGE, &val);
}

void madc32_multi_event(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_MULTIEVENT, &val);
}

void madc32_irq_threshold(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_IRQ_THRESHOLD, &val);
}


void madc32_max_transfer_data(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_MAX_TRANSFER_DATA, &val);
}

// Set marking type (event counter/time stamp)
void madc32_marking_type(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_MARKING_TYPE, &val);
}

// Set clock input of gate1
void madc32_NIM_gat1_osc(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_NIM_GAT1_OSC, &val);
}

// Set time stamp sources
void madc32_ts_sources(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_TS_SOURCES, &val);
}

// Switch off sliding scale
void madc32_slc_off(unsigned int maddr){
  short val;
  val = 1;
  vwrite16(maddr+MADC32_SLC_OFF, &val);
}

// Skip out of range values
void madc32_skip_oorange(unsigned int maddr){
  short val;
  val = 1;
  vwrite16(maddr+MADC32_SKIP_OORANGE, &val);
}

void madc32_module_id(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_MODULE_ID, &val);
}
 
void madc32_hold_delay(unsigned int maddr, unsigned int ch, short val){
  vwrite16(maddr+MADC32_HOLD_DELAY0+ch*2, &val);
}

void madc32_hold_width(unsigned int maddr, unsigned int ch, short val){
  vwrite16(maddr+MADC32_HOLD_WIDTH0+ch*2, &val);
}

void madc32_use_gg(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_USE_GG, &val);
}

void madc32_nim_busy(unsigned int maddr, short val){
  vwrite16(maddr+MADC32_NIM_BUSY, &val);
}

int madc32_segdata(unsigned int maddr){
  volatile short cnt, i;

  vread16(maddr + MADC32_BUFFER_DATA_LENGTH, (short *)&cnt);
  cnt = cnt & 0x3fff;
   //  printk("Data length:%d\n",cnt);
  for(i=0;i<cnt;i++){
    vread32(maddr + MADC32_DATA, (unsigned int *)(data+mp));
    //    printk("  Read Data: %08x\n",*(unsigned int *)(data+mp));
    mp += 2;
    segmentsize += 2;
  }

  return segmentsize;
}



#ifdef UNIV
void madc32_map_clear(int n){
  short val;

  val = 1;
  univ_map_write16(MADC32_READOUT_RESET, &val, n);

}

void madc32_map_start_acq(int n){
  short val;

  val = 1;
  univ_map_write16(MADC32_START_ACQ, &val, n);
}

void madc32_map_stop_acq(int n){
  short val;

  val = 0;
  univ_map_write16(MADC32_START_ACQ, &val, n);
}

void madc32_map_irq_level(short val, int n){

  univ_map_write16(MADC32_IRQ_LEVEL, &val, n);
}

void madc32_map_resol_8khires(int n){
  short val;

  val = 4;
  univ_map_write16(MADC32_ADC_RESOLUTION, &val, n);
}

void madc32_map_input_range(short val, int n){
  univ_map_write16(MADC32_INPUT_RANGE, &val, n);
}


void madc32_map_module_id(short val, int n){
  univ_map_write16(MADC32_MODULE_ID, &val, n);
}

int madc32_map_segdata(int n){
  volatile short cnt, i;

  univ_map_read16(MADC32_BUFFER_DATA_LENGTH, (short *)&cnt, n);
  cnt = cnt & 0x3fff;

  for(i=0;i<cnt;i++){
    univ_map_read32(MADC32_DATA, (long *)(data+mp), n);
    mp += 2;
    segmentsize += 2;
  }

  return segmentsize;
}
#endif

/////////////////////////////////
//  for new SUBARU experiment  //
/////////////////////////////////


#define DMA_WINDOWSIZE 0x800

int madc32_dmasegdata(unsigned long maddr,int depth,unsigned int *buff){
  int wordcnt, csize;
  int i,j;
  volatile int dmadelay, dmaflag;

  for(wordcnt=0;wordcnt<depth;){
    //    printk("MADC32:wordcnt:%d\n",wordcnt);
    dmaflag = 0;
    /* long word -> char size */
    if(depth<DMA_WINDOWSIZE) csize = depth * 4;
    else csize = DMA_WINDOWSIZE * 4;
    
    vme_dma_vread32_start(maddr, csize);
    delay_us();  /* delay about 1us */
    dmadelay = 0;
    for(dmadelay=0;dmadelay<1000000;dmadelay++){
      if(vme_dma_vread32_store((char *)(buff+wordcnt),csize)){
	//	printk("DMA Delay:%d\n",dmadelay);
	dmadelay = 2000000;
	dmaflag = 1;
	break;
      }else{
	delay_us();
      }
    }
    //    printk("DMA dealy:%d\n",dmadelay);
    wordcnt += (DMA_WINDOWSIZE);
  }

  //  /* Test output for kernel log. */
  //  if(maddr==TMB2ADR1 && icn==0){
  //    for(i=0;i<depth;){
  //      for(j=0;j<8;j++) printk("%08x ",buff[i++]);
  //      printk("\n");
  //    }
  //  }
	    
  //  wordcnt = depth;
  //  segmentsize += (depth*2);
  //  mp += (depth*2);

  return wordcnt;
}


