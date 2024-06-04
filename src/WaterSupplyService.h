#ifndef WaterStateService_h
#define WaterStateService_h
#include <WaterSupplyMQTTService.h>

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <MqttEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>

#define DEFAULT_WATER_STATE false
#define DEFAULT_TANK_LENGTH 4
#define OFF_STATE "OFF"
#define ON_STATE "ON"

#define Water_SETTINGS_ENDPOINT_PATH "/rest/waterState"
#define Water_SETTINGS_SOCKET_PATH "/ws/waterState"
#define Water_SETTINGS_EVENT "water"
#define WATER_IN_SENSOR 17
#define WATER_LEVEL_TRIG 15
#define WATER_LEVEL_ECHO 16

class WaterState
{
public:
    bool waterIn;
    int16_t waterLevel;
    int8_t tankSize;

    static void read(WaterState &settings, JsonObject &root)
    {
        root["waterIn"] = settings.waterIn;
        root["waterLevel"] = settings.waterLevel;
        root["tankSize"] = settings.tankSize;
    }

    static StateUpdateResult update(JsonObject &root, WaterState &WaterState)
    {
        boolean newState = root["waterIn"] | DEFAULT_WATER_STATE;
        boolean newState = root["waterLevel"] | 10;
        boolean newState = root["tankSize"] | DEFAULT_TANK_LENGTH;
        if (WaterState.waterIn != newState)
        {
            WaterState.waterIn = newState;
            return StateUpdateResult::CHANGED;
        }
        return StateUpdateResult::UNCHANGED;
    }

    static void homeAssistRead(WaterState &settings, JsonObject &root)
    {
        root["state"] = settings.waterIn ? ON_STATE : OFF_STATE;
    }

    static StateUpdateResult homeAssistUpdate(JsonObject &root, WaterState &WaterState)
    {
        // String state = root["state"];
        // // parse new led state
        // boolean newState = false;
        // if (state.equals(ON_STATE))
        // {
        //     newState = true;
        // }
        // else if (!state.equals(OFF_STATE))
        // {
        //     return StateUpdateResult::ERROR;
        // }
        // // change the new state, if required
        // if (WaterState.waterIn != newState)
        // {
        //     WaterState.waterIn = newState;
        //     return StateUpdateResult::CHANGED;
        // }
        return StateUpdateResult::UNCHANGED;
    }
};

class WaterStateService : public StatefulService<WaterState>
{
public:
    WaterStateService(PsychicHttpServer *server,
                      EventSocket *socket,
                      SecurityManager *securityManager,
                      PsychicMqttClient *mqttClient,
                      WaterMqttSettingsService *WaterMqttSettingsService);

    void begin();

private:
    HttpEndpoint<WaterState> _httpEndpoint;
    EventEndpoint<WaterState> _eventEndpoint;
    MqttEndpoint<WaterState> _mqttEndpoint;
    WebSocketServer<WaterState> _webSocketServer;
    PsychicMqttClient *_mqttClient;
    WaterMqttSettingsService *_WaterMqttSettingsService;

    void registerConfig();
    void onConfigUpdated();
};

#endif
