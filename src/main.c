// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"

// ChibiOS
#include "ch.h"
#include "hal.h"
#include "hal_can_lld.h"
#include "peripherals/steering_input.h"

// Entrypoint -----------------------------------------------------------------------------------------------------------------

int main (void)
{

	// ChibiOS Initialization
	halInit ();
	chSysInit ();

	// Debug Initialization
	ioline_t ledLine = LINE_LED_HEARTBEAT;
	debugHeartbeatStart (&ledLine, LOWPRIO);
	debugSerialInit (&SD1, NULL);

	// Assign Can GPIO pads
	palSetLineMode(GPIOA_CAN1_STBY, PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(GPIOA_CAN1_RX, PAL_MODE_ALTERNATE(9));
	palSetLineMode(GPIOA_CAN1_TX, PAL_MODE_ALTERNATE(9));
	
	// Start Can Driver
	static const CANConfig CAN_DRIVER_CONFIG =
	{
	.mcr = CAN_MCR_ABOM |
		CAN_MCR_AWUM |
		CAN_MCR_TXFP,
	.btr = CAN_BTR_SJW (0) |
		CAN_BTR_TS2 (1) |
		CAN_BTR_TS1 (10) |
		CAN_BTR_BRP (2)
	};
    
	canStart(&CAND1, &CAN_DRIVER_CONFIG);
	palClearLine (LINE_CAN1_STBY);

	steeringInputInit();

	// SIB can transmission (25hz transmission)
	while (true) 
	{
		steeringInputTransmit();
		chThdSleepMilliseconds(50);
	}

}

void hardFaultCallback (void)
{
	// Fault handler implementation
}