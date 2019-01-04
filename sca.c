void sca(void){
  int i;

  init_ncscaler(SCRID);
  for(i=0;i<16;i++) {
    vread32(V560ADR+0x10+i*4,(unsigned int *)(data+mp));
    mp+=2;
  }
  end_ncscaler32();

}
