#ifndef WaterMqttSettingsService_h
#define WaterMqttSettingsService_h
#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <SettingValue.h>

#define Water_BROKER_SETTINGS_FILE "/config/brokerSettings.json"
#define Water_BROKER_SETTINGS_PATH "/rest/brokerSettings"

class WaterMqttSettings
{
public:
    String mqttPath;
    String name;
    String uniqueId;

    static void read(WaterMqttSettings &settings, JsonObject &root)
    {
        root["mqtt_path"] = settings.mqttPath;
        root["name"] = settings.name;
        root["unique_id"] = settings.uniqueId;
    }

    static StateUpdateResult update(JsonObject &root, WaterMqttSettings &settings)
    {
        settings.mqttPath = root["mqtt_path"] | SettingValue::format("homeassistant/Water/#{unique_id}");
        settings.name = root["name"] | SettingValue::format("Water-#{unique_id}");
        settings.uniqueId = root["unique_id"] | SettingValue::format("Water-#{unique_id}");
        return StateUpdateResult::CHANGED;
    }
};

class WaterMqttSettingsService : public StatefulService<WaterMqttSettings>
{
public:
    WaterMqttSettingsService(PsychicHttpServer *server, FS *fs, SecurityManager *securityManager);
    void begin();

private:
    HttpEndpoint<WaterMqttSettings> _httpEndpoint;
    FSPersistence<WaterMqttSettings> _fsPersistence;
};

#endif
