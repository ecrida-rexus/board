#include "ECRIDA-lib-data.hpp"

void ECRIDA_DATA_setup_gpio() {
    DEBUG.begin(38400);
    INTER_UC.begin(38400);

    // setup on-board LEDs
    pinMode(DATA_BLUE, OUTPUT);
    pinMode(DATA_GREEN, OUTPUT);
    pinMode(DATA_RED, OUTPUT);

    accel.begin();
    SD.begin(10);
    pcb_temperature.begin();
}
