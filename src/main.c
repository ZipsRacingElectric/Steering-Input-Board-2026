// Includes -------------------------------------------------------------------------------------------------------------------

#include "debug.h"

// ChibiOS
#include "ch.h"
#include "hal.h"
// REVIEW(Barach): You shouldn't ever need to include low-level driver files directly, as they are meant to be internal to
// ChibiOS. Looks like it isn't doing anything here.
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
	// REVIEW(Barach): This is already done in board.chcfg, so no need.
	palSetLineMode(GPIOA_CAN1_STBY, PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(GPIOA_CAN1_RX, PAL_MODE_ALTERNATE(9));
	palSetLineMode(GPIOA_CAN1_TX, PAL_MODE_ALTERNATE(9));
	
	// REVIEW(Barach): Nothing wrong with this, but usually I'd put this at the top-level scope (before main) just to be a bit cleaner.
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

	// Steering Input Board 
	steeringInputInit();

	// SIB can transmission (25hz transmission)
	while (true) 
	{
		steeringInputTransmit();

		// REVIEW(Barach): Might be a good idea to make this a constant (ex. #define CAN_TX_PERIOD or something like that) so
		// we can tweak this pretty easily.
		chThdSleepMilliseconds(50);
	}

}

void hardFaultCallback (void)
{
	// Fault handler implementation
}