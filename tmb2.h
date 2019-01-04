/*
  TMB2, VME memory module for uPIC TPC
*/

/* Address Map */
#define TMB2_OUTBUFF1        0x0000   /* - 0x07ff (D32) CN1 */
#define TMB2_OUTBUFF2        0x0800   /* - 0x0fff (D32) CN2 */

#define TMB2_CONTREG         0x1010   /* Control Register */
#define TMB2_VERREG          0x1018   /* Version Register */
#define TMB2_STAREG1         0x1020   /* Status Register for CN1 */
#define TMB2_STAREG2         0x1028   /* Status Register for CN2 */
#define TMB2_DEPREG1         0x1030   /* Depth Register for CN1 */
#define TMB2_DEPREG2         0x1038   /* Depth Register for CN2 */
#define TMB2_CONTREG1        0x1040   /* Counter Register for CN1 */
#define TMB2_CONTREG2        0x1048   /* Counter Register for CN2 */
#define TMB2_RECONTREG1      0x1050   /* Read Counter Register for CN1 */
#define TMB2_RECONTREG2      0x1058   /* Read Counter Register for CN2 */

/* Bits for Control regsiter */
#define TMB2_CONT_START      0x01
#define TMB2_CONT_STOP       0x02
#define TMB2_CONT_CRESET     0x04
#define TMB2_CONT_CLEAR      0x08
#define TMB2_CONT_ALLCLEAR   0x10
#define TMB2_CONT_CLALL1     0x20
#define TMB2_CONT_CLALL0     0x00
#define TMB2_CONT_CLINC      0x60
#define TMB2_CONT_BUFCHANGE  0x80

/* Bit Mask for version regsiter */
#define TMB2_VER_REV         0x000000ff
#define TMB2_VER_VER         0x0000ff00
#define TMB2_VER_SW1         0x000f0000
#define TMB2_VER_SIDE        0x01000000

/* Bit Mask for status regsiter */
#define TMB2_STAT_RUN         0x01
#define TMB2_STAT_DATAREADY   0x02
#define TMB2_STAT_DATAINPUT   0x04
#define TMB2_STAT_RANGEOVER   0x08
#define TMB2_STAT_BUSYCLEAR   0x10

#define TMB2_WINDOWSIZE   0x200

void tmb2_reset(unsigned int maddr);
void tmb2_switchbuf(unsigned int maddr);
void tmb2_start(unsigned int maddr);
void tmb2_stop(unsigned int maddr);
void tmb2_clear(unsigned int maddr,unsigned int iflag);
void tmb2_setdepth(unsigned int maddr,int ibuf, int depth);
int tmb2_segdata(unsigned int maddr,int icn,int depth,int *buff);
int tmb2_readdepth(unsigned int maddr,int icn);
int tmb2_readcnt(unsigned int maddr,int icn);
int tmb2_dmasegdata(unsigned int maddr,int icn,int depth,int *buff);

