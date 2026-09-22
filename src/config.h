// ============================================================
// SD2 PrintSphere Lite - ESP8266 Bambu Cloud MQTT display
// ============================================================

#ifndef CONFIG_H
#define CONFIG_H

#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

#define ESP_CONFIG_PORT 8081

#define MQTT_PORT 8883
#define MQTT_BUFFER_SIZE 12288
#define MQTT_RECONNECT_INTERVAL 30000
#define MQTT_REQUEST_INTERVAL 15000

#define DISPLAY_REFRESH 350

// --- Display colors (RGB565) ---
#define BG_BLACK  0x0000
#define C_TEXT    0xFFFF
#define C_DIM     0x8410
#define C_CYAN    0x07FF
#define C_ORANGE  0xFD20
#define C_BLUE    0x5D1F
#define C_RED     0xF800
#define C_RING    0x07E0
#define C_TRACK   0x2104
#define C_PANEL   0x3B6D
#define C_PANEL2  0x2A6B
#define C_PANEL3  0x4C10

#endif
