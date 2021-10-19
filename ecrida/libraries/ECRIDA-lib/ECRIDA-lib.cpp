#include "ECRIDA-lib.hpp"

void ECRIDA_EXP_setup_gpio() {
    DEBUG.begin(115200);
    RXSM.begin(38400);
    INTER_UC.begin(38400);

    // setup on-board LEDs
    pinMode(EXP_BLUE, OUTPUT);
    pinMode(EXP_GREEN, OUTPUT);
    pinMode(EXP_RED, OUTPUT);

    // setup RXSM signals
    pinMode(LO, INPUT);
    pinMode(SODS, INPUT);
    pinMode(SOE, INPUT);

    // setup analog pins to measure UV current
    analogReference(INTERNAL1V1);
    pinMode(BACKLIGHT_1_SENSE, INPUT);
    pinMode(BACKLIGHT_2_SENSE, INPUT);

    // setup UV IC communication
    pinMode(BACKLIGHT_1_DIM, OUTPUT);
    pinMode(BACKLIGHT_2_DIM, OUTPUT);
    pinMode(BACKLIGHT_1_EN, OUTPUT);
    pinMode(BACKLIGHT_2_EN, OUTPUT);

    // power on UV drivers
    digitalWrite(BACKLIGHT_1_EN, HIGH);
    digitalWrite(BACKLIGHT_2_EN, HIGH);
    // turn off UV LEDs
    analogWrite(BACKLIGHT_1_DIM, 0);
    analogWrite(BACKLIGHT_2_DIM, 0);
}

volatile uint32_t steps;

ISR(TIMER1_COMPA_vect) {
    if (steps > 0) {
        PORTE |= 1 << 5;
        PORTE &= ~(1 << 5);

        steps--;
    }
}

void ECRIDA_EXP_motor_turn_on() {
    TMC2130.begin();
    TMC2130.vsense(false);
    TMC2130.irun(HOMING_CURRENT);
    TMC2130.ihold(HOMING_CURRENT);
    TMC2130.microsteps(256);
    TMC2130.en_pwm_mode(true);

    {
        cli();

        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1 = 0;
        OCR1A = 39;  // 1 000 000 / (MOTOR_STEPS_FULL_REVOLUTION steps/rotation * 256 microsteps)

        TCCR1B |= (1 << WGM12) | (1 << CS11);

        sei();
    }

    // MOTOR_DRV_EN is inverted
    digitalWrite(MOTOR_DRV_EN, LOW);
}

void ECRIDA_EXP_motor_turn_off() {
    // MOTOR_DRV_EN is inverted
    digitalWrite(MOTOR_DRV_EN, HIGH);
}

void move_motor(double dist_mm) {
    // one full screw revolution raises the buildplate by 1.5mm
    steps = dist_mm * 256 * MOTOR_STEPS_FULL_REVOLUTION / 1.5f;

    TIMSK1 |= (1 << OCIE1A);  // start timer
    while (steps > 0) {
        delay(10);
    }
    TIMSK1 &= ~(1 << OCIE1A);  // stop timer
}

void ECRIDA_EXP_lower_buildplate(double dist_mm) {
    TMC2130.shaft_dir(BUILDPLATE_DOWN);

    move_motor(dist_mm);
}

void ECRIDA_EXP_raise_buildplate(double dist_mm) {
    TMC2130.shaft_dir(BUILDPLATE_UP);

    move_motor(dist_mm);
}

void ECRIDA_EXP_UV_on(int pin) { analogWrite(pin, UV_POWER); }
void ECRIDA_EXP_UV_off(int pin) { analogWrite(pin, 0); }
