#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- Function prototypes (needed in PlatformIO .cpp) ----------
void showWelcome();
void showOverview();
void showParkingMap();
void showTemperatureGraph();
void updateDemoData();
void updateGraph();
void printTenths(int v);
int    mapInt(long v, long inLo, long inHi, long outLo, long outHi);

// ============================================================
// DEMO PARKING DATA
// ============================================================

int availableSlots = 2;

bool slotStatus[4] = {
    false,     // S1 vacant
    true,        // S2 occupied
    false,     // S3 vacant
    true         // S4 occupied
};

bool gateOpen = false;

// ============================================================
// DEMO TEMPERATURE / HUMIDITY    (stored as TENTHS, integer only)
// 280 = 28.0 C, 650 = 65.0 %
// ============================================================

int temperature = 280;
int humidity = 650;

int tempChange = 3;
int humidityChange = -5;

unsigned long demoTimer = 0;
const unsigned long demoInterval = 2000;

// ============================================================
// SCREEN CONTROL
// ============================================================

int currentScreen = 0;

unsigned long screenTimer = 0;
const unsigned long screenInterval = 4000;

// ============================================================
// GRAPH DATA
// ============================================================

#define GRAPH_POINTS 100
#define GRAPH_SHOWN    50

int16_t tempGraph[GRAPH_POINTS];
int16_t humidityGraph[GRAPH_POINTS];

unsigned long graphTimer = 0;
const unsigned long graphInterval = 1000;

// ============================================================
// HELPERS
// ============================================================

// Print a value given in tenths with one decimal, e.g. 285 -> "28.5"
void printTenths(int v) {
    if (v < 0) {
        display.print('-');
        v = -v;
    }
    display.print(v / 10);
    display.print('.');
    display.print(v % 10);
}

int mapInt(long v, long inLo, long inHi, long outLo, long outHi) {
    if (v < inLo) v = inLo;
    if (v > inHi) v = inHi;
    return (int)((v - inLo) * (outHi - outLo) / (inHi - inLo) + outLo);
}

// ============================================================
// SETUP
// ============================================================

void setup() {

    Wire.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (1);
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    for (int i = 0; i < GRAPH_POINTS; i++) {
        tempGraph[i] = temperature;
        humidityGraph[i] = humidity;
    }

    showWelcome();
    delay(3000);

    display.clearDisplay();
    display.display();

    screenTimer = millis();
    graphTimer = millis();
    demoTimer = millis();
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

    unsigned long currentMillis = millis();

    if (currentMillis - demoTimer >= demoInterval) {
        demoTimer = currentMillis;
        updateDemoData();
    }

    if (currentMillis - graphTimer >= graphInterval) {
        graphTimer = currentMillis;
        updateGraph();
    }

    if (currentMillis - screenTimer >= screenInterval) {
        screenTimer = currentMillis;
        currentScreen++;
        if (currentScreen > 2) {
            currentScreen = 0;
        }
    }

    display.clearDisplay();

    switch (currentScreen) {
        case 0: showOverview(); break;
        case 1: showParkingMap(); break;
        case 2: showTemperatureGraph(); break;
    }

    display.display();
}

// ============================================================
// WELCOME SCREEN
// ============================================================

void showWelcome() {

    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);

    display.setCursor(5, 10);
    display.println("SMART");

    display.setCursor(5, 30);
    display.println("PARKING");

    display.display();
}

// ============================================================
// OVERVIEW
// ============================================================

void showOverview() {

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(35, 2);
    display.println("SMART PARKING");

    display.setCursor(5, 22);
    display.print("VACANT");

    display.setTextSize(2);
    display.setCursor(60, 18);
    display.print(availableSlots);
    display.print("/4");

    display.setTextSize(1);

    display.setCursor(5, 38);
    display.print("GATE");

    display.setCursor(60, 38);
    display.print(gateOpen ? "OPEN" : "CLOSED");

    display.setCursor(5, 53);
    display.print("T:");
    printTenths(temperature);
    display.print("C");

    display.setCursor(76, 53);
    display.print("H:");
    display.print((humidity + 5) / 10);
    display.print("%");
}

// ============================================================
// PARKING MAP
// ============================================================

void drawSlot(int x, int y, const char *label, bool occupied) {
    display.drawRect(x, y, 58, 13, SSD1306_WHITE);
    display.setCursor(x + 4, y + 3);
    display.print(label);
    if (occupied) {
        display.fillRect(x + 31, y + 2, 23, 9, SSD1306_WHITE);
    }
}

void showParkingMap() {

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(40, 2);
    display.println("PARKING MAP");

    drawSlot(3, 18, "S1", slotStatus[0]);
    drawSlot(67, 18, "S2", slotStatus[1]);
    drawSlot(3, 34, "S3", slotStatus[2]);
    drawSlot(67, 34, "S4", slotStatus[3]);

    display.setCursor(2, 53);
    display.print("V:");
    display.print(availableSlots);
    display.print("/4");

    display.setCursor(45, 53);
    display.print("T:");
    display.print((temperature + 5) / 10);
    display.print("C");

    display.setCursor(87, 53);
    display.print("H:");
    display.print((humidity + 5) / 10);
    display.print("%");
}

// ============================================================
// TEMPERATURE + HUMIDITY GRAPH
// ============================================================

void showTemperatureGraph() {

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(22, 2);
    display.println("TEMP / HUMIDITY");

    display.setCursor(5, 17);
    display.print("TEMP:");
    display.setCursor(38, 17);
    printTenths(temperature);
    display.print("C");

    display.setCursor(69, 17);
    display.print("HUM:");
    display.setCursor(98, 17);
    display.print((humidity + 5) / 10);
    display.print("%");

    display.drawRect(3, 27, 58, 23, SSD1306_WHITE);
    display.drawRect(67, 27, 58, 23, SSD1306_WHITE);

    const int base = GRAPH_POINTS - GRAPH_SHOWN;     // show the newest samples

    // Temperature graph (25.0 - 32.0 C)
    for (int x = 1; x < GRAPH_SHOWN; x++) {
        int x1 = 7 + x;
        int x2 = 8 + x;
        if (x2 >= 59) break;

        int y1 = 48 - mapInt(tempGraph[base + x - 1], 250, 320, 0, 18);
        int y2 = 48 - mapInt(tempGraph[base + x], 250, 320, 0, 18);

        display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
    }

    // Humidity graph (55 - 75 %), dotted
    for (int x = 1; x < GRAPH_SHOWN; x++) {
        int x1 = 71 + x;
        int x2 = 72 + x;
        if (x2 >= 123) break;

        int y1 = 48 - mapInt(humidityGraph[base + x - 1], 550, 750, 0, 18);
        int y2 = 48 - mapInt(humidityGraph[base + x], 550, 750, 0, 18);

        if (x % 2 == 0) {
            display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
        }
    }

    display.setCursor(37, 54);
    display.print("VACANT:");
    display.print(availableSlots);
    display.print("/4");
}

// ============================================================
// UPDATE DEMO DATA
// ============================================================

void updateDemoData() {

    temperature += tempChange;

    if (temperature >= 320) {
        temperature = 320;
        tempChange = -3;
    }
    if (temperature <= 250) {
        temperature = 250;
        tempChange = 3;
    }

    humidity += humidityChange;

    if (humidity >= 750) {
        humidity = 750;
        humidityChange = -5;
    }
    if (humidity <= 550) {
        humidity = 550;
        humidityChange = 5;
    }
}

// ============================================================
// UPDATE GRAPH DATA
// ============================================================

void updateGraph() {

    for (int i = 0; i < GRAPH_POINTS - 1; i++) {
        tempGraph[i] = tempGraph[i + 1];
        humidityGraph[i] = humidityGraph[i + 1];
    }

    tempGraph[GRAPH_POINTS - 1] = temperature;
    humidityGraph[GRAPH_POINTS - 1] = humidity;
}