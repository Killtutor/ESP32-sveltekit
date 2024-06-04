
#include <ESP32SvelteKit.h>
#include <WaterSupplyMQTTService.h>
#include <WaterSupplyService.h>
#include <PsychicHttpServer.h>

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

WaterMqttSettingsService waterMqttSettingsService = WaterMqttSettingsService(&server,
                                                                             esp32sveltekit.getFS(),
                                                                             esp32sveltekit.getSecurityManager());

WaterStateService waterStateService = WaterStateService(&server,
                                                        esp32sveltekit.getSocket(),
                                                        esp32sveltekit.getSecurityManager(),
                                                        esp32sveltekit.getMqttClient(),
                                                        &waterMqttSettingsService);

void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    // load the initial Water settings
    waterStateService.begin();
    // start the Water service
    waterMqttSettingsService.begin();
}

void loop()
{

    bool sensor = digitalRead(WATER_IN_SENSOR);
    waterStateService.update([&](WaterState &state)
                             {
                                 if (state.waterIn == (sensor == 1 ? false : true))
                                 {
                                     return StateUpdateResult::UNCHANGED; // water is unchanged
                                 }
                                 state.waterIn = !state.waterIn;    // Set the water boolean value
                                 return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
                             },
                             "Sensor");

    digitalWrite(WATER_LEVEL_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(WATER_LEVEL_TRIG, HIGH);
    delayMicroseconds(210);
    digitalWrite(WATER_LEVEL_TRIG, LOW);
    long duration = pulseInLong(WATER_LEVEL_ECHO, HIGH);
    long distance = (duration / 29) / 2;
    Serial.print("Distance: ");
    Serial.println(distance);

    sleep(2);

    // Delete Arduino loop task, as it is not needed in this example
    // vTaskDelete(NULL);
}
