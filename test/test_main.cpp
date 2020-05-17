#include <unity.h>
#include <Arduino.h>

String STR_TO_TEST;

void setUp() {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();
    UNITY_END(); // stop unit testing
}

void loop()
{
}