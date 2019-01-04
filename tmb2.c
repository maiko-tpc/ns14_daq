#include "tmb2.h"

void tmb2_reset(unsigned int maddr){
  int sval;
  sval=TMB2_CONT_CRESET;
  vwrite32(maddr + TMB2_CONTREG, &sval);  
}


void tmb2_switchbuf(unsigned int maddr){
  int sval;
  sval=TMB2_CONT_BUFCHANGE;
  vwrite32(maddr + TMB2_CONTREG, &sval);  
}

void tmb2_start(unsigned int maddr){
  int sval;
  sval=TMB2_CONT_START;
  vwrite32(maddr + TMB2_CONTREG, &sval);  
}

void tmb2_stop(unsigned int maddr){
  int sval;
  sval=TMB2_CONT_STOP;
  vwrite32(maddr + TMB2_CONTREG, &sval);  
}

void tmb2_clear(unsigned int maddr,unsigned int iflag){
  int sval;
  if(iflag==TMB2_CONT_CLALL1 ||
     iflag==TMB2_CONT_CLALL0 ||
     iflag==TMB2_CONT_CLINC)
    sval=iflag+TMB2_CONT_CLEAR;
    vwrite32(maddr + TMB2_CONTREG, &sval);  
}

void tmb2_setdepth(unsigned int maddr,int ibuf, int depth){
  vwrite32(maddr+TMB2_DEPREG1+ibuf*8,&depth);
}

#if 0
int tmb2_segdata(unsigned int maddr,int ibuf){
  int wordcnt;
  int depth;
  wordcnt = 0;

  vread32(maddr+TMB2_CONTREG1+ibuf*8,&depth);
  tmb2_reset(maddr);
  for(wordcnt=0;wordcnt<depth;wordcnt++){
    vread32(maddr+TMB2_OUTBUFF1+ibuf*0x800,(int *)(data+mp));
    mp += 2;
    segmentsize += 2;
  }
  return segmentsize;
}
#endif

int tmb2_segdata(unsigned int maddr,int icn,int depth,int *buff){
  int ilp = 0;
  //  printk("%d addr:%08x  icn:%d  depth:%d\n",sizeof(long),maddr,icn,depth);
  for(ilp=0;ilp<depth;ilp++){
    vread32(maddr+TMB2_OUTBUFF1+icn*0x800,buff+ilp);
    //    mp += 2;
    //    segmentsize += 2;
  }
  return ilp;
}


int tmb2_readstat(unsigned int maddr,int icn){
  int istat;
  vread32(maddr+TMB2_STAREG1+icn*8,&istat);
  return istat;
}


int tmb2_readdepth(unsigned int maddr,int icn){
  int depth;
  vread32(maddr+TMB2_DEPREG1+icn*8,&depth);
  return depth;
}

int tmb2_readcnt(unsigned int maddr,int icn){
  int cnt;
  vread32(maddr+TMB2_CONTREG1+icn*8,&cnt);
  return cnt;
}

int tmb2_readverreg(unsigned int maddr){
  int istat;
  vread32(maddr+TMB2_VERREG,&istat);
  return istat;
}

int tmb2_dmasegdata(unsigned int maddr,int icn,int depth,int *buff){
  int wordcnt, csize;
  int staadr, stoadr, staflag, vsize;
  int i,j;
  volatile int dmadelay, dmaflag, loop;

  for(wordcnt=0;wordcnt<depth;){
    dmaflag = 0;
    if(depth<TMB2_WINDOWSIZE) csize = (depth+1)* 4; /* long word -> char size */
    else csize = TMB2_WINDOWSIZE * 4;  /* long word -> char size */
    vme_dma_vread32_start(maddr+TMB2_OUTBUFF1+icn*0x800, csize);
    delay_us();  /* delay about 1us */
    dmadelay = 0;
    for(dmadelay=0;dmadelay<1000000;dmadelay++){
      if(vme_dma_vread32_store((char *)(buff+wordcnt),csize)){
	//	printk("DMA Delay:%d\n",dmadelay);
	//	dmadelay = 5555;
	dmaflag = 1;
	break;
      }else{
	delay_us();
      }
    }
    //    printk("DMA dealy:%d\n",dmadelay);
    //    wordcnt += (TMB2_WINDOWSIZE-1);
    wordcnt += (csize/4-1);
    printk("DMA Read: size %d  wdcnt %d\n",csize/4,wordcnt);

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

