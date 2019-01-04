#define EFN     253
#define MAXBUFF 3000
//#define MAXBUFF 1000
#define VME
#define VMEINT
#define SBS620
//#define V2718

#define DBUFF

#define INTLEVEL 3

#define USE_MADC32
#define USE_RPV130
#define USE_CAEN
#define USE_TMB2

// definitions for RPV130
#define RPV130ADR   0xf000 
#define OPBUSYCL    0x1
#define OPIWKCLE    0x2
#define OPIWKRST    0x4
#define OPSCACLER   0x8
#define OPSCASTART  0x10
#define OPSCASTOP   0x20
#define OPDAQON     0x40
#define OPTMB2BFCH  0x80

#define OPBUSYCL2   16

// definitions for MADC32
#define MADC32ADR 0x20000000

//definitions for V775
#define V775ADR 0x50000000

//definitions for V560
#define V560ADR 0x40000000
#define SCRID 22

// definitions for TMB2
#define TMB2ADR1 0x300000
#define TMB2ADR2 0x310000
#define TMB2_NMEM 2
#define TMB2_NCN  2
//#define TMB2DEPTH 0x80000
#define TMB2DEPTH 0x8000
#define FADCDEPTH 256
#define TPCDEPTH 8193

//unsigned int tmb2adr[2]={TMB2ADR1,TMB2ADR2};
unsigned int tmb2adr[2]={TMB2ADR1,TMB2ADR2};
unsigned int tmb2data[TMB2_NMEM][TMB2_NCN][TMB2DEPTH];
unsigned int evcnt_madc; /* total events from MADC32 */
