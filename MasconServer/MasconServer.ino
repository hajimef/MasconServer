#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#define IR_SEND_PIN 4
#include <IRremote.hpp>

const char* ssid = "your_ssid";
const char* password = "your_pass";

const uint16_t cha_s_up[] = { 800, 1300, 800, 500, 800, 500, 800, 1300, 800, 500, 800, 1300, 800, 1300, 800, 500, 800 };
const uint16_t cha_c_up[] = { 800, 1300, 800, 1300, 800, 500, 800, 1300, 800, 500, 800, 500, 800, 1300, 800, 500, 800 };
const uint16_t cha_s_dn[] = { 800, 1300, 800, 500, 800, 1300, 800, 500, 800, 500, 800, 1300, 800, 500, 800, 1300, 800 };
const uint16_t cha_c_dn[] = { 800, 1300, 800, 1300, 800, 1300, 800, 500, 800, 500, 800, 500, 800, 500, 800, 1300, 800 };
const uint16_t cha_keep[] = { 800, 1300, 800, 500, 800, 500, 800, 500, 800, 500, 800, 1300, 800, 1300, 800, 1300, 800 };
const uint16_t chb_s_up[] = { 800, 500, 800, 500, 800, 500, 800, 1300, 800, 1300, 800, 1300, 800, 1300, 800, 500, 800 };
const uint16_t chb_c_up[] = { 800, 500, 800, 1300, 800, 500, 800, 1300, 800, 1300, 800, 500, 800, 1300, 800, 500, 800 };
const uint16_t chb_s_dn[] = { 800, 500, 800, 500, 800, 1300, 800, 500, 800, 1300, 800, 1300, 800, 500, 800, 1300, 800 };
const uint16_t chb_c_dn[] = { 800, 500, 800, 1300, 800, 1300, 800, 500, 800, 1300, 800, 500, 800, 500, 800, 1300, 800 };
const uint16_t chb_keep[] = { 800, 500, 800, 500, 800, 500, 800, 500, 800, 1300, 800, 1300, 800, 1300, 800, 1300, 800 };

WebServer server(80);
int speed_a = 0, speed_b = 0;
bool is_cha_back = false, is_chb_back = false;

void showPage() {
  String html = "";
  html += "<!doctype html>";
  html += "<html>";
  html += "  <head>";
  html += "    <meta charset=\"utf-8\">";
  html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "    <title>グリップマスコンコントローラ</title>";
  html += "    <link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM\" crossorigin=\"anonymous\">";
  html += "    <style type=\"text/css\">";
  html += "      h1, h2 { text-align: center; }";
  html += "      .buttons { padding-left: 0.5em; padding-right: 0.5em; }";
  html += "      .buttons button { display: block; width: 100%; margin-top: 0.4em; margin-bottom: 0.4em; font-size: 1.3em; }";
  html += "    </style>";
  html += "  </head>";
  html += "  <body>";
  html += "    <div class=\"row\">";
  html += "      <div class=\"col\">";
  html += "        <h1>グリップマスコンコントローラ</h1>";
  html += "      </div>";
  html += "    </div>";
  html += "    <div class=\"row\">";
  html += "      <div class=\"col\">";
  html += "        <h2>Channel A</h2>";
  html += "        <div class=\"buttons\">";
  html += "          <button type=\"button\" class=\"btn btn-success\" id=\"cha_up\">1段加速</button>";
  html += "          <button type=\"button\" class=\"btn btn-success\" id=\"cha_down\">1段減速</button>";
  html += "          <button type=\"button\" class=\"btn btn-primary\" id=\"cha_cup\">連続加速</button>";
  html += "          <button type=\"button\" class=\"btn btn-primary\" id=\"cha_cdown\">連続減速</button>";
  html += "          <button type=\"button\" class=\"btn btn-warning\" id=\"cha_sback\">バック開始</button>";
  html += "          <button type=\"button\" class=\"btn btn-warning\" id=\"cha_eback\">バック終了</button>";
  html += "        </div>";
  html += "      </div>";
  html += "      <div class=\"col\">";
  html += "        <h2>Channel B</h2>";
  html += "        <div class=\"buttons\">";
  html += "          <button type=\"button\" class=\"btn btn-success\" id=\"chb_up\">1段加速</button>";
  html += "          <button type=\"button\" class=\"btn btn-success\" id=\"chb_down\">1段減速</button>";
  html += "          <button type=\"button\" class=\"btn btn-primary\" id=\"chb_cup\">連続加速</button>";
  html += "          <button type=\"button\" class=\"btn btn-primary\" id=\"chb_cdown\">連続減速</button>";
  html += "          <button type=\"button\" class=\"btn btn-warning\" id=\"chb_sback\">バック開始</button>";
  html += "          <button type=\"button\" class=\"btn btn-warning\" id=\"chb_eback\">バック終了</button>";
  html += "        </div>";
  html += "      </div>";
  html += "    </div>";
  html += "  <script";
  html += "  src=\"https://code.jquery.com/jquery-3.7.1.min.js\"";
  html += "  integrity=\"sha256-/JqT3SQfawRcv/BIHPThkBvs0OEvtFFmqPF/lYI/Cxo=\"";
  html += "  crossorigin=\"anonymous\"></script>";
  html += "  <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz\" crossorigin=\"anonymous\"></script>";
  html += "  <script type=\"text/javascript\">";
  html += "function all_button_disable() {";
  html += "  $('button').attr('disabled', 'disabled');";
  html += "}";
  html += "";
  html += "function all_button_enable() {";
  html += "  $('button').removeAttr('disabled');";
  html += "}";
  html += "";
  html += "function do_button(id) {";
  html += "  $.ajax({ url: '/' + id })";
  html += "    .always(function(data) {";
  html += "      all_button_enable();";
  html += "    });";
  html += "  all_button_disable();";
  html += "}";
  html += "";
  html += "$(function() {";
  html += "  console.log('loaded');";
  html += "  $('#cha_up').on('click', function() {";
  html += "    do_button('cha_up');";
  html += "  });";
  html += "  $('#chb_up').on('click', function() {";
  html += "    do_button('chb_up');";
  html += "  });";
  html += "  $('#cha_down').on('click', function() {";
  html += "    do_button('cha_down');";
  html += "  });";
  html += "  $('#chb_down').on('click', function() {";
  html += "    do_button('chb_down');";
  html += "  });";
  html += "  $('#cha_cup').on('click', function() {";
  html += "    do_button('cha_cup');";
  html += "  });";
  html += "  $('#chb_cup').on('click', function() {";
  html += "    do_button('chb_cup');";
  html += "  });";
  html += "  $('#cha_cdown').on('click', function() {";
  html += "    do_button('cha_cdown');";
  html += "  });";
  html += "  $('#chb_cdown').on('click', function() {";
  html += "    do_button('chb_cdown');";
  html += "  });";
  html += "  $('#cha_sback').on('click', function() {";
  html += "    do_button('cha_sback');";
  html += "    $('#cha_eback').removeAttr('disabled');";
  html += "  });";
  html += "  $('#chb_sback').on('click', function() {";
  html += "    do_button('chb_sback');";
  html += "    $('#chb_eback').removeAttr('disabled');";
  html += "  });";
  html += "  $('#cha_eback').on('click', function() {";
  html += "    do_button('cha_eback');";
  html += "  });";
  html += "  $('#chb_eback').on('click', function() {";
  html += "    do_button('chb_eback');";
  html += "  });";
  html += "});";
  html += "  </script>";
  html += "</body>";
  server.send(200, "text/html", html);
}

void sendOk(String json) {
  server.send(200, "application/json", json);
}
void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void handle_cha_Up() {
  if (speed_a < 6) {
    speed_a++;
  }
  Serial.print("Channel A Speed Up : ");
  Serial.println(speed_a);
  for (int i = 0; i < 3; i++) {
    delay(80);
    IrSender.sendRaw(cha_s_up, sizeof(cha_s_up) / sizeof(cha_s_up[0]), NEC_KHZ);
  }
  for (int i = 0; i < 10; i++) {
    delay(80);
    IrSender.sendRaw(cha_keep, sizeof(cha_keep) / sizeof(cha_keep[0]), NEC_KHZ);
  }
  Serial.println("Channel A Speed Up End");
  String json = "{\"ok\":1,\"speed\":" + String(speed_a) + "}";
  sendOk(json);
}

void handle_chb_Up() {
  if (speed_b < 6) {
    speed_b++;
  }
  Serial.print("Channel B Speed Up : ");
  Serial.println(speed_b);
  for (int i = 0; i < 3; i++) {
    delay(80);
    IrSender.sendRaw(chb_s_up, sizeof(chb_s_up) / sizeof(chb_s_up[0]), NEC_KHZ);
  }
  for (int i = 0; i < 10; i++) {
    delay(80);
    IrSender.sendRaw(chb_keep, sizeof(chb_keep) / sizeof(chb_keep[0]), NEC_KHZ);
  }
  Serial.println("Channel B Speed Up End");
  String json = "{\"ok\":1,\"speed\":" + String(speed_b) + "}";
  sendOk(json);
}

void handle_cha_CUp() {
  speed_a = 6;
  Serial.println("Channel A Continuous Speed Up");
  for (int i = 0; i < 3; i++) {
    IrSender.sendRaw(cha_s_up, sizeof(cha_s_up) / sizeof(cha_s_up[0]), NEC_KHZ);
    delay(80);
  }
  for (int i = 0; i < 40; i++) {
    IrSender.sendRaw(cha_c_up, sizeof(cha_c_up) / sizeof(cha_c_up[0]), NEC_KHZ);
    delay(80);
  }
  for (int i = 0; i < 5; i++) {
    IrSender.sendRaw(cha_keep, sizeof(cha_keep) / sizeof(cha_keep[0]), NEC_KHZ);
    delay(80);
  }
  Serial.println("Channel A Continuous Speed Up End");
  String json = "{\"ok\":1,\"speed\":" + String(speed_a) + "}";
  sendOk(json);
}

void handle_chb_CUp() {
  speed_b = 6;
  Serial.println("Channel B Continuous Speed Up");
  for (int i = 0; i < 3; i++) {
    IrSender.sendRaw(chb_s_up, sizeof(chb_s_up) / sizeof(chb_s_up[0]), NEC_KHZ);
    delay(80);
  }
  for (int i = 0; i < 40; i++) {
    IrSender.sendRaw(chb_c_up, sizeof(chb_c_up) / sizeof(chb_c_up[0]), NEC_KHZ);
    delay(80);
  }
  for (int i = 0; i < 5; i++) {
    IrSender.sendRaw(chb_keep, sizeof(chb_keep) / sizeof(chb_keep[0]), NEC_KHZ);
    delay(80);
  }
  Serial.println("Channel B Continuous Speed Up End");
  String json = "{\"ok\":1,\"speed\":" + String(speed_b) + "}";
  sendOk(json);
}

void handle_cha_Down() {
  if (speed_a > 0) {
    speed_a--;
  }
  Serial.print("Channel A Speed Down : ");
  Serial.println(speed_a);
  for (int i = 0; i < 3; i++) {
    delay(80);
    IrSender.sendRaw(cha_s_dn, sizeof(cha_s_dn) / sizeof(cha_s_dn[0]), NEC_KHZ);
  }
  for (int i = 0; i < 10; i++) {
    delay(80);
    IrSender.sendRaw(cha_keep, sizeof(cha_keep) / sizeof(cha_keep[0]), NEC_KHZ);
  }
  Serial.println("Channel A Speed Down End");
  String json = "{\"ok\":1,\"speed\":" + String(speed_a) + "}";
  sendOk(json);
}

void handle_chb_Down() {
  if (speed_b > 0) {
    speed_b--;
  }
  Serial.print("Channel B Speed Down : ");
  Serial.println(speed_b);
  for (int i = 0; i < 3; i++) {
    delay(80);
    IrSender.sendRaw(chb_s_dn, sizeof(chb_s_dn) / sizeof(chb_s_dn[0]), NEC_KHZ);
  }
  for (int i = 0; i < 10; i++) {
    delay(80);
    IrSender.sendRaw(chb_keep, sizeof(chb_keep) / sizeof(chb_keep[0]), NEC_KHZ);
  }
  Serial.println("Channel B Speed Down End");
  String json = "{\"ok\":1,\"speed\":" + String(speed_b) + "}";
  sendOk(json);
}

void handle_cha_CDown() {
  speed_a = 0;
  Serial.println("Channel A Continuous Speed Down");
  for (int i = 0; i < 3; i++) {
    IrSender.sendRaw(cha_s_dn, sizeof(cha_s_dn) / sizeof(cha_s_dn[0]), NEC_KHZ);
    delay(80);
  }
  for (int i = 0; i < 25; i++) {
    IrSender.sendRaw(cha_c_dn, sizeof(cha_c_dn) / sizeof(cha_c_dn[0]), NEC_KHZ);
    delay(80);
  }
  for (int i = 0; i < 10; i++) {
    IrSender.sendRaw(cha_keep, sizeof(cha_keep) / sizeof(cha_keep[0]), NEC_KHZ);
    delay(80);
  }
  Serial.println("Channel A Continuous speed_a Up End");
  String json = "{\"ok\":1,\"speed\":" + String(speed_a) + "}";
  sendOk(json);
}

void handle_chb_CDown() {
  speed_b = 0;
  Serial.println("Channel B Continuous Speed Down");
  for (int i = 0; i < 3; i++) {
    IrSender.sendRaw(chb_s_dn, sizeof(chb_s_dn) / sizeof(chb_s_dn[0]), NEC_KHZ);
    delay(80);
  }
  for (int i = 0; i < 25; i++) {
    IrSender.sendRaw(chb_c_dn, sizeof(chb_c_dn) / sizeof(chb_c_dn[0]), NEC_KHZ);
    delay(80);
  }
  for (int i = 0; i < 10; i++) {
    IrSender.sendRaw(chb_keep, sizeof(chb_keep) / sizeof(chb_keep[0]), NEC_KHZ);
    delay(80);
  }
  Serial.println("Channel B Continuous speed_a Up End");
  String json = "{\"ok\":1,\"speed\":" + String(speed_b) + "}";
  sendOk(json);
}

void handle_cha_Back_Start() {
  Serial.println("Channel A Back Start");
  for (int i = 0; i < 3; i++) {
    IrSender.sendRaw(cha_s_dn, sizeof(cha_s_dn) / sizeof(cha_s_dn[0]), NEC_KHZ);
    delay(80);
  }
  Serial.println("Channel A Back Send End");
  String json = "{\"ok\":1}";
  is_cha_back = true;
  sendOk(json);
}

void handle_cha_Back_End() {
  Serial.println("Channel A Back End");
  for (int i = 0; i < 5; i++) {
    IrSender.sendRaw(cha_keep, sizeof(cha_keep) / sizeof(cha_keep[0]), NEC_KHZ);
    delay(80);
  }
  Serial.println("Channel A Back Send End");
  String json = "{\"ok\":1}";
  is_cha_back = false;
  sendOk(json);
}

void handle_chb_Back_Start() {
  Serial.println("Channel B Back Start");
  for (int i = 0; i < 3; i++) {
    IrSender.sendRaw(chb_s_dn, sizeof(chb_s_dn) / sizeof(chb_s_dn[0]), NEC_KHZ);
    delay(80);
  }
  Serial.println("Channel B Back Send End");
  String json = "{\"ok\":1}";
  is_chb_back = true;
  sendOk(json);
}

void handle_chb_Back_End() {
  Serial.println("Channel B Back End");
  for (int i = 0; i < 5; i++) {
    IrSender.sendRaw(chb_keep, sizeof(chb_keep) / sizeof(chb_keep[0]), NEC_KHZ);
    delay(80);
  }
  Serial.println("Channel B Back Send End");
  String json = "{\"ok\":1}";
  is_chb_back = false;
  sendOk(json);
}

void setup() {
  Serial.begin(115200);
  IrSender.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32mascon")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", showPage);
  server.on("/cha_up", handle_cha_Up);
  server.on("/chb_up", handle_chb_Up);
  server.on("/cha_cup", handle_cha_CUp);
  server.on("/chb_cup", handle_chb_CUp);
  server.on("/cha_down", handle_cha_Down);
  server.on("/chb_down", handle_chb_Down);
  server.on("/cha_cdown", handle_cha_CDown);
  server.on("/chb_cdown", handle_chb_CDown);
  server.on("/cha_sback", handle_cha_Back_Start);
  server.on("/cha_eback", handle_cha_Back_End);
  server.on("/chb_sback", handle_chb_Back_Start);
  server.on("/chb_eback", handle_chb_Back_End);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
  if (is_cha_back) {
    IrSender.sendRaw(cha_c_dn, sizeof(cha_c_dn) / sizeof(cha_c_dn[0]), NEC_KHZ);
    delay(80);    
  }
  if (is_chb_back) {
    IrSender.sendRaw(chb_c_dn, sizeof(chb_c_dn) / sizeof(chb_c_dn[0]), NEC_KHZ);
    delay(80);    
  }
  delay(2);
}
