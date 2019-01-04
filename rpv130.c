/* modified for uPIC TPC on 25 Sep. 2013 by T. Kawabata */
#include "rpv130.h"


int rpv130_write( unsigned int maddr, unsigned short val){
  set_amsr(0x29);
  vwrite16(maddr, &val);
  set_amsr(0x09);
  return 1;
}

int rpv130_level(unsigned int maddr, unsigned short val){
  set_amsr(0x29);
  rpv130_write(maddr+RPV130_LEVEL, val);
  set_amsr(0x09);

  return 1;
}

int rpv130_output(unsigned int maddr, unsigned short val){
  set_amsr(0x29);
  rpv130_write(maddr+RPV130_PULSE, val);
  set_amsr(0x09);

  return 1;
}

int rpv130_reset( unsigned int maddr){
  set_amsr(0x29);
  rpv130_write(maddr+RPV130_CTL1, 0x3);
  rpv130_write(maddr+RPV130_CTL2, 0x3);
  set_amsr(0x09);
  return 1;
}

int rpv130_enable( unsigned int maddr){
  set_amsr(0x29);
  rpv130_write(maddr+RPV130_CTL1, 0x18);
  rpv130_write(maddr+RPV130_CTL2, 0x18);
  set_amsr(0x09);
  return 1;
}

int rpv130_segdata(unsigned int maddr, int mode){
  set_amsr(0x29);
  vread16(maddr+mode,(short *)(data+mp));
  set_amsr(0x09);
  mp += 1;
  segmentsize += 1;
  
  return segmentsize;
}

int rpv130_segdata_v(unsigned int maddr, int mode, unsigned short *sval){
  set_amsr(0x29);
  vread16(maddr+mode,(short *)(data+mp));
  set_amsr(0x09);

  memcpy((char *)sval,(char *)(data+mp),2);

  mp += 1;
  segmentsize += 1;
  
  return segmentsize;
}

int rpv130_clear(unsigned int maddr){
  short sval;

  sval = RPV130_CLEAR1OR2 | RPV130_CLEAR3 | RPV130_MASK1OR2;
  set_amsr(0x29);
  vwrite16(maddr+RPV130_CTL1,&sval);
  vwrite16(maddr+RPV130_CTL2,&sval);
  set_amsr(0x09);

  return 1;
}
