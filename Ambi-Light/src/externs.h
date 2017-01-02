#ifndef __EXTERNS_H
#define __EXTERNS_H

/* Functions from exeDSP */
extern int uldSys_GetMemoryConfig(int, unsigned int *);

extern int lldDpTmgStillOn1(int);

extern int OsaWait(int);

extern int modIncapt_GetActiveVSize(int, int *);

extern int modIncapt_SetActiveVSize(int, int);

extern int lldDp_Ve_Burst3216(int);

extern int lldDp_Ve_SetDumpAddress(unsigned int);

extern int lldDp_Ve_CapPos(int);

extern int lldDp_Ve_WrCapOn(int);

extern int spIDp_Open(int, unsigned int *);

extern int spIDp_Close(unsigned int);

#endif
