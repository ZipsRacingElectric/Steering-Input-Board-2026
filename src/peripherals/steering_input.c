// Includes 

#include "steering_input.h"
#include "hal_pal.h"
#include "peripherals/adc/stm_adc.h"

#include "debug.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define ADC_CHANNEL_10 10
#define ADC_CHANNEL_11 11
// Come from STM datasheet

#define UINT8_MAX_VALUE 255

static const sibConfig_t sibConfig =
{
    .driver = &CAND1,
    .timeout = TIME_MS2I(100),
    .baseId = 0x405,

    .trigger_id_t = {
        [TRIG_LT] = { .sampleMin = 700, .sampleMax = 4095, .valueMin = 0.0f, .valueMax = 100.0f },
        [TRIG_RT] = { .sampleMin = 900, .sampleMax = 4095, .valueMin = 0.0f, .valueMax = 100.0f },
    }
};

static sib_t sib = { &sibConfig };
static stmAdc_t steering_adc;
static linearSensor_t trigger_sensors[TRIG_COUNT];

// Functions Prototypes -------------------------------------------------------------------------------------------------------

static bool buttonInput(button_id_t btn_id);

static uint16_t triggerSample(trigger_id_t trig_id);

static float triggerValue(trigger_id_t trig_id);

// Steering input config ---------------------------------------------------------------------------------------------------------

const button_t buttons[BTN_COUNT] = 
{
    [BTN_LU] = {GPIOB, GPIOB_BTN_LU, PAL_MODE_INPUT_PULLUP, "Left Yellow Button"},
    [BTN_LD] = {GPIOB, GPIOB_BTN_LD, PAL_MODE_INPUT_PULLUP, "Left Red Button"},
    [BTN_RU] = {GPIOB, GPIOB_BTN_RU, PAL_MODE_INPUT_PULLUP, "Right Yellow Button"},
    [BTN_RD] = {GPIOB, GPIOB_BTN_RD, PAL_MODE_INPUT_PULLUP, "Right Red Button"},
    [BTN_LF] = {GPIOB, GPIOB_BTN_LF, PAL_MODE_INPUT_PULLUP, "Blue Button"},
    [BTN_R_UP] = {GPIOB, GPIOB_BTN_R_UP, PAL_MODE_INPUT_PULLUP, "Switch Up"},
    [BTN_R_DW] = {GPIOB, GPIOB_BTN_R_DW, PAL_MODE_INPUT_PULLUP, "Switch Down"},
};

const trigger_t triggers[TRIG_COUNT] = 
{
    [TRIG_LT] = {GPIOC, GPIOC_TRIG_LT, PAL_MODE_INPUT_ANALOG, "Left Trigger"},
    [TRIG_RT] = {GPIOC, GPIOC_TRIG_RT, PAL_MODE_INPUT_ANALOG, "Right Trigger"},
};

// ADC config -------------------------------------------------------------------------------------------------------------------

static const stmAdcConfig_t adc_config = 
{
    .driver = &ADCD1,
    .channels = { 
        ADC_CHANNEL_10, 
        ADC_CHANNEL_11,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    .sensors = {
        (analogSensor_t*)&trigger_sensors[0],
        (analogSensor_t*)&trigger_sensors[1],
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },
    .channelCount = 2
};

// Functions --------------------------------------------------------------------------------------------------------------------

bool steeringInputInit(void) 
{
    // Set sib config
        sib.config = &sibConfig;

    // Set buttons configs
    for (int i = 0; i < BTN_COUNT; i++) 
    {
        palSetPadMode(buttons[i].port, buttons[i].pad, buttons[i].mode);
    }

    // Set trigger GPIO and initialize linear sensors
    for (int i = 0; i < TRIG_COUNT; i++) 
    {
        palSetPadMode(triggers[i].port, triggers[i].pad, triggers[i].mode);

        if (!linearSensorInit(&trigger_sensors[i], &sibConfig.trigger_id_t[i]))
        {
            return false;
        }
    }

    // Initialize ADC
    if (!stmAdcInit(&steering_adc, &adc_config)) 
    {
        return false;
    }
    return true;
}

bool buttonInput(button_id_t btn_id) 
{
    return palReadPad(buttons[btn_id].port, buttons[btn_id].pad) == PAL_LOW;
}

// Get raw sampled value of triggers 
uint16_t triggerSample(trigger_id_t trig_id)
{
    if (trig_id >= TRIG_COUNT) {
        return 0;
    }
    else {
        return trigger_sensors[trig_id].sample;
    }
}

// Get sampled value of triggers with linear interpolation
float triggerValue(trigger_id_t trig_id)
{
    if (trig_id >= TRIG_COUNT) {
        return 0;
    }
    else {
        return trigger_sensors[trig_id].value;
    }
}


// Can Transmit Function ------------------------------------------------------------------------------------------------------

msg_t steeringInputTransmit(void) 
{
    uint8_t button_byte = 0;   // Start 00000000
    uint8_t right_percent = 0;
    uint8_t left_percent = 0;

    for (int i = 0; i < BTN_COUNT; i++) 
    {
        if (buttonInput(i)) {
            button_byte |= (1 << i); // Set bit to 1 for each button pressed
        }
    }

    if (stmAdcSample(&steering_adc)) {
        // Sample from paddles is 100 -> 0, subtract ADC value from 100 to flip
        left_percent = (uint8_t)((100.0f - triggerValue(TRIG_LT)) * UINT8_MAX_VALUE / 100.0f);
        right_percent = (uint8_t)((100.0f - triggerValue(TRIG_RT)) * UINT8_MAX_VALUE / 100.0f);
    }

    CANTxFrame transmit = 
    {
        .DLC = 3,
        .IDE = CAN_IDE_STD,
        .SID = sib.config->baseId,
        .data8 = {
            button_byte,
            left_percent,
            right_percent
        }
    };

    msg_t result = canTransmitTimeout(sib.config->driver, CAN_ANY_MAILBOX, &transmit, sib.config->timeout);

    return result;
}