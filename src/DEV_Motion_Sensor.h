#include "HomeSpan.h"

#define PIR_motion_sensor 13  // PIR sensor connected to GPIO13

struct DEV_Motion_Sensor : Service::MotionSensor {
    SpanCharacteristic *motion;      // Motion detection state
    SpanCharacteristic *active;      // Sensor active/inactive state
    SpanCharacteristic *status_fault;  // Sensor fault status
    
    DEV_Motion_Sensor() : Service::MotionSensor() {
        motion = new Characteristic::MotionDetected(false);  
        active = new Characteristic::StatusActive(true);     
        status_fault = new Characteristic::StatusFault(0);   // 0 = no fault
        
        // Initialize PIR pin
        pinMode(PIR_motion_sensor, INPUT);
    }

    void loop() {
        // Only update if enough time has passed (debounce)
        static unsigned long lastUpdate = 0;
        if (millis() - lastUpdate < 2000) return;  // 2 second debounce
        
        // Read sensor with error checking
        bool currentState = false;
        int readAttempts = 0;
        int maxAttempts = 3;
        
        // Take multiple readings to ensure stability
        while (readAttempts < maxAttempts) {
            if (digitalRead(PIR_motion_sensor) == HIGH) {
                currentState = true;
                break;
            }
            delay(50);
            readAttempts++;
        }

        // Update motion state if changed
        if (motion->getVal() != currentState) {
            motion->setVal(currentState);
            lastUpdate = millis();
        }

        // Update fault status if sensor seems stuck
        static unsigned long lastStateChange = 0;
        if (currentState == motion->getVal()) {
            if (millis() - lastStateChange > 3600000) { // 1 hour
                status_fault->setVal(1);  // Possible sensor fault
            }
        } else {
            lastStateChange = millis();
            status_fault->setVal(0);
        }
    }
}; 