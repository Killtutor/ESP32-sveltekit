
#include <WaterSupplyService.h>

WaterStateService::WaterStateService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager,
                                     PsychicMqttClient *mqttClient,
                                     WaterMqttSettingsService *WaterMqttSettingsService) : _httpEndpoint(WaterState::read,
                                                                                                         WaterState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         Water_SETTINGS_ENDPOINT_PATH,
                                                                                                         securityManager,
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(WaterState::read,
                                                                                                          WaterState::update,
                                                                                                          this,
                                                                                                          socket,
                                                                                                          Water_SETTINGS_EVENT),
                                                                                           _mqttEndpoint(WaterState::homeAssistRead,
                                                                                                         WaterState::homeAssistUpdate,
                                                                                                         this,
                                                                                                         mqttClient),
                                                                                           _webSocketServer(WaterState::read,
                                                                                                            WaterState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            Water_SETTINGS_SOCKET_PATH,
                                                                                                            securityManager,
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _mqttClient(mqttClient),
                                                                                           _WaterMqttSettingsService(WaterMqttSettingsService)
{
    // configure Water Input Sensor to be PullUp Input (1 == false, 0 == true)
    pinMode(WATER_IN_SENSOR, INPUT_PULLUP);
    pinMode(WATER_LEVEL_TRIG, OUTPUT);
    pinMode(WATER_LEVEL_ECHO, INPUT);

    // configure MQTT callback
    _mqttClient->onConnect(std::bind(&WaterStateService::registerConfig, this));

    // configure update handler for when the Water settings change
    _WaterMqttSettingsService->addUpdateHandler([&](const String &originId)
                                                { registerConfig(); },
                                                false);

    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void WaterStateService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _state.waterIn = DEFAULT_WATER_STATE;
    _state.waterLevel = 10;
    onConfigUpdated();
}

void WaterStateService::onConfigUpdated()
{
    // digitalWrite(LED_BUILTIN, _state.ledOn ? 1 : 0);
}

void WaterStateService::registerConfig()
{
    if (!_mqttClient->connected())
    {
        return;
    }
    String configTopic;
    String subTopic;
    String pubTopic;

    JsonDocument doc;
    _WaterMqttSettingsService->read([&](WaterMqttSettings &settings)
                                    {
    configTopic = settings.mqttPath + "/config";
    subTopic = settings.mqttPath + "/set";
    pubTopic = settings.mqttPath + "/state";
    doc["~"] = settings.mqttPath;
    doc["name"] = settings.name;
    doc["unique_id"] = settings.uniqueId; });
    doc["cmd_t"] = "~/set";
    doc["stat_t"] = "~/state";
    doc["schema"] = "json";
    doc["brightness"] = false;

    String payload;
    serializeJson(doc, payload);
    _mqttClient->publish(configTopic.c_str(), 0, false, payload.c_str());

    _mqttEndpoint.configureTopics(pubTopic, subTopic);
}
