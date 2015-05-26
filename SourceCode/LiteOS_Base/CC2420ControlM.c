#include "avrhardware.h"
#include "micazhardware.h"
#include "scheduling.h"
#include "Leds.h"
//#include "Timer.h"
#include "amcommon.h"
#include "amradio.h"
#include "byteorder.h"
#include "cc2420Const.h"
#include "CC2420ControlM.h"
#include "CC2420RadioM.h"
#include "HPLCC2420FIFOM.h"
#include "HPLCC2420InterruptM.h"
#include "HPLCC2420M.h"
#include "HPLTimer1.h"
#include "Types.h"


// added for post-compile frequency changing
uint8_t CC2420_CHANNEL;
uint8_t CC2420_RFPOWER;

uint8_t CC2420ControlM_state;
uint16_t CC2420ControlM_gCurrentParameters[14];


 inline   

result_t CC2420ControlM_CC2420Control_VREFOff(void)

{
  TOSH_CLR_CC_VREN_PIN();

  TOSH_uwait(600);
  return SUCCESS;
}




inline  
result_t CC2420ControlM_SplitControl_init(void)
{

  uint8_t _state = FALSE;

  
  
  { _atomic_t _atomic = _atomic_start();
    {
      if (CC2420ControlM_state == CC2420ControlM_IDLE_STATE) {
          CC2420ControlM_state = CC2420ControlM_INIT_STATE;
          _state = TRUE;
        }
    }
    //Qing Revision

	//IDLE state
    CC2420ControlM_state = CC2420ControlM_IDLE_STATE;
	
    CC2420_CHANNEL =            CC2420_DEF_CHANNEL;
    CC2420_RFPOWER =            CC2420_DEF_RFPOWER;
    
    _atomic_end(_atomic); }

  if (!_state) {
    return FAIL;
    }

  CC2420ControlM_HPLChipconControl_init();

  //Basically, reset everything page 64
  CC2420ControlM_gCurrentParameters[CP_MAIN] = 0xf800;


  //Basically, check page 65. Very easy 
  CC2420ControlM_gCurrentParameters[CP_MDMCTRL0] = ((((0 << 11) | (
  2 << 8)) | (3 << 6)) | (
  1 << 5)) | (2 << 0);

  //Page 66
  CC2420ControlM_gCurrentParameters[CP_MDMCTRL1] = 20 << 6;


  //reset values
  CC2420ControlM_gCurrentParameters[CP_RSSI] = 0xE080;

  //reset
  CC2420ControlM_gCurrentParameters[CP_SYNCWORD] = 0xA70F;

  //the last one, according to page 52 of the data sheet and page 67, default to output 0dbm 
  
  CC2420ControlM_gCurrentParameters[CP_TXCTRL] = ((((1 << 14) | (
  1 << 13)) | (3 << 6)) | (
  1 << 5)) | (CC2420_RFPOWER << 0);


  CC2420ControlM_gCurrentParameters[CP_RXCTRL0] = (((((1 << 12) | (
  2 << 8)) | (3 << 6)) | (
  2 << 4)) | (1 << 2)) | (
  1 << 0);

  CC2420ControlM_gCurrentParameters[CP_RXCTRL1] = (((((1 << 11) | (
  1 << 9)) | (1 << 6)) | (
  1 << 4)) | (1 << 2)) | (
  2 << 0);


 //PAGE 51 of the manual 
 
  CC2420ControlM_gCurrentParameters[CP_FSCTRL] = (1 << 14) | ((
  357 + 5 * (CC2420_CHANNEL - 11)) << 0);

  CC2420ControlM_gCurrentParameters[CP_SECCTRL0] = (((1 << 8) | (
  1 << 7)) | (1 << 6)) | (
  1 << 2);

  CC2420ControlM_gCurrentParameters[CP_SECCTRL1] = 0;
  CC2420ControlM_gCurrentParameters[CP_BATTMON] = 0;


 //fifop and cca polarity are inversed
 
  CC2420ControlM_gCurrentParameters[CP_IOCFG0] = (127 << 0) | (
  1 << 9);

  CC2420ControlM_gCurrentParameters[CP_IOCFG1] = 0;


  { _atomic_t _atomic = _atomic_start();
    CC2420ControlM_state = CC2420ControlM_INIT_STATE_DONE;
    _atomic_end(_atomic); }

  postTask(CC2420ControlM_taskInitDone, 5);

  return SUCCESS;
}




inline    result_t CC2420ControlM_CCA_startWait(bool arg_0xa422588){

  unsigned char result;



  result = HPLCC2420InterruptM_CCA_startWait(arg_0xa422588);



  return result;

}




 inline   

result_t CC2420ControlM_CC2420Control_VREFOn(void)

{
  TOSH_SET_CC_VREN_PIN();

  TOSH_uwait(600);
  return SUCCESS;
}






inline   result_t CC2420ControlM_SplitControl_initDone(void){

  unsigned char result;



  result = CC2420RadioM_CC2420SplitControl_initDone();



  return result;

}




 inline  

void CC2420ControlM_taskInitDone(void)

{
  CC2420ControlM_SplitControl_initDone();
}







inline    uint8_t CC2420ControlM_HPLChipcon_cmd(uint8_t arg_0xa403928){

  unsigned char result;



  result = HPLCC2420M_HPLCC2420_cmd(arg_0xa403928);



  return result;

}






inline    uint8_t CC2420ControlM_HPLChipcon_write(uint8_t arg_0xa403d80, uint16_t arg_0xa403ed0){

  unsigned char result;



  result = HPLCC2420M_HPLCC2420_write(arg_0xa403d80, arg_0xa403ed0);



  return result;

}




 inline   

result_t CC2420ControlM_CC2420Control_OscillatorOn(void)

{
  uint16_t i;
  uint8_t status;

  i = 0;

  CC2420ControlM_HPLChipcon_write(0x1D, 24);


  CC2420ControlM_CCA_startWait(TRUE);


  status = CC2420ControlM_HPLChipcon_cmd(0x01);

  return SUCCESS;
}






inline   result_t CC2420ControlM_HPLChipconControl_start(void){

  unsigned char result;




  result = HPLTimer1M_StdControl_start();



  return result;

}

 inline  

result_t CC2420ControlM_SplitControl_start(void)

{
  result_t status;
  uint8_t _state = FALSE;

  { _atomic_t _atomic = _atomic_start();

    {
      if (CC2420ControlM_state == CC2420ControlM_INIT_STATE_DONE) {
          CC2420ControlM_state = CC2420ControlM_START_STATE;
          _state = TRUE;
        }
    }

    _atomic_end(_atomic); }
  if (!_state) {
    return FAIL;
    }
  CC2420ControlM_HPLChipconControl_start();

  CC2420ControlM_CC2420Control_VREFOn();

  TOSH_CLR_CC_RSTN_PIN();
  TOSH_wait();
  TOSH_SET_CC_RSTN_PIN();
  TOSH_wait();


  status = CC2420ControlM_CC2420Control_OscillatorOn();

  return status;
}






 inline   

result_t CC2420ControlM_CC2420Control_RxMode(void)

{
  CC2420ControlM_HPLChipcon_cmd(0x03);
  return SUCCESS;
}






inline   result_t CC2420ControlM_SplitControl_startDone(void){

  unsigned char result;



  result = CC2420RadioM_CC2420SplitControl_startDone();



  return result;

}

 inline  

result_t CC2420ControlM_CC2420Control_TuneManual(uint16_t DesiredFreq)

{
  int fsctrl;
  uint8_t status;

  fsctrl = DesiredFreq - 2048;
  CC2420ControlM_gCurrentParameters[CP_FSCTRL] = (CC2420ControlM_gCurrentParameters[CP_FSCTRL] & 0xfc00) | (fsctrl << 0);
  status = CC2420ControlM_HPLChipcon_write(0x18, CC2420ControlM_gCurrentParameters[CP_FSCTRL]);


 //IF THE oscillator is running, turn on the rx mode 
  if (status & (1 << 6)) {
    CC2420ControlM_HPLChipcon_cmd(0x03);
    }

  return SUCCESS;
}

inline 
result_t CC2420ControlM_CC2420Control_TunePower( uint8_t powerlevel )
 {
   uint8_t status;
   //  cc2420controlm_gCurrentParameters[CP_FSCTRL] = (cc2420controlm_gCurrentParameters[CP_FSCTRL] & 0xfc00) | (fsctrl << 0);
   CC2420ControlM_gCurrentParameters[ CP_TXCTRL ] = ( CC2420ControlM_gCurrentParameters[ CP_TXCTRL ]& 0xffe0 ) | ( powerlevel& 0x1f );
   status = CC2420ControlM_HPLChipcon_write( 0x15, CC2420ControlM_gCurrentParameters[ CP_FSCTRL ] );
   //IF THE oscillator is running, turn on the rx mode 
   // STATUS bit 06 means that the oscillator is running or not 
   if ( status& ( 1 << 6 )) {
      CC2420ControlM_HPLChipcon_cmd( 0x03 );
   }
   return SUCCESS;
}




//the channel must be 11 to 26
inline result_t CC2420ControlM_CC2420Control_TuneChannel( uint8_t channel )
 {
   uint16_t freq;
   freq = 2405+5 *( channel - 11 );
   CC2420ControlM_CC2420Control_TuneManual( freq );
   return SUCCESS;
}


 inline   

result_t CC2420ControlM_HPLChipconRAM_writeDone(uint16_t addr, uint8_t length, uint8_t *buffer)

{
  return SUCCESS;
}





inline    result_t CC2420ControlM_HPLChipconRAM_write(uint16_t arg_0xa45ad38, uint8_t arg_0xa45ae80, uint8_t *arg_0xa45afe0){

  unsigned char result;



  result = HPLCC2420M_HPLCC2420RAM_write(arg_0xa45ad38, arg_0xa45ae80, arg_0xa45afe0);



  return result;

}

 inline  

result_t CC2420ControlM_CC2420Control_setShortAddress(uint16_t addr)

{
  addr = toLSB16(addr);
  return CC2420ControlM_HPLChipconRAM_write(0x16A, 2, (uint8_t *)&addr);
}


inline    uint16_t CC2420ControlM_HPLChipcon_read(uint8_t arg_0xa4103b0){

  unsigned int result;



  result = HPLCC2420M_HPLCC2420_read(arg_0xa4103b0);



  return result;

}

 inline 

bool CC2420ControlM_SetRegs(void)

{
  uint16_t data;

  CC2420ControlM_HPLChipcon_write(0x10, CC2420ControlM_gCurrentParameters[CP_MAIN]);
  CC2420ControlM_HPLChipcon_write(0x11, CC2420ControlM_gCurrentParameters[CP_MDMCTRL0]);
  data = CC2420ControlM_HPLChipcon_read(0x11);
  if (data != CC2420ControlM_gCurrentParameters[CP_MDMCTRL0]) {

    return FALSE;
    }
  CC2420ControlM_HPLChipcon_write(0x12, CC2420ControlM_gCurrentParameters[CP_MDMCTRL1]);
  CC2420ControlM_HPLChipcon_write(0x13, CC2420ControlM_gCurrentParameters[CP_RSSI]);
  CC2420ControlM_HPLChipcon_write(0x14, CC2420ControlM_gCurrentParameters[CP_SYNCWORD]);
  CC2420ControlM_HPLChipcon_write(0x15, CC2420ControlM_gCurrentParameters[CP_TXCTRL]);
  CC2420ControlM_HPLChipcon_write(0x16, CC2420ControlM_gCurrentParameters[CP_RXCTRL0]);
  CC2420ControlM_HPLChipcon_write(0x17, CC2420ControlM_gCurrentParameters[CP_RXCTRL1]);
  CC2420ControlM_HPLChipcon_write(0x18, CC2420ControlM_gCurrentParameters[CP_FSCTRL]);

  CC2420ControlM_HPLChipcon_write(0x19, CC2420ControlM_gCurrentParameters[CP_SECCTRL0]);
  CC2420ControlM_HPLChipcon_write(0x1A, CC2420ControlM_gCurrentParameters[CP_SECCTRL1]);
  CC2420ControlM_HPLChipcon_write(0x1C, CC2420ControlM_gCurrentParameters[CP_IOCFG0]);
  CC2420ControlM_HPLChipcon_write(0x1D, CC2420ControlM_gCurrentParameters[CP_IOCFG1]);

  CC2420ControlM_HPLChipcon_cmd(0x09);
  CC2420ControlM_HPLChipcon_cmd(0x08);

  return TRUE;
}

 inline  








void CC2420ControlM_PostOscillatorOn(void)

{
//This fucntion sets up all the registers of the radio module 
  CC2420ControlM_SetRegs();

//This function sets up the short address of the node, and therefore, if the mac frame includes a short address, it should be matched

  CC2420ControlM_CC2420Control_setShortAddress(CURRENT_NODE_ID);

//tHIS TURNS THE CHANELL
  CC2420ControlM_CC2420Control_TuneManual(((CC2420ControlM_gCurrentParameters[CP_FSCTRL] << 0) & 0x1FF) + 2048);
  { _atomic_t _atomic = _atomic_start();

    CC2420ControlM_state = CC2420ControlM_START_STATE_DONE;

    _atomic_end(_atomic); }
  CC2420ControlM_SplitControl_startDone();
}




 inline   
result_t CC2420ControlM_CCA_fired(void)

{

  CC2420ControlM_HPLChipcon_write(0x1D, 0);
  postTask(CC2420ControlM_PostOscillatorOn, 5);
  return FAIL;
}


inline   result_t CC2420ControlM_HPLChipconControl_init(void){
  unsigned char result;

  result = HPLCC2420M_StdControl_init();
  result = rcombine(result, HPLTimer1M_StdControl_init());

  return result;
}
