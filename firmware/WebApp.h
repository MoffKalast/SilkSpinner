/* 
	Rui Santos
	Complete project details at https://RandomNerdTutorials.com/esp32-web-server-websocket-sliders/
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files.
	
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <Arduino_JSON.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <EEPROM.h>

struct SaveData {
	int R_mode;
	float R_swipe_speed;
	int R_groups;
	int R_group_len;
	int R_angle_min;
	int R_angle_max;
	bool R_dir;
	
	int L_mode;
	float L_swipe_speed;
	int L_groups;
	int L_group_len;
	int L_angle_min;
	int L_angle_max;
	bool L_dir;
};

const char* ssid = "SilkSpinner";
const char* password = "sviloprejka";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String message = "";
String webdata[3];

//-------------------------------

int L_mode = 0;

float L_swipe_speed = 0.2;
int L_angle = 50;

int L_groups = 4;
int L_group_len = 100;

int L_angle_min = 31;
int L_angle_max = 69;

int L_throttle = 0;

bool L_dir = false;
bool L_start = false;

String L_timestamp = "";
String L_duration = "";

//-------------------------------

int R_mode = 0;

float R_swipe_speed = 0.2;

int R_angle = 50;

int R_groups = 4;
int R_group_len = 100;

int R_angle_min = 31;
int R_angle_max = 69;

int R_throttle = 0;

bool R_dir = false;
bool R_start = false;

String R_timestamp = "";
String R_duration = "";

//-------------------------------

bool setings_updated = true;

extern int counter_left;
extern int counter_right;

extern int L_current_group;
extern int L_count_zeroer;
extern float L_group_progress;
extern bool L_wait_for_servo;

extern int R_current_group;
extern int R_count_zeroer;
extern float R_group_progress;
extern bool R_wait_for_servo;

unsigned long save_time = millis();
bool save_eeprom = false;

void executeCommand(){
	if(webdata[0] == "R"){
		if(webdata[1] == "mode"){
			R_mode = webdata[2].toInt();
		}
		else if(webdata[1] == "swipe_speed"){
			R_swipe_speed = webdata[2].toFloat();
		}
		else if(webdata[1] == "angle"){
			R_angle = webdata[2].toInt();
		}
		else if(webdata[1] == "groups"){
			R_groups = webdata[2].toInt();
		}
		else if(webdata[1] == "group_len"){
			R_group_len = webdata[2].toInt();
		}
		else if(webdata[1] == "angle_limits"){
			R_angle_min = webdata[2].toInt();
			R_angle_max = webdata[3].toInt();
		}
		else if(webdata[1] == "throt"){
			R_throttle = webdata[2].toInt();
		}
		else if(webdata[1] == "dir"){
			R_dir = webdata[2] == "true";
		}
		else if(webdata[1] == "start"){
			R_start = webdata[2] == "true";
			R_wait_for_servo = true;
		}
		else if(webdata[1] == "time"){
			R_timestamp = webdata[2];
		}
		else if(webdata[1] == "duration"){
			R_duration = webdata[2];
		}
		else if(webdata[1] == "clear"){
			counter_right = 0;
			R_timestamp = "";
			R_duration = "";
			R_current_group = 0;
			R_count_zeroer = 0;
			R_group_progress = 0;
			R_wait_for_servo = true;
		}
	}
	else if(webdata[0] == "L"){
		if(webdata[1] == "mode"){
			L_mode = webdata[2].toInt();
		}
		else if(webdata[1] == "swipe_speed"){
			L_swipe_speed = webdata[2].toFloat();
		}
		else if(webdata[1] == "angle"){
			L_angle = webdata[2].toInt();
		}
		else if(webdata[1] == "groups"){
			L_groups = webdata[2].toInt();
		}
		else if(webdata[1] == "group_len"){
			L_group_len = webdata[2].toInt();
		}
		else if(webdata[1] == "angle_limits"){
			L_angle_min = webdata[2].toInt();
			L_angle_max = webdata[3].toInt();
		}
		else if(webdata[1] == "throt"){
			L_throttle = webdata[2].toInt();
		}
		else if(webdata[1] == "dir"){
			L_dir = webdata[2] == "true";
		}
		else if(webdata[1] == "start"){
			L_start = webdata[2] == "true";
			L_wait_for_servo = true;
		}
		else if(webdata[1] == "time"){
			L_timestamp = webdata[2];
		}
		else if(webdata[1] == "duration"){
			L_duration = webdata[2];
		}
		else if(webdata[1] == "clear"){
			counter_left = 0;
			L_timestamp = "";
			L_duration = "";
			L_current_group = 0;
			L_count_zeroer = 0;
			L_group_progress = 0;
			L_wait_for_servo = true;
		}
	}

	SaveData save_data = {
		R_mode,
		R_swipe_speed,
		R_groups,
		R_group_len,
		R_angle_min,
		R_angle_max,		
		R_dir,

		L_mode,
		L_swipe_speed,
		L_groups,
		L_group_len,
		L_angle_min,
		L_angle_max,
		L_dir
	};
	EEPROM.put(0, save_data);

	save_time = millis();
	save_eeprom = true;
	setings_updated = true;	
}

void parseStringMsg(void *arg, uint8_t *data, size_t len) {
	AwsFrameInfo *info = (AwsFrameInfo*)arg;
	if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
		data[len] = 0;
		message = (char*)data;

		int string_index = 0;
		while (message.length() > 0)
		{
			int index = message.indexOf(',');
			if (index == -1)
			{
				webdata[string_index++] = message;
				break;
			}
			else
			{
				webdata[string_index++] = message.substring(0, index);
				message = message.substring(index+1);
			}
		}
		
		executeCommand();
	}
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	switch (type) {
		case WS_EVT_CONNECT:
			Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
			setings_updated = true;
			break;
		case WS_EVT_DISCONNECT:
			Serial.printf("WebSocket client #%u disconnected\n", client->id());
			break;
		case WS_EVT_DATA:
			parseStringMsg(arg, data, len);
			break;
		case WS_EVT_PONG:
		case WS_EVT_ERROR:
			break;
	}
}

void initWebApp(){
	if (!SPIFFS.begin()) {
		Serial.println("SPIFFS Error!");
	}	

	WiFi.setTxPower(WIFI_POWER_19_5dBm);	
	WiFi.softAP(ssid, password, 6, 0, 2);
	Serial.println(WiFi.softAPIP());

	ws.onEvent(onEvent);
	
	server.addHandler(&ws);
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/index.html", "text/html");
	});
	server.serveStatic("/", SPIFFS, "/");
	server.begin();

	EEPROM.begin(128);
	delay(2000);

	SaveData save_data;
	EEPROM.get(0, save_data);

	R_mode = save_data.R_mode;
	R_swipe_speed = save_data.R_swipe_speed;
	R_groups = save_data.R_groups;
	R_group_len = save_data.R_group_len;
	R_angle_min = save_data.R_angle_min;
	R_angle_max = save_data.R_angle_max;		
	R_dir = save_data.R_dir;

	L_mode = save_data.L_mode;
	L_swipe_speed = save_data.L_swipe_speed;
	L_groups = save_data.L_groups;
	L_group_len = save_data.L_group_len;
	L_angle_min = save_data.L_angle_min;
	L_angle_max = save_data.L_angle_max;		
	L_dir = save_data.L_dir;
}

extern int counter_left;
extern int counter_right;

extern float frequency_right;
extern float frequency_left;

extern float wiper_state_left;
extern float wiper_state_right;

extern float motor_vel_right;
extern float motor_vel_left;

extern int finalstate_right;
extern int finalstate_left;

JSONVar telemetry;
JSONVar L_telemetry, R_telemetry;

JSONVar settings;
JSONVar L_settings, R_settings;

void sendTelemetry(){

	R_telemetry["servo_pos"] = wiper_state_right;
	R_telemetry["freq"] = frequency_right;
	R_telemetry["counter"] = counter_right;
	R_telemetry["vel"] = motor_vel_right;

	L_telemetry["servo_pos"] = wiper_state_left;
	L_telemetry["freq"] = frequency_left;
	L_telemetry["counter"] = counter_left;
	L_telemetry["vel"] = motor_vel_left;
	
	telemetry["left"] = L_telemetry;
	telemetry["right"] = R_telemetry;
	ws.textAll(JSON.stringify(telemetry));

	if(save_eeprom && millis() - save_time > 1000*40){
		EEPROM.commit();
		save_eeprom = false;
	}
}

void sendSettingsData(){

	R_settings["mode"] = R_mode;
	R_settings["swipe_speed"] = R_swipe_speed;
	//R_settings["angle"] = R_angle;
	R_settings["groups"] = R_groups;
	R_settings["group_len"] = R_group_len;
	R_settings["angle_min"] = R_angle_min;
	R_settings["angle_max"] = R_angle_max;
	R_settings["throttle"] = R_throttle;
	R_settings["dir"] = R_dir;
	R_settings["start"] = R_start;
	R_settings["time"] = R_timestamp;
	R_settings["duration"] = R_duration;

	L_settings["mode"] = L_mode;
	L_settings["swipe_speed"] = L_swipe_speed;
	//L_settings["angle"] = L_angle;
	L_settings["groups"] = L_groups;
	L_settings["group_len"] = L_group_len;
	L_settings["angle_min"] = L_angle_min;
	L_settings["angle_max"] = L_angle_max;
	L_settings["throttle"] = L_throttle;
	L_settings["dir"] = L_dir;
	L_settings["start"] = L_start;
	L_settings["time"] = L_timestamp;
	L_settings["duration"] = L_duration;

	settings["left"] = L_settings;
	settings["right"] = R_settings;
	
	ws.textAll(JSON.stringify(settings));
}
