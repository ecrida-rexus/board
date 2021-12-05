#ifndef _ECRIDA_LIB_H_
#define _ECRIDA_LIB_H_

#define REQUIRESALARMS false

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <TMC2130Stepper.h>

enum EXPERIMENT_STATES {
    POWERED_ON,
    AWAITS_SODS,
    HOMING,
    AWAITS_LO,
    AWAITS_SOE,
    PRINTING,
    FINISHED,
    PARKED,
};

#pragma pack(push, 1)
typedef struct {
    uint16_t sync;
    uint32_t message_timestamp : 24;
    enum EXPERIMENT_STATES state : 4;
    uint8_t soe : 1;
    uint8_t sods : 1;
    uint8_t lo : 1;
    uint8_t uv_string_used : 1;
    int16_t temperatures[4];
    uint16_t uv_current;
    uint32_t crc;
} ECRIDA_telemetry_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t power_cycles;
    uint8_t exposed_layers;
    enum EXPERIMENT_STATES experiment_state : 4;
    bool sods : 1;
    bool lo : 1;
    bool soe : 1;
    uint8_t uv_string_used : 1;
    uint8_t soe_timeout;
    uint16_t eeprom_temperatures;
} ECRIDA_status_t;
#pragma pack(pop)

typedef struct {
    uint16_t timestamp;
    int16_t temperature[4];
} ECRIDA_EEPROM_temperature_t;

#define BUILDPLATE_UP 1
#define BUILDPLATE_DOWN 0

#define BACKLIGHT_1 (BACKLIGHT_1_DIM)
#define BACKLIGHT_2 (BACKLIGHT_2_DIM)

#define MOTOR_STEPS_FULL_REVOLUTION (200)

extern TMC2130Stepper TMC2130;
extern DallasTemperature vat_sensors;

extern volatile bool ECRIDA_UV_active;
extern volatile uint32_t steps;

typedef enum {
    mA_100 = 4,
    mA_450 = 8,
    mA_800 = 13,
} ECRIDA_CURRENT;

void ECRIDA_EXP_setup_gpio();

void ECRIDA_EXP_reset_datauc();

void ECRIDA_EXP_motor_turn_on(uint8_t current);  // 0-31
void ECRIDA_EXP_motor_turn_off();
void ECRIDA_EXP_lower_buildplate(double dist_mm, double rotationsPerSecond);
void ECRIDA_EXP_raise_buildplate(double dist_mm, double rotationsPerSecond);
void ECRIDA_EXP_wait_motor();

void ECRIDA_EXP_UV_on(int pin, uint16_t ms);

int16_t getTempByIndex(uint8_t deviceIndex);
uint32_t checksum(uint8_t* data, uint32_t length);

#endif
