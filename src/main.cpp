// ---------------------------------------------------------
// Librerias
// ---------------------------------------------------------
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

// ---------------------------------------------------------
// Archivos *.hpp - Fragmentar el código
// ---------------------------------------------------------
#include "settings.hpp"
#include "functions.hpp"
#include "settingsReset.hpp"
#include "settingsRead.hpp"
#include "settingsSave.hpp"
#include "esp32_wifi.hpp"

void setup() {
  // baudrate
  Serial.begin(115200);
  // CPU Freq
  setCpuFrequencyMhz(240);
  // Inicio del Log por serial
  log("\nInfo: Inicio Setup");
  // Configurar los Pines
  settingPines();
  // Inicio SPIFFS
  if(!SPIFFS.begin(true)){
    log(F("Error: Falló la inicialización del SPIFFS"));
    while(true);
  }
  // Lee la configuración Wifi
  settingsReadWifi();
  // Configuración Wifi
  WiFi.disconnect(true);
  delay(1000);
  // Setup del wifi
  wifi_setup();
}
// ---------------------------------------------------------
// Loop principal nucleo 0
// ---------------------------------------------------------
void loop() {
  yield();
  // ---------------------------------------------------------
  // WIFI
  // ---------------------------------------------------------
  if(wifi_mode == WIFI_STA){
    wifiLoop();
  }
  if(wifi_mode == WIFI_AP){
    wifiAPLoop();
  }
}