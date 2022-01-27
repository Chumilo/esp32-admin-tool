#include <WiFi.h>
#include <DNSServer.h>
#include <ESPmDNS.h>

const byte DNSSERVER_PORT = 53;
DNSServer dnsServer;

IPAddress apIP(192,168,4,1);
IPAddress netMsk(255,255,255,0);

int wifi_mode = WIFI_STA;

unsigned long previousMillisWIFI = 0;
unsigned long intervalWIFI = 30000;

// hostname for ESPmDNS. Should work at least on windows. Try http://adminesp.local
const char *esp_hostname = id;

void startAP(){
    log("Info: Iniciando modo AP");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.softAPConfig(apIP,apIP,netMsk);
    WiFi.setHostname(deviceID().c_str());
    WiFi.softAP(ap_nameap,ap_passwordap,ap_canalap,ap_hiddenap,ap_connetap);
    log("Info: Wifi AP" + deviceID() + "-IP" + ipStr(WiFi.softAPIP()));
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNSSERVER_PORT,"*",apIP);
    wifi_mode = WIFI_AP;
} 
// ---------------------------------------------------------
// Start Client, attempt to connect to wifi network
// ---------------------------------------------------------
void startClient(){
    if(wifi_staticIP){
        if(WiFi.config(CharToIP(wifi_ip_static),CharToIP(wifi_gateway),CharToIP(wifi_subnet),CharToIP(wifi_primaryDNS),CharToIP(wifi_secondaryDNS))){
            log("Error: Falló al conectar el modo estación.");
         }
    }
    WiFi.hostname(deviceID());
    WiFi.begin(wifi_ssid,wifi_passw);
    log("Info: Conectado wifi" + String(wifi_ssid));
    delay(50);
    byte b = 0;
    while(WiFi.status() != WL_CONNECTED && b <60){
        b++;
        log("Warning: Intetando conexión wifi...");
        delay(500);
        // Para parpadear led wifi cuando esta conectandose a wifi no bloquante
        // Parpadeo simple del led cada 100 ms
        blinkSingle(100,WIFILED);
    }
    if(WiFi.status() == WL_CONNECTED){
        // Wifi station conectado
        log("Info: wifi conectado (" + String(WiFi.RSSI()) + ") IP" + ipStr(WiFi.localIP()));
        // Parpadeo ramdon del led
        blinkRandomSingle(10,100,WIFILED);
        delay(100);
    }else{
        // Wifi station no conectado
        log(F("Error: wifi no conectado"));
        // Parpadeo ramdon del led
        blinkRandomSingle(10,100,WIFILED);
        delay(100);
    }
}

// Wifi.mode(WIFI_STA)      - station mode: the ESP32 connects to an access point
// Wifi.mode(WIFI_AP)       - access point mode: stations can connect to the ESP32
// Wifi.mode(WIFI_AP_STA)   - access point and a statiom connected to another access point

void wifi_setup(){
    WiFi.disconnect();
    // 1) Si esta activo en modo AP
    if(ap_accessPoint){
        startAP();
        log("Info: Wifi modo AP");
    }
    // 2) Caso contrario en modo cliente
    else{
        WiFi.disconnect(WIFI_STA);
        wifi_mode = WIFI_STA;
        startClient();
        log("Info: Wifi modo estación");
    }

    // Iniciar hostname broadcast en modo STA o AP
    if(wifi_mode == WIFI_STA || wifi_mode == WIFI_AP){
        if(MDNS.begin(esp_hostname)){
            MDNS.addService("http","tcp",80);
        }
    }

}
// ---------------------------------------------------------
// Loop modo cliente
// ---------------------------------------------------------
void wifiLoop(){

    unsigned long currentMillis = millis();

    if(WiFi.status() != WL_CONNECTED && (currentMillis - previousMillisWIFI >= intervalWIFI)){
        // para papadear un led cuando esta conectandose al wifi no bloqueante
        // Parpadeo Simple del Led cada 100 ms
        blinkSingle(100,WIFILED);

        WiFi.disconnect();
        WiFi.reconnect();
        previousMillisWIFI = currentMillis;
    }else{
        // parpadeo del Led Tiempo on y Tiempo off
        blinkSingleAsy(10,500,WIFILED);
    }
}
// ---------------------------------------------------------
// Loop modo AP
// ---------------------------------------------------------
void wifiAPLoop(){
    // Parpadeo del Led con tiempo variable como transferencia de datos
    blinkRandomSingle(50,100,WIFILED);
    dnsServer.processNextRequest(); // Captive portal DNS re-dierct
}