#ifndef _ECRIDA_LIB_H_
#define _ECRIDA_LIB_H_

#include <Arduino.h>
#include <TMC2130Stepper.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t sync;
    uint32_t message_id;
    uint8_t state;
    float temperatures[4];
    uint16_t uv_current[2];
    uint32_t crc;
} ECRIDA_telementry_t;
#pragma pack(pop)

#define BUILDPLATE_UP 0
#define BUILDPLATE_DOWN 1

#define BACKLIGHT_1 (BACKLIGHT_1_DIM)
#define BACKLIGHT_2 (BACKLIGHT_2_DIM)

#define MOTOR_STEPS_FULL_REVOLUTION (100)  // faulhaber motor
//#define MOTOR_STEPS_FULL_REVOLUTION (200)  // robofun motor

extern TMC2130Stepper TMC2130;

typedef enum {
    mA_100 = 4,
    mA_450 = 8,
    mA_800 = 13,
} ECRIDA_CURRENT;

void ECRIDA_EXP_setup_gpio();
void ECRIDA_EXP_motor_turn_on(ECRIDA_CURRENT current);  // 0-31
void ECRIDA_EXP_motor_turn_off();

void ECRIDA_EXP_lower_buildplate(double dist_mm, double rotationsPerSecond);
void ECRIDA_EXP_raise_buildplate(double dist_mm, double rotationsPerSecond);

void ECRIDA_EXP_UV_on(int pin, uint8_t pwm);
void ECRIDA_EXP_UV_off(int pin);

uint32_t checksum(uint8_t *data, uint32_t length);

#endif
