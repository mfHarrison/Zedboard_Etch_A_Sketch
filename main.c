#include "PmodOLEDrgb.h"
#include "PmodENC.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xgpio.h"
void Initialize();
void Run();
void Cleanup();
void EnableCaches();
void DisableCaches();

// Declare the devices used
PmodOLEDrgb oledrgb;
PmodENC myDevice;
XGpio btn;

int main(void) {
   // Run the necessary methods
   Initialize();
   Run();
   Cleanup();
   return 0;
}

/* Enables Caches and instantiate the devices:
 * Initializes Zedboard button XGpio driver and sets as input,
 * Begin drivers for OLED display and encoder
 */
void Initialize() {
   EnableCaches();
   XGpio_Initialize(&btn,XPAR_AXI_GPIO_1_DEVICE_ID);
   XGpio_SetDataDirection(&btn, 1, 0xF);
   OLEDrgb_begin(&oledrgb, XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR,
         XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR);
   ENC_begin(&myDevice, XPAR_PMODENC_0_AXI_LITE_GPIO_BASEADDR);
}

/* Main loop to run the Etch-a-Sketch:
 * Keeps track of encoder rotation and button state
 * to update coordinates where a pixel is to be drawn.
 *
 */
void Run() {
   // Store state and laststate of encoder to detect rotation
	u32 state, laststate;
   laststate = ENC_getState(&myDevice);

   // Set initial coordinates(center), declare button_data
   int x = 48;
   int y = 32;
   int button_data;

   while (1) {
      // Get current encoder and button state
      state = ENC_getState(&myDevice);
      button_data = XGpio_DiscreteRead(&btn, 1);

      // Modify x & y coordinates based on encoder and buttons
      if(button_data == 0b00000){
     	 // no button, move horizontally
    	 x += ENC_getRotation(state, laststate);
         if(x >= 95) x = 94;
         if(x <= 1) x = 2;
      }
      else if(button_data == 0b00001 && y ){
    	 // center button, move vertically
       	 y += ENC_getRotation(state, laststate);
         if(y >= 63) y = 62;
         if(y <= 1) y = 2;
      }
      else if(button_data == 0b01000){
    	 // top button, move diagonally up
     	 x += ENC_getRotation(state, laststate);
         if(x >= 95) x = 94;
         if(x <= 1) x = 2;
     	 y -= abs(ENC_getRotation(state, laststate));
       	 if(y <= 1) y = 2;
      }
      else if(button_data == 0b00100){
    	 // bottom button, move diagonally down
     	 x += ENC_getRotation(state, laststate);
         if(x >= 95) x = 94;
         if(x <= 1) x = 2;
     	 y += abs(ENC_getRotation(state, laststate));
     	 if(y >= 63) y = 62;
      }
      else if(button_data == 0b00010){
    	  // right button, clear screen and re-center
    	  x = 48;
    	  y = 32;
    	  OLEDrgb_Clear(&oledrgb);
      }

      // draw a pixel at x,y if the encoder switch is on
      if (ENC_switchOn(state)){
    	  OLEDrgb_DrawPixel(&oledrgb,x,y,OLEDrgb_BuildRGB(100,240,100));
      }
      // clear screen without moving cursor if encoder is pressed in
      if (ENC_buttonPressed(state) && !ENC_buttonPressed(laststate)) {
         OLEDrgb_Clear(&oledrgb);
      }
      laststate = state; // set laststate to this state before next iteration
      usleep(1000);
   }
}


void Cleanup() {
   DisableCaches();
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#endif
}
