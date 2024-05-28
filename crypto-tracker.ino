#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define upLED 19

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "S9+";
const char* password = "xyz12345.";

const int httpsPort = 443;
const String url = "http://api.coindesk.com/v1/bpi/currentprice/BTC.json";
const String ethUrl = "https://api.etherscan.io/api?module=stats&action=ethprice&apikey=89SD4YRTF3P7SXZ36YDHWTE5QUEU3653CS";
const String historyURL = "http://api.coindesk.com/v1/bpi/historical/close.json";
const String cryptoCode = "BTC";
const String cryptoCode2 = "ETH";

const int trigPin = 5;
const int echoPin = 18;
long duration;
float distanceCm;
float distanceInch;

WiFiClient client;
HTTPClient http;
HTTPClient http2;

String formattedDate;
String dayStamp;
String timeStamp;

const unsigned char bitcoinLogo[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x03, 0xe7, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x03, 0xe7, 0xc0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x02, 0x24, 0x40, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x02, 0x24, 0x40, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x02, 0x24, 0x40, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x24, 0x40, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x02, 0x3c, 0x40, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x07, 0x03, 0xfe, 0x3c, 0x7c, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x1f, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x03, 0x00, 0x00, 0x01, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x03, 0xf0, 0x3f, 0x80, 0x60, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0xf0, 0x3f, 0xe0, 0x20, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x18, 0x30, 0x70, 0x30, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x30, 0x10, 0x30, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x30, 0x10, 0x30, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x10, 0x30, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x30, 0x20, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0xe0, 0x60, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x3f, 0xc0, 0xc0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x00, 0x01, 0xc0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x00, 0x00, 0xf0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x00, 0x00, 0x38, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x3f, 0xe0, 0x18, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x30, 0xf8, 0x0c, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x1c, 0x0c, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x0c, 0x0c, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x04, 0x0c, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x30, 0x0c, 0x0c, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x30, 0x0c, 0x0c, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x18, 0x30, 0x38, 0x0c, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x01, 0xf0, 0x3f, 0xf0, 0x08, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x01, 0xe0, 0x1f, 0x00, 0x18, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x01, 0x00, 0x00, 0x00, 0x30, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x03, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x0f, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x07, 0x03, 0xfe, 0x3c, 0x7e, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x02, 0x3c, 0x40, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x24, 0x40, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x02, 0x24, 0x40, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x02, 0x24, 0x40, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x02, 0x24, 0x40, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x03, 0xe7, 0xc0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x03, 0xe7, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  Serial.begin(115200);

  pinMode(upLED, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.drawBitmap(0, 0, bitcoinLogo, 128, 64, WHITE);
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  display.println("Connected to: ");
  display.print(ssid);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.display();
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  distanceInch = distanceCm * CM_TO_INCH;
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  delay(1000);

  if (distanceCm > 50) {
    Serial.print("Connecting to ");
    Serial.println(url);

    http.begin(url);
    int httpCode = http.GET();
    StaticJsonDocument<2000> doc;
    DeserializationError error = deserializeJson(doc, http.getString());

    if (error) {
      Serial.print(F("deserializeJson Failed"));
      Serial.println(error.f_str());
      delay(2500);
      return;
    }

    Serial.print("HTTP Status Code1: ");
    Serial.println(httpCode);

    String BTCUSDPrice = doc["bpi"]["USD"]["rate_float"].as<String>();
    double btcdouble = BTCUSDPrice.toDouble();
    String formattedBTC = String(btcdouble, 1);
    String lastUpdated = doc["time"]["updated"].as<String>();
    http.end();


    Serial.print("Getting history...");
    StaticJsonDocument<2000> historyDoc;

    http.begin(historyURL);
    int historyHttpCode = http.GET();
    DeserializationError historyError = deserializeJson(historyDoc, http.getString());

    if (historyError) {
      Serial.print(F("deserializeJson(History) failed"));
      Serial.println(historyError.f_str());
      delay(2500);
      return;
    }

    Serial.print("History HTTP Status Code: ");
    Serial.println(historyHttpCode);
    JsonObject bpi = historyDoc["bpi"].as<JsonObject>();
    double yesterdayPrice;
    for (JsonPair kv : bpi) {
      yesterdayPrice = kv.value().as<double>();
    }

    Serial.print("BTCUSD Price: ");
    Serial.println(formattedBTC);

    Serial.print("Yesterday's Price: ");
    Serial.println(yesterdayPrice);

    bool isUp = BTCUSDPrice.toDouble() > yesterdayPrice;
    double percentChange;
    String dayChangeString = "24hr Change:";
    if (isUp) {
      percentChange = ((BTCUSDPrice.toDouble() - yesterdayPrice) / yesterdayPrice) * 100;
      digitalWrite(upLED, HIGH);
    }

    Serial.print("Percent Change: ");
    Serial.println(percentChange);

    display.clearDisplay();
    display.setTextSize(1);
    printCenter("BTC/USD", 0, 0);

    display.setTextSize(2);
    printCenter("$" + formattedBTC, 0, 20);

    display.setTextSize(1);
    dayChangeString = dayChangeString + percentChange + "%";
    printCenter(dayChangeString, 0, 55);
    display.display();
    http.end();
    delay(2000);
  }


  // eth
  if (distanceCm < 50) {
    Serial.print("Connecting to ");
    Serial.println(ethUrl);

    http2.begin(ethUrl);
    http2.setTimeout(15000);
    int httpCode2 = http2.GET();

    if (httpCode2 > 0) {
      // HTTP header has been sent and Server response header has been handled
      Serial.print("HTTP Status Code2: ");
      Serial.println(httpCode2);

      // Get the response payload
      String payload = http2.getString();
      Serial.print("Response: ");
      Serial.println(payload);

      StaticJsonDocument<2000> doc2;
      DeserializationError error2 = deserializeJson(doc2, payload);

      if (error2) {
        Serial.print(F("deserializeJson Failed: "));
        Serial.println(error2.f_str());
        delay(2500);
        http2.end();
        return;
      }

      String ETHUSDPrice = doc2["result"]["ethusd"].as<String>();
      double ethdouble = ETHUSDPrice.toDouble();
      String formattedPrice = String(ethdouble, 1);
      Serial.print("ETHUSD Price: ");
      Serial.println(formattedPrice);

      display.clearDisplay();
      display.setTextSize(1);
      printCenter("ETH/USD", 0, 0);

      display.setTextSize(2);
      printCenter("$" + formattedPrice, 0, 20);

      display.display();
      delay(2000);
    } else {
      Serial.print("HTTP GET request failed, error: ");
      Serial.println(http2.errorToString(httpCode2).c_str());
    }

    http2.end();
  }


  esp_sleep_enable_timer_wakeup(900000000);
}

void printCenter(const String buf, int x, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h);
  display.setCursor((x - w / 2) + (128 / 2), y);
  display.print(buf);
}
