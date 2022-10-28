/**
  **********************************************************************************
  * @file      ebyte_e31x_register_def.h
  * @brief     AX5243ºƒ¥Ê∆˜∂®“Â ∫ÕAX5043ª˘±æ“ª÷¬   
  **********************************************************************************
  */

//Revision & Interface Probing
#define  REG_AX5043_REVISION               0x00
#define  REG_AX5043_SCRATCH                0x01
#define  REG_AX5043_PWRMODE                0x02
//Voltage Regulator
#define  REG_AX5043_POWSTAT                0x03
#define  REG_AX5043_POWSTICKYSTAT          0x04
#define  REG_AX5043_POWIRQMASK             0x05
//Interrupt Control
#define  REG_AX5043_IRQMASK1               0x06
#define  REG_AX5043_IRQMASK0               0x07
#define  REG_AX5043_RADIOEVENTMASK1        0x08
#define  REG_AX5043_RADIOEVENTMASK0        0x09
#define  REG_AX5043_IRQINVERSION1          0x0A
#define  REG_AX5043_IRQINVERSION0          0x0B
#define  REG_AX5043_IRQREQUEST1            0x0C
#define  REG_AX5043_IRQREQUEST0            0x0D
#define  REG_AX5043_RADIOEVENTREQ1         0x0E
#define  REG_AX5043_RADIOEVENTREQ0         0x0F
//Modulation & Framing
#define  REG_AX5043_MODULATION             0x10
#define  REG_AX5043_ENCODING               0x11
#define  REG_AX5043_FRAMING                0x12
#define  REG_AX5043_CRCINIT3               0x14
#define  REG_AX5043_CRCINIT2               0x15
#define  REG_AX5043_CRCINIT1               0x16
#define  REG_AX5043_CRCINIT0               0x17

//Forward Error Correction
#define  REG_AX5043_FEC                    0x18
#define  REG_AX5043_FECSYNC                0x19
#define  REG_AX5043_FECSTATUS              0x1A
//Status
#define     REG_AX5043_RADIOSTATE             0x1C
#define     REG_AX5043_XTALSTATUS             0x1D
//Pin Configuration
#define  REG_AX5043_PINSTATE               0x20
#define  REG_AX5043_PINFUNCSYSCLK          0x21
#define  REG_AX5043_PINFUNCDCLK            0x22
#define  REG_AX5043_PINFUNCDATA            0x23
#define  REG_AX5043_PINFUNCANTSEL          0x25
#define  REG_AX5043_PINFUNCPWRAMP          0x26
#define  REG_AX5043_PWRAMP                 0x27
#define     REG_AX5043_FIFOSTAT               0x28
#define     REG_AX5043_FIFODATA               0x29
#define     REG_AX5043_FIFOFREE1              0x2C
#define     REG_AX5043_FIFOFREE0              0x2D


#define  REG_AX5043_FIFOTHRESH1            0x2E
#define  REG_AX5043_FIFOTHRESH0            0x2F

#define REG_AX5043_PLLLOOPBOOST   0x38
#define REG_AX5043_PLLCPIBOOST   0X39
//==============================================================
//config register Addr
//============================================================
//GPIO
#define  REG_AX5043_WAKEUPXOEARLY          0x06E
//#define  REG_AX5043_PLLOOPBOOST            0x038
//#define  REG_AX5043_PLLCPIBOOST            0x039
#define  REG_AX5043_IFFREQ1                0x100
#define  REG_AX5043_IFFREQ0                0x101
#define  REG_AX5043_DECIMATION             0x102
#define  REG_AX5043_RXDATARATE2            0x103
#define  REG_AX5043_RXDATARATE1            0x104
#define  REG_AX5043_RXDATARATE0            0x105
#define  REG_AX5043_MAXDROFFSET2           0x106
#define  REG_AX5043_MAXDROFFSET1           0x107
#define  REG_AX5043_MAXDROFFSET0           0x108
#define  REG_AX5043_MAXRFOFFSET2           0x109
#define  REG_AX5043_MAXRFOFFSET1           0x10A
#define  REG_AX5043_MAXRFOFFSET0           0x10B
#define  REG_AX5043_FSKDMAX1               0x10C
#define  REG_AX5043_FSKDMAX0               0x10D
#define  REG_AX5043_FSKDMIN1               0x10E
#define  REG_AX5043_FSKDMIN0               0x10F

#define REG_AX5043_AFSKSPACE1   0x110
#define REG_AX5043_AFSKSPACE0   0x111
#define REG_AX5043_AFSKMARK1   0x112
#define REG_AX5043_AFSKMARK0   0x113
#define REG_AX5043_AFSKCTRL    0x114

#define  REG_AX5043_AMPLFILTER             0x115
#define  REG_AX5043_FREQUENCYLEAK   0x116
#define  REG_AX5043_RXPARAMSETS            0x117
#define  REG_AX5043_AGCGAIN0               0x120
#define  REG_AX5043_AGCTARGET0             0x121
#define  REG_AX5043_AGCAHYST0    0x122
#define  REG_AX5043_AGCMINMAX0    0x123

#define  REG_AX5043_TIMEGAIN0              0x124
#define  REG_AX5043_DRGAIN0                0x125
#define  REG_AX5043_PHASEGAIN0             0x126
#define  REG_AX5043_FREQUENCYGAINA0        0x127
#define  REG_AX5043_FREQUENCYGAINB0        0x128
#define  REG_AX5043_FREQUENCYGAINC0        0x129
#define  REG_AX5043_FREQUENCYGAIND0        0x12A
#define  REG_AX5043_AMPLITUDEGAIN0         0x12B
#define  REG_AX5043_FREQDEV10              0x12C
#define  REG_AX5043_FREQDEV00              0x12D
#define  REG_AX5043_FOURFSK0               0x12E
#define  REG_AX5043_BBOFFSRES0             0x12F
#define  REG_AX5043_AGCGAIN1               0x130
#define  REG_AX5043_AGCTARGET1             0x131
#define  REG_AX5043_AGCAHYST1              0x132
#define  REG_AX5043_AGCMINMAX1             0x133
#define  REG_AX5043_TIMEGAIN1              0x134
#define  REG_AX5043_DRGAIN1                0x135
#define  REG_AX5043_PHASEGAIN1             0x136
#define  REG_AX5043_FREQUENCYGAINA1        0x137
#define  REG_AX5043_FREQUENCYGAINB1        0x138
#define  REG_AX5043_FREQUENCYGAINC1        0x139
#define  REG_AX5043_FREQUENCYGAIND1        0x13A
#define  REG_AX5043_AMPLITUDEGAIN1         0x13B
#define  REG_AX5043_FREQDEV11              0x13C
#define  REG_AX5043_FREQDEV01              0x13D
#define  REG_AX5043_FOURFSK1               0x13E
#define  REG_AX5043_BBOFFSRES1             0x13F
#define  REG_AX5043_AGCGAIN3               0x150
#define  REG_AX5043_AGCTARGET3             0x151
#define  REG_AX5043_AGCAHYST3              0x152
#define  REG_AX5043_AGCMINMAX3             0x153
#define  REG_AX5043_TIMEGAIN3              0x154
#define  REG_AX5043_DRGAIN3                0x155
#define  REG_AX5043_PHASEGAIN3             0x156
#define  REG_AX5043_FREQUENCYGAINA3        0x157
#define  REG_AX5043_FREQUENCYGAINB3        0x158
#define  REG_AX5043_FREQUENCYGAINC3        0x159
#define  REG_AX5043_FREQUENCYGAIND3        0x15A
#define  REG_AX5043_AMPLITUDEGAIN3         0x15B
#define  REG_AX5043_FREQDEV13              0x15C
#define  REG_AX5043_FREQDEV03              0x15D
#define  REG_AX5043_FOURFSK3               0x15E
#define  REG_AX5043_BBOFFSRES3             0x15F
#define  REG_AX5043_MODCFGF                0x160
#define  REG_AX5043_FSKDEV2                0x161
#define  REG_AX5043_FSKDEV1                0x162
#define  REG_AX5043_FSKDEV0                0x163
#define  REG_AX5043_MODCFGA                0x164
#define  REG_AX5043_TXRATE2                0x165
#define  REG_AX5043_TXRATE1                0x166
#define  REG_AX5043_TXRATE0                0x167
#define  REG_AX5043_TXPWRCOEFFA1           0x168    //+   00 00
#define  REG_AX5043_TXPWRCOEFFA0           0x169    //+   00 00
#define  REG_AX5043_TXPWRCOEFFB1           0x16A  //0F 0F
#define  REG_AX5043_TXPWRCOEFFB0           0x16B  //FF  FF
#define  REG_AX5043_TXPWRCOEFFC1           0x16C  //+   00 00
#define  REG_AX5043_TXPWRCOEFFC0           0x16D  //+   00 00
#define  REG_AX5043_TXPWRCOEFFD1           0x16E  //+   00 00
#define  REG_AX5043_TXPWRCOEFFD0           0x16F  //+   00 00
#define  REG_AX5043_TXPWRCOEFFE1           0x170  //+   00 00
#define  REG_AX5043_TXPWRCOEFFE0           0x171  //+   00 00
#define  REG_AX5043_PLLLOCKDET             0x182   //+  03 03
#define  REG_AX5043_PLLRNGCLK              0x183
#define  REG_AX5043_BBTUNE                 0x188
#define  REG_AX5043_BBOFFSCAP              0x189
#define  REG_AX5043_PKTADDRCFG             0x200
#define  REG_AX5043_PKTLENCFG              0x201
#define  REG_AX5043_PKTLENOFFSET           0x202
#define  REG_AX5043_PKTMAXLEN              0x203
#define  REG_AX5043_MATCH0PAT3             0x210
#define  REG_AX5043_MATCH0PAT2             0x211
#define  REG_AX5043_MATCH0PAT1             0x212
#define  REG_AX5043_MATCH0PAT0             0x213
#define  REG_AX5043_MATCH0LEN              0x214
#define  REG_AX5043_MATCH0MIN              0x215    //+ 00 00
#define  REG_AX5043_MATCH0MAX              0x216
#define  REG_AX5043_MATCH1PAT1             0x218
#define  REG_AX5043_MATCH1PAT0             0x219
#define  REG_AX5043_MATCH1LEN              0x21C
#define  REG_AX5043_MATCH1MIN              0x21D   //+ 00 00
#define  REG_AX5043_MATCH1MAX              0x21E
#define  REG_AX5043_TMGTXBOOST             0x220  //+  32 32
#define  REG_AX5043_TMGTXSETTLE            0x221  //+ 0A 0A
#define  REG_AX5043_TMGRXBOOST             0x223
#define  REG_AX5043_TMGRXSETTLE            0x224
#define  REG_AX5043_TMGRXOFFSACQ           0x225
#define  REG_AX5043_TMGRXCOARSEAGC         0x226
#define  REG_AX5043_TMGRXAGC               0x227
#define  REG_AX5043_TMGRXRSSI              0x228
#define  REG_AX5043_TMGRXPREAMBLE1         0x229  //
#define  REG_AX5043_TMGRXPREAMBLE2         0x22A
#define  REG_AX5043_TMGRXPREAMBLE3         0x22B  //+ 00 00
#define  REG_AX5043_RSSIREFERENCE          0x22C  // 19 19
#define  REG_AX5043_RSSIABSTHR             0x22D
#define  REG_AX5043_BGNDRSSIGAIN           0x22E   //02 02
#define  REG_AX5043_BGNDRSSITHR            0x22F
#define  REG_AX5043_PKTCHUNKSIZE           0x230
#define  REG_AX5043_PKTMISCFLAGS           0x231  //03 03
#define     REG_AX5043_PKTSTOREFLAGS       0x232
#define  REG_AX5043_PKTACCEPTFLAGS         0x233    //20 20

#define REG_AX5043_GPADCCTRL  0x300
#define REG_AX5043_GPADCPERIOD  0x301
#define REG_AX5043_LPOSCCONFIG   0x310
#define REG_AX5043_LPOSCKFILT1   0x312
#define REG_AX5043_LPOSCKFILT0   0x313
#define REG_AX5043_LPOSCREF1   0x314
#define REG_AX5043_LPOSCREF0   0x315
#define REG_AX5043_LPOSCFREQ1   0x316
#define REG_AX5043_LPOSCFREQ0   0x317
#define REG_AX5043_LPOSCPER1   0x318
#define REG_AX5043_LPOSCPER0   0x319


#define  REG_AX5043_DACVALUE1              0x330
#define  REG_AX5043_DACVALUE0              0x331
#define  REG_AX5043_DACCONFIG              0x332
#define  REG_AX5043_0xF08                  0xF08
#define  REG_AX5043_0xF10                  0xF10
#define  REG_AX5043_0xF11                  0xF11
#define  REG_AX5043_0xF1C                  0xF1C
#define  REG_AX5043_0xF21                  0xF21
#define  REG_AX5043_0xF22                  0xF22
#define  REG_AX5043_0xF23                  0xF23
#define  REG_AX5043_0xF26                  0xF26
#define  REG_AX5043_0xF34                  0xF34
#define  REG_AX5043_0xF35                  0xF35
#define  REG_AX5043_0xF44                  0xF44
#define     AX5043_Addr_END                   0xFFFF
#define     AX5043_Vule_END                   0xDDDD

//TX &RX
#define  REG_AX5043_PLLLOOP                0x30  //0x1030
#define  REG_AX5043_PLLCPI                 0x31  //0x1031
#define  REG_AX5043_PLLVCODIV              0x32  //0x1032
#define  REG_AX5043_PLLVCOI                0x180  //0x1180
#define  REG_AX5043_XTALCAP                0x184  //0x1184
#define  REG_AX5043_0xF00                  0xF00  //0x1F00
#define  REG_AX5043_POWCTRL1                0xF08
#define  REG_AX5043_REF                    0xF0D  //0x1F0D
#define  REG_AX5043_0xF18                  0xF18  //0x1F18

//===================================================
//AX5043 Channel Set Freq
//#define    REG_AX5043_PLLLOOP                     0x30  //0x1030
#define    REG_AX5043_PLLRANGINGA                 0x33
#define    REG_AX5043_FREQA0                      0x37
#define    REG_AX5043_FREQA1                      0x36
#define    REG_AX5043_FREQA2                      0x35
#define    REG_AX5043_FREQA3                      0x34
#define    REG_AX5043_PLLRANGINGB                 0x3B
#define    REG_AX5043_FREQB3                      0x3C
#define    REG_AX5043_FREQB2                      0x3D
#define    REG_AX5043_FREQB1                      0x3E
#define    REG_AX5043_FREQB0                      0x3F
//====================================================
//set packet data length
#define    REG_AX5043_PKTLENOFFSET                0x202

//LOPCS
#define    REG_AX5043_LPOSCCONFIG                 0x310


//=================================================
//Radio Interrupt
#define    REG_AX5043_PINFUNCIRQ                 0x24

//==================================================
//TX and RX ID
#define   REG_AX5043_PKTADDR0                   0x207
#define   REG_AX5043_PKTADDR1                   0x206
#define   REG_AX5043_PKTADDR2                   0x205
#define   REG_AX5043_PKTADDR3                   0x204
#define   REG_AX5043_PKTADDRMASK0               0x20B
#define   REG_AX5043_PKTADDRMASK1               0x20A
#define   REG_AX5043_PKTADDRMASK2               0x209
#define   REG_AX5043_PKTADDRMASK3               0x208

//====================================================
//ÂàùÂßãÂåñRFÂØÑÂ≠òÂô®ÂÄ? 433.0MHz 10Kpbs  16CH
#define AX5043_PINFUNCSYSCLK            0x01
#define AX5043_PINFUNCDCLK              0x01
#define AX5043_PINFUNCDATA              0x01
#define AX5043_PINFUNCANTSEL            0x01
//#define AX5043_PINFUNCANTSEL            0x04
#define AX5043_PINFUNCPWRAMP            0x01
#define AX5043_WAKEUPXOEARLY            0x01

#define AX5043_MODULATION               0x08
#define AX5043_ENCODING                 0x00
#define AX5043_FRAMING                  0x26
#define AX5043_PLLLOOPBOOST  0x0B
#define AX5043_PLLCPIBOOST  0xC8

#define AX5043_IFFREQ1                  0x00
#define AX5043_IFFREQ0                  0x89
#define AX5043_DECIMATION               0x7F
#define AX5043_RXDATARATE2              0x00
#define AX5043_RXDATARATE1              0x4E
#define AX5043_RXDATARATE0              0xbd

#define AX5043_MAXDROFFSET2             0x00
#define AX5043_MAXDROFFSET1             0x00
#define AX5043_MAXDROFFSET0             0x00
#define AX5043_MAXRFOFFSET2             0x80
#define AX5043_MAXRFOFFSET1             0x01
#define AX5043_MAXRFOFFSET0             0x2F
#define AX5043_FSKDMAX1                 0x00
#define AX5043_FSKDMAX0                 0xa6
#define AX5043_FSKDMIN1                 0xFF
#define AX5043_FSKDMIN0                 0x5a

#define AX5043_AFSKSPACE1_TX  0x00
#define AX5043_AFSKSPACE0_TX  0x40
#define AX5043_AFSKMARK1_TX    0x00
#define AX5043_AFSKMARK0_TX    0x75

#define AX5043_AFSKSPACE1_RX   0x00
#define AX5043_AFSKSPACE0_RX   0x40
#define AX5043_AFSKMARK1_RX    0x00
#define AX5043_AFSKMARK0_RX    0x75

#define AX5043_AFSKCTRL        0x04
#define AX5043_AMPLFILTER               0x00
#define AX5043_FREQUENCYLEAK   0x00
#define AX5043_RXPARAMSETS              0xF4
#define AX5043_AGCGAIN0                 0x98
#define AX5043_AGCTARGET0               0x76
#define AX5043_AGCAHYST0    0x00
#define AX5043_AGCMINMAX0    0x00

#define AX5043_TIMEGAIN0                0x99
#define AX5043_DRGAIN0                  0x93
#define AX5043_PHASEGAIN0               0x83
#define AX5043_FREQUENCYGAINA0          0x0F
#define AX5043_FREQUENCYGAINB0          0x1F
#define AX5043_FREQUENCYGAINC0          0x0B
#define AX5043_FREQUENCYGAIND0          0x0B
#define AX5043_AMPLITUDEGAIN0           0x46

#define AX5043_FREQDEV10                0x00
#define AX5043_FREQDEV00                0x00
#define AX5043_FOURFSK0                 0x00
#define AX5043_BBOFFSRES0               0x00
#define AX5043_AGCGAIN1                 0x98
#define AX5043_AGCTARGET1               0x76
#define AX5043_AGCAHYST1                0x00
#define AX5043_AGCMINMAX1               0x00
#define AX5043_TIMEGAIN1                0x97
#define AX5043_DRGAIN1                  0x92
#define AX5043_PHASEGAIN1               0x83
#define AX5043_FREQUENCYGAINA1          0x0F
#define AX5043_FREQUENCYGAINB1          0x1F
#define AX5043_FREQUENCYGAINC1          0x0b
#define AX5043_FREQUENCYGAIND1          0x0b
#define AX5043_AMPLITUDEGAIN1           0x46
#define AX5043_FREQDEV11                0x00
#define AX5043_FREQDEV01                0x38
#define AX5043_FOURFSK1                 0x00
#define AX5043_BBOFFSRES1               0x00
#define AX5043_AGCGAIN3                 0xFF
#define AX5043_AGCTARGET3               0x76
#define AX5043_AGCAHYST3                0x00
#define AX5043_AGCMINMAX3               0x00
#define AX5043_TIMEGAIN3                0x96
#define AX5043_DRGAIN3                  0x91
#define AX5043_PHASEGAIN3               0x83
#define AX5043_FREQUENCYGAINA3          0x0F
#define AX5043_FREQUENCYGAINB3          0x1F
#define AX5043_FREQUENCYGAINC3          0x0d
#define AX5043_FREQUENCYGAIND3          0x0d
#define AX5043_AMPLITUDEGAIN3           0x46
#define AX5043_FREQDEV13                0x00
#define AX5043_FREQDEV03                0x38
#define AX5043_FOURFSK3                 0x00
#define AX5043_BBOFFSRES3               0x00
#define AX5043_MODCFGF                  0x03
#define AX5043_FSKDEV2                  0x00
#define AX5043_FSKDEV1                  0x00
#define AX5043_FSKDEV0                  0xAF
#define AX5043_MODCFGA                  0x05
//10K
#define AX5043_TXRATE2                  0x00
#define AX5043_TXRATE1                  0x01
#define AX5043_TXRATE0                  0xA3
/*
#define AX5043_TXRATE2                  0x00
#define AX5043_TXRATE1                  0x19
#define AX5043_TXRATE0                  0x35
*/
#define AX5043_TXPWRCOEFFA1             0x00
#define AX5043_TXPWRCOEFFA0             0x00
#define AX5043_TXPWRCOEFFB1             0x0F
#define AX5043_TXPWRCOEFFB0             0xFF
#define AX5043_TXPWRCOEFFC1             0x00
#define AX5043_TXPWRCOEFFC0             0x00
#define AX5043_TXPWRCOEFFD1             0x00
#define AX5043_TXPWRCOEFFD0             0x00
#define AX5043_TXPWRCOEFFE1             0x00
#define AX5043_TXPWRCOEFFE0             0x00

#define AX5043_PLLLOCKDET    0x03
#define AX5043_PLLRNGCLK                0x05
#define AX5043_BBTUNE                   0x0F
#define AX5043_BBOFFSCAP                0x77

#define AX5043_PKTADDRCFG               0x00
#define AX5043_PKTLENCFG                0x80
#define AX5043_PKTLENOFFSET             0x00
#define AX5043_PKTMAXLEN                0xf0

#define AX5043_MATCH0PAT3               0xAA
#define AX5043_MATCH0PAT2               0xCC
#define AX5043_MATCH0PAT1               0xAA
#define AX5043_MATCH0PAT0               0xCC
#define AX5043_MATCH0LEN                0x1F
#define AX5043_MATCH0MIN    0x00
#define AX5043_MATCH0MAX                0x1F
#define AX5043_MATCH1PAT1               0x55
#define AX5043_MATCH1PAT0               0x55
#define AX5043_MATCH1LEN                0x8A
#define AX5043_MATCH1MIN    0x00
#define AX5043_MATCH1MAX                0x0A
#define AX5043_TMGRXBOOST               0x5B
#define AX5043_TMGTXBOOST               0x32

#define AX5043_TMGRXSETTLE      0x3E
#define AX5043_TMGRXOFFSACQ     0x9c
#define AX5043_TMGRXCOARSEAGC   0x32
#define AX5043_TMGRXAGC         0x00
#define AX5043_TMGRXRSSI        0x08

#define AX5043_TMGTXSETTLE    0x0a
#define AX5043_TMGRXPREAMBLE1  0x00
#define AX5043_TMGRXPREAMBLE2  0x35
#define AX5043_TMGRXPREAMBLE3  0x00

#define AX5043_RSSIREFERENCE            0x19
#define AX5043_BGNDRSSIGAIN             0x02

#define AX5043_PKTMISCFLAGS             0x03
#define AX5043_PKTACCEPTFLAGS           0x20

#define AX5043_GPADCCTRL  0x00
#define AX5043_GPADCPERIOD  0x3F

#define AX5043_LPOSCCONFIG   0x00
#define AX5043_LPOSCKFILT1   0x20
#define AX5043_LPOSCKFILT0   0xC4
#define AX5043_LPOSCREF1   0x61
#define AX5043_LPOSCREF0   0xA8
#define AX5043_LPOSCFREQ1   0x00
#define AX5043_LPOSCFREQ0   0x00
#define AX5043_LPOSCPER1   0x00
#define AX5043_LPOSCPER0   0x00

#define AX5043_DACVALUE1                0x00
#define AX5043_DACVALUE0                0x00
#define AX5043_DACCONFIG                0x00
#define AX5043_0xF10                    0x04
#define AX5043_0xF11                    0x00
#define AX5043_0xF1C                    0x07
#define AX5043_0xF21                    0x5C
#define AX5043_0xF22                    0x53
#define AX5043_0xF23                    0x76
#define AX5043_0xF26                    0x92
#define AX5043_0xF34                    0x28

#define AX5043_RSSIABSTHR               0xC6
#define AX5043_BGNDRSSITHR              0x00
#define AX5043_PKTCHUNKSIZE             0x0D
//===========================================================

//ËÆæÁΩÆTXÊ®°ÂºèÁöÑÂÄ?    16MHz  data Rate:10k
#define AX5043_PLLLOOP                  0x0b
#define AX5043_PLLCPI                   0x10
#define AX5043_PLLVCODIV                0x24
#define AX5043_PLLVCOI                  0x99
#define AX5043_0xF00                    0x0F
#define AX5043_REF                      0x03
#define  AX5043_POWCTRL1_TX             0x04
#define AX5043_0xF18                    0x06
#define AX5043_XTALCAP                  0x00


//ËÆæÁΩÆRXÊ®°ÂºèÁöÑÂÄ?
#define RXAX5043_PLLLOOP                    0x0b
#define RXAX5043_PLLCPI                     0x10
#define RXAX5043_PLLVCODIV                  0x25
#define RXAX5043_PLLVCOI                    0x99
#define RXAX5043_0xF00                      0x0F
#define RXAX5043_REF                        0x03
#define AX5043_POWCTRL1_RX                  0x04
#define RXAX5043_0xF18                      0x02
#define RXAX5043_XTALCAP                    0x00


//ËøîÂõû‰∏çÂêåÁöÑÁä∂ÊÄ?
#define AXRADIO_ERR_NOERROR                  0x00
#define AXRADIO_ERR_ERROR                    0x01
#define AXRADIO_ERR_BUSY                     0x02
#define AXRADIO_ERR_TIMEOUT                  0x03
#define AXRADIO_ERR_INVALID                  0x04
#define AXRADIO_ERR_NOCHIP                   0x05
#define AXRADIO_ERR_RANGING                  0x06
#define AXRADIO_ERR_LOCKLOST                 0x07
#define AXRADIO_ERR_RETRANSMISSION           0x08
#define AXRADIO_ERR_RESYNC                   0x09
#define AXRADIO_ERR_RESYNCTIMEOUT            0x0a
#define AXRADIO_ERR_RECEIVESTART             0x0b

#define AXRADIO_STAT_RECEIVE                 0x00
#define AXRADIO_STAT_RECEIVESFD              0x01
#define AXRADIO_STAT_CHANNELSTATE            0x02
#define AXRADIO_STAT_TRANSMITSTART           0x03
#define AXRADIO_STAT_TRANSMITEND             0x04
#define AXRADIO_STAT_TRANSMITDATA            0x05


// power states
#define AX5043_PWRSTATE_POWERDOWN               0x0      //Powerdown; all circuits powered down
#define AX5043_PWRSTATE_DEEPSLEEP               0x1      //Deep Sleep Mode; Chip is fully powered down until SEL is lowered again; looses all register contents
#define AX5043_PWRSTATE_REGS_ON                 0x4
#define AX5043_PWRSTATE_XTAL_ON                 0x5      //Crystal Oscillator enabled
#define AX5043_PWRSTATE_FIFO_ON                 0x7      //FIFO enabled
#define AX5043_PWRSTATE_SYNTH_RX                0x8      //Synthesizer running, Receive Mode
#define AX5043_PWRSTATE_FULL_RX                 0x9      //Receiver Running
#define AX5043_PWRSTATE_WOR_RX                  0xb      //Receiver Wake-on-Radio Mode
#define AX5043_PWRSTATE_SYNTH_TX                0xc      //Synthesizer running, Transmit Mode
#define AX5043_PWRSTATE_FULL_TX                 0xd      //1101 Transmitter Running

//fifo commands
#define AX5043_FIFOCMD_NOP                      0x00
#define AX5043_FIFOCMD_DATA                     0x01
#define AX5043_FIFOCMD_REPEATDATA               0x02
#define AX5043_FIFOCMD_TIMER                    0x10
#define AX5043_FIFOCMD_RSSI                     0x11
#define AX5043_FIFOCMD_FREQOFFS                 0x12
#define AX5043_FIFOCMD_RFFREQOFFS               0x13
#define AX5043_FIFOCMD_DATARATE                 0x14
#define AX5043_FIFOCMD_ANTRSSI                  0x15
#define AX5043_FIFOCMD_TXCTRL                   0x1C
#define AX5043_FIFOCMD_TXPWR                    0x1D

// RF Â∑•‰ΩúÁä∂ÊÄ?
// typedefs

#define trxstate_off                   0x00
#define trxstate_rx                    0x01
#define trxstate_rxwor                 0x02
#define trxstate_wait_xtal             0x03
#define trxstate_xtal_ready            0x04
#define trxstate_pll_ranging           0x05
#define trxstate_pll_ranging_done      0x06
#define trxstate_pll_settling          0x07
#define trxstate_pll_settled           0x08
#define trxstate_tx_xtalwait           0x09
#define trxstate_tx_longpreamble       0x0A
#define trxstate_tx_shortpreamble      0x0B
#define trxstate_tx_packet             0x0C
#define trxstate_tx_waitdone           0x0D
#define trxstate_txcw_xtalwait         0x0E
#define trxstate_txstream_xtalwait     0x0F
#define trxstate_txstream              0x10


/*********add zwf by 0505************/





#define REG_AX5043_TRKRFFREQ1 0x4E
#define REG_AX5043_TRKRFFREQ0  0x4F
#define REG_AX5043_TRKFREQ1 0x50
#define REG_AX5043_TRKFREQ0 0x51
#define REG_AX5043_TRKFSKDEMOD1 0x52
#define REG_AX5043_TRKFSKDEMOD0 0x53
#define REG_AX5043_TIMER2 0x59
#define REG_AX5043_TIMER1 0x5A
#define REG_AX5043_TIMER0 0x5B
#define REG_AX5043_WAKEUPTIMER1 0x68
#define REG_AX5043_WAKEUPTIMER0 0x69
#define REG_AX5043_WAKEUP1 0x6A
#define REG_AX5043_WAKEUP0 0x6B
#define REG_AX5043_WAKEUPFREQ1 0x6C
#define REG_AX5043_WAKEUPFREQ0 0x6D

// additional AX5043 registers & definitions

#define REG_AX5043_0xF01            0x4f01
#define REG_AX5043_0xF0C              0x4f0c
#define REG_AX5043_0xF2F              0x4f2f
#define REG_AX5043_0xF30              0x4f30
#define REG_AX5043_0xF31              0x4f31
#define REG_AX5043_0xF32              0x4f32
#define REG_AX5043_0xF33             0x4f33

#define REG_AX5043_LPOSCCONFIG      0x310
#define REG_AX5043_LPOSCSTATUS      0x311
#define REG_AX5043_LPOSCKFILT1      0x312
#define REG_AX5043_LPOSCKFILT0      0x313
#define REG_AX5043_LPOSCREF1        0x314
#define REG_AX5043_LPOSCREF0        0x315
#define REG_AX5043_LPOSCFREQ1       0x316
#define REG_AX5043_LPOSCFREQ0       0x317
#define REG_AX5043_LPOSCPER1        0x318
#define REG_AX5043_LPOSCPER0        0x319
