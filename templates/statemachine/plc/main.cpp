//***************************************************************************
//                          main.cpp  -  description
//                             -------------------
//  begin            : Sa. Mai 4 09:29:07 2013
//  generated by     : pvdevelop (C) Lehrig Software Engineering
//  email            : lehrig@t-online.de
//***************************************************************************
#include "plcapp.h"

SHM_DATA      *shm_data;
rlSharedMemory shm("/srv/automation/shm/plc.shm", sizeof(SHM_DATA));
rlSerial       tty;
rlModbus       mb;
rlMutex        mb_mutex;
rlState        sm1, sm2;

// helper functions
int printBinByte(unsigned char val)
{
  if(val & BIT7) printf("1");
  else           printf("0");
  if(val & BIT6) printf("1");
  else           printf("0");
  if(val & BIT5) printf("1");
  else           printf("0");
  if(val & BIT4) printf("1");
  else           printf("0");
  printf(":");
  if(val & BIT3) printf("1");
  else           printf("0");
  if(val & BIT2) printf("1");
  else           printf("0");
  if(val & BIT1) printf("1");
  else           printf("0");
  if(val & BIT0) printf("1");
  else           printf("0");
  return 0;
}

int printBin(unsigned char *data)
{
  printf("BinData: ");
  printBinByte(data[0]);
  printf(" - ");
  printBinByte(data[1]);
  return 0;
}

// Schneider PLC: first 4 bits are outputs then 6 bits input follow
static int readIO() 
{
  unsigned char data[256];
  int ret;
  
  MB_readInputStatus(1,0,10,data);          // read all IO values from modbus
  shm_data->plc.in.in1 = mb.data2int(data); // store data in shared memory

  if(trace)
  {
    printf("readIO:: ret=%d ", ret); 
    printBin(data);
    printf(" in1=%x\n", shm_data->plc.in.in1);
  }  
  return 0;
}

static int writeIO()
{
  unsigned char coils[8];
  int ret;

  coils[0] = shm_data->plc.out.out1 & 0x0ff;
  MB_forceMultipleCoils(1,0,4,coils);       // write the 4 output bits to modbus

  return 0;
}

int main()
{
  if(trace) printf("plc starting ...\n");
  if(trace) printf("shm.status=%d\n", shm.status);

  // --- Initialize our DCS ---
  if(shm.status != rlSharedMemory::OK)
  {
    printf("ERROR: shared memory status is not ok\n");
    return -1;
  }
  shm_data = (SHM_DATA *) shm.getUserAdr();
  memset(shm_data,0,sizeof(SHM_DATA));
retry:
  if(tty.openDevice("/dev/ttyUSB0",B9600,1,1,8,1,rlSerial::NONE) < 0)
  {
    printf("ERROR: openDevice(\"/dev/tty/USB0\")\n");
    rlsleep(5000);
    goto retry;
  }
  mb.registerSerial(&tty);

  // --- Start our treads ---
  startStepsStm1(&sm1, 100); // start statemachine 1
  startStepsStm2(&sm2, 100); // start statemachine 2
  // TODO: eventually start a thread for logging data (into a database)
  // TODO: eventually start threads for continuous loop back controller with rlController from rllib

  // --- Continuous loop for data acquisition ---
  printf("going to IO loop\n");
  while(1)
  {
    readIO();
    writeIO();
    rlsleep(10);
  }
}

