//
// MADC-32
//

/* Address Map */
#define MADC32ADR                   0x20000000
#define MADC32_DATA                 0x0000
#define MADC32_THRESHOLD            0x4000 /* to 0x403f, 16bit words */
#define MADC32_MODULE_ID            0x6004
#define MADC32_SOFT_RESET           0x6008
#define MADC32_IRQ_LEVEL            0x6010
#define MADC32_IRQ_THRESHOLD        0x6018
#define MADC32_MAX_TRANSFER_DATA    0x601A
#define MADC32_BUFFER_DATA_LENGTH   0x6030
#define MADC32_READOUT_RESET        0x6034
#define MADC32_MULTIEVENT           0x6036
#define MADC32_MARKING_TYPE         0x6038
#define MADC32_START_ACQ            0x603A
#define MADC32_FIFO_RESET           0x603C
#define MADC32_DATA_READY           0x603E
#define MADC32_ADC_RESOLUTION       0x6042
#define MADC32_SLC_OFF              0x6048
#define MADC32_SKIP_OORANGE         0x604A
#define MADC32_HOLD_DELAY0          0x6050
#define MADC32_HOLD_DELAY1          0x6052
#define MADC32_HOLD_WIDTH0          0x6054
#define MADC32_HOLD_WIDTH1          0x6056
#define MADC32_USE_GG               0x6058
#define MADC32_INPUT_RANGE          0x6060
#define MADC32_NIM_GAT1_OSC         0x606A
#define MADC32_NIM_BUSY             0x606E
#define MADC32_RESET_CTR_AB         0x6090
#define MADC32_TS_SOURCES           0x6096

/* Data Bit Map */
#define MADC32_BOE_BIT        0x40000000
#define MADC32_EOE_BIT        0xC0000000

/* Data Bit Mask */
#define MADC32_SUBHEADER      0xC0000000


