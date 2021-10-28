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

void ECRIDA_EXP_motor_turn_on(ECRIDA_CURRENT current) {
    TMC2130.begin();
    TMC2130.vsense(false);
    TMC2130.irun(current);
    TMC2130.ihold(current);
    TMC2130.microsteps(256);
    TMC2130.en_pwm_mode(true);

    {
        cli();

        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1 = 0;
        OCR1A = 8000000.0 / (MOTOR_STEPS_FULL_REVOLUTION * 256.0);

        TCCR1B |= (1 << WGM12) | (1 << CS10);

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
    steps = dist_mm * 256.0 * MOTOR_STEPS_FULL_REVOLUTION / 1.5f;

    TIMSK1 |= (1 << OCIE1A);  // start timer
    while (steps > 0) {
        delay(10);
    }
    TIMSK1 &= ~(1 << OCIE1A);  // stop timer
}

void ECRIDA_EXP_lower_buildplate(double dist_mm, double rotationsPerSecond) {
    TMC2130.shaft_dir(BUILDPLATE_DOWN);

    OCR1A = 8000000.0 / (rotationsPerSecond * MOTOR_STEPS_FULL_REVOLUTION * 256.0);

    move_motor(dist_mm);
}

void ECRIDA_EXP_raise_buildplate(double dist_mm, double rotationsPerSecond) {
    TMC2130.shaft_dir(BUILDPLATE_UP);

    OCR1A = 8000000.0 / (rotationsPerSecond * MOTOR_STEPS_FULL_REVOLUTION * 256.0);

    move_motor(dist_mm);
}

void ECRIDA_EXP_UV_on(int pin, uint8_t pwm) { analogWrite(pin, pwm); }
void ECRIDA_EXP_UV_off(int pin) { analogWrite(pin, 0); }

uint32_t checksum(uint8_t *data, uint32_t length) {
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; i++) {
        char ch = data[i];
        for (size_t j = 0; j < 8; j++) {
            uint32_t b = (ch ^ crc) & 1;
            crc >>= 1;
            if (b) crc = crc ^ 0xEDB88320;
            ch >>= 1;
        }
    }

    return ~crc;
}
