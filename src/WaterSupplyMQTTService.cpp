
#include <WaterSupplyMQTTService.h>

WaterMqttSettingsService::WaterMqttSettingsService(PsychicHttpServer *server,
                                                   FS *fs,
                                                   SecurityManager *securityManager) : _httpEndpoint(WaterMqttSettings::read,
                                                                                                     WaterMqttSettings::update,
                                                                                                     this,
                                                                                                     server,
                                                                                                     Water_BROKER_SETTINGS_PATH,
                                                                                                     securityManager,
                                                                                                     AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                       _fsPersistence(WaterMqttSettings::read,
                                                                                                      WaterMqttSettings::update,
                                                                                                      this,
                                                                                                      fs,
                                                                                                      Water_BROKER_SETTINGS_FILE)
{
}

void WaterMqttSettingsService::begin()
{
    _httpEndpoint.begin();
    _fsPersistence.readFromFS();
}
