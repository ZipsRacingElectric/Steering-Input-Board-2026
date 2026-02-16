#ifndef STEERING_INPUT_H
#define STEERING_INPUT_H

// Steering Wheel Buttons Input -------------------------------------------------------------------------------------------------
//
// Author: Jake Nowak
// Date Created: 2026.01.21
//
// Description: Take GPIO input from buttons and create a CAN output

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "hal.h"
#include "board.h"
#include "peripherals/adc/analog_linear.h"

typedef enum 
{
    BTN_LU = 0,
    BTN_LD,
    BTN_RU,
    BTN_RD,
    BTN_LF,
    BTN_R_UP,
    BTN_R_DW,
    BTN_COUNT
} button_id_t;

typedef enum
{
    TRIG_LT = 0,
    TRIG_RT,
    TRIG_COUNT
} trigger_id_t;

typedef struct 
{
    ioportid_t port;
    uint16_t pad;
    iomode_t mode;
    const char* name;
} button_t;

typedef struct 
{
    ioportid_t port;
    uint16_t pad;
    iomode_t mode;
    const char* name;
} trigger_t;

extern const button_t buttons[BTN_COUNT];
extern const trigger_t triggers[TRIG_COUNT];

typedef struct 
{
    CANDriver* driver;
    sysinterval_t timeout;
    uint16_t baseId;
    
    linearSensorConfig_t trigger_id_t[TRIG_COUNT];

} sibConfig_t;

typedef struct 
{
    const sibConfig_t* config;
} sib_t;

// Functions -------------------------------------------------------------------------------------------------------------------

/**
* @brief Initialize the steering wheel buttons and paddles inputs with the correct GPIO port and pad.
*
* @param sib The device to initailize.
* @param config The configuraton to use.
* @return Returns true if successful, false if not successful.
*/

bool steeringInputInit(void);

/**
 * @brief Take GPIO and ADC input and transmit a can message.
 * 
 * @param sib Contains pointer to can driver and transmit timeout.
 * @return MSG_OK if transmit is successful.
 */

msg_t steeringInputTransmit(void);

#endif // STEERING_INPUT_H