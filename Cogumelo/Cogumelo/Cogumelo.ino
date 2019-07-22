/*
 Name:		Cogumelo.ino
 Created:	11/20/2018 11:04:49 PM
 Author:	PTL00023-PAN006
// *************************************************
//  Projeto Arduino - Alarme Casa -  Cogumelo
//  Alarme com Arduino e sensor de movimento PIR
//  Sensor de temperatura incluido canal A0
//  Relrogio DTC DS3132 para tempos de disparo
//              HRinnovation V 1.50
// *************************************************
*/

#include <ESP8266WiFi.h>
#include <Virtuino_ESP_WifiServer.h>
#include <DS3231.h>

const char* ssid = "MEO-home";
const char* password = "Anasofia";
WiFiServer server(8000);                      // Server port

Virtuino_ESP_WifiServer virtuino(&server);

int Sensor_1 = 0;
int Sensor_2 = 0;
int Sensor_3 = 0;
int Sirene = D5;
int ACK = 0;
int contador_1 = 0;
int contador_2 = 0;
int contador_3 = 0;
int RESET = 0;
int START = 0;
int temp_sensor = 0;
int temp = 0;
//============================== setup ========================================================
void setup() {
	virtuino.DEBUG = true;                       // set this value TRUE to enable the serial monitor status
	virtuino.password = "1234";                  // Set a password to your web server for more protection
	Serial.begin(115200);                          // Enable this line only if DEBUG=true
	delay(10);
	//----  1. Settings as Station - Connect to a WiFi network
	Serial.println("Ligar a " + String(ssid));
	// If you don't want to config IP manually disable the next four lines
	IPAddress ip(192, 168, 1, 150);            // where 150 is the desired IP Address
	IPAddress gateway(192, 168, 1, 254);       // set gateway to match your network
	IPAddress subnet(255, 255, 255, 0);        // set subnet mask to match your network
	WiFi.config(ip, gateway, subnet);          // If you don't want to config IP manually disable this line
	WiFi.mode(WIFI_STA);                       // Config module as station only.
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("Dispositivo ligado - WiFi ");
	Serial.println(WiFi.localIP());
	// ---- Start the server
	server.begin();
	Serial.println("Servidor Ligado");
	// ================IO Defenicao de entradas/Saidas
	pinMode(D1, INPUT);        // Sensor_1
	pinMode(D2, INPUT);        // Sensor_2
	pinMode(D3, INPUT);        // Sensor_3
	pinMode(D5, OUTPUT);       // Red - Ative Alarme ON
	pinMode(D6, OUTPUT);       // Green Wifi conected
	pinMode(D7, OUTPUT);       // Blue Alarme Enable
	pinMode(D8, OUTPUT);       // Output Alarme General alamr
}
void WIFI() {
	if (WiFi.status() != WL_CONNECTED) {
		START = 0;
		RESET = 0;
		digitalWrite(D6, LOW);
	}
}
void Temperatura() {
	temp = analogRead(A0);
	temp_sensor = float((temp * 3.3 / 1023) / 0.01);
}
//================================== MAIN =====================================
void loop() {
	virtuino.run();
	digitalWrite(D6, HIGH);
	RESET = virtuino.vDigitalMemoryRead(0);
	START = virtuino.vDigitalMemoryRead(1);
	WIFI();
	if (START >= 1) {
		Serial.println("Alarme ON");
		digitalWrite(D7, HIGH);
		//----- Leitura de variaveis no Virtuino 1ª variavel
		RESET = virtuino.vDigitalMemoryRead(0);
		START = virtuino.vDigitalMemoryRead(1);
		if (RESET >= 1) {
			Serial.println("ACK " + String(RESET));
			digitalWrite(D5, LOW);
			Sensor_1 = 0;
			Sensor_2 = 0;
			Sensor_3 = 0;
			contador_1 = 0;
			contador_2 = 0;
			contador_3 = 0;
			delay(10);
		}
		//----  Leitura dor PIR
		Sensor_1 = digitalRead(D1);
		Sensor_2 = digitalRead(D2);
		Sensor_3 = digitalRead(D3);
		//verificar ligacao ao Router
		WIFI();
		// Rotina logica de disparo de intrusao
		if (Sensor_1 >= 1) {
			Serial.println("------ -Sensor 1 - Ativado");
			contador_1 = contador_1 + 1;
			virtuino.vMemoryWrite(2, contador_1);
		}
		if (Sensor_2 >= 1) {
			Serial.println("------ -Sensor 2 - Ativado");
			contador_2 = contador_2 + 1;
			virtuino.vMemoryWrite(3, contador_2);
		}
		if (Sensor_3 >= 1) {
			Serial.println("------ -Sensor 3 - Ativado");
			contador_3 = contador_3 + 1;
			virtuino.vMemoryWrite(4, contador_3);
		}
		// Disparo de alarme geral de intrusao
		if (contador_1 >= 5 || contador_2 >= 5 || contador_3 >= 5 && START >= 1) {
			Serial.println("___________________ - *Alarme ativo * -_________________________________");
			digitalWrite(Sirene, HIGH);
			contador_1 = 0;
			contador_2 = 0;
			contador_3 = 0;
		}
	}
	else
		Serial.println("Alarme OFF");
	if (START <= 0) {
		digitalWrite(D7, LOW);
		digitalWrite(D5, LOW);
	}
	Temperatura();
	virtuino.vMemoryWrite(1, temp_sensor);
	Serial.print("Temperatura");
	Serial.println(temp_sensor);
	delay(25);
	digitalWrite(D6, LOW);
	digitalWrite(D7, LOW);
	delay(450);
}
