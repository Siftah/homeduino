#include <SerialCommand.h>
#include <RFControl.h>
#include <RCSwitch.h>

/*
1 On    Decimal: 3402623 (24Bit) Binary: 001100111110101101111111 PulseLength: 306 microseconds Protocol: 1
1 Off   Decimal: 3402615 (24Bit) Binary: 001100111110101101110111 PulseLength: 307 microseconds Protocol: 1
2 On    Decimal: 3402619 (24Bit) Binary: 001100111110101101111011 PulseLength: 307 microseconds Protocol: 1
2 Off   Decimal: 3402611 (24Bit) Binary: 001100111110101101110011 PulseLength: 307 microseconds Protocol: 1
3 On    Decimal: 3402621 (24Bit) Binary: 001100111110101101111101 PulseLength: 307 microseconds Protocol: 1
3 Off   Decimal: 3402613 (24Bit) Binary: 001100111110101101110101 PulseLength: 307 microseconds Protocol: 1
4 On    Decimal: 3402622 (24Bit) Binary: 001100111110101101111110 PulseLength: 306 microseconds Protocol: 1
4 Off   Decimal: 3402614 (24Bit) Binary: 001100111110101101110110 PulseLength: 307 microseconds Protocol: 1
All Off Decimal: 3402616 (24Bit) Binary: 001100111110101101111000 PulseLength: 307 microseconds Protocol: 1
D On     Decimal: 5457 (24Bit) Binary: 000000000001010101010001 Tri-State: 00000FFFFF0F PulseLength: 325 microseconds Protocol: 1
D Off    Decimal: 5396 (24Bit) Binary: 000000000001010100010100 Tri-State: 00000FFF0FF0 PulseLength: 324 microseconds Protocol: 1
*/
RCSwitch mySwitch = RCSwitch();

void argument_error();

SerialCommand sCmd;

#include "rfcontrol_command.h"

void ts_command();
void digital_read_command();
void digital_write_command();
void analog_read_command();
void analog_write_command();
void reset_command();
void pin_mode_command();
void ping_command();
void unrecognized(const char *command);


void setup() {
  mySwitch.enableTransmit(4);
  mySwitch.setProtocol(1);
  mySwitch.setRepeatTransmit(10);
	Serial.begin(115200);
	// Setup callbacks for SerialCommand commands
	sCmd.addCommand("DR", digital_read_command);
	sCmd.addCommand("DW", digital_write_command);
	sCmd.addCommand("AR", analog_read_command);
	sCmd.addCommand("AW", analog_write_command);
	sCmd.addCommand("PM", pin_mode_command);
	sCmd.addCommand("RF", rfcontrol_command);    
	sCmd.addCommand("PING", ping_command);
  sCmd.addCommand("RESET", reset_command);
  sCmd.addCommand("TS", ts_command);
	sCmd.setDefaultHandler(unrecognized);
	Serial.print("ready\r\n");
}

void loop() {
	// handle serial command
	sCmd.readSerial();
	// handle rf control receiving
	rfcontrol_loop();
}

void digital_read_command() {
	char* arg = sCmd.next();
  	if(arg == NULL) {
  		argument_error();
    	return;
  	}
  	int pin = atoi(arg);
  	int val = digitalRead(pin);
  	Serial.print("ACK ");
  	Serial.write('0' + val);
  	Serial.print("\r\n");
}

void analog_read_command() {
	char* arg = sCmd.next();
  	if(arg == NULL) {
  		argument_error();
    	return;
  	}
  	int pin = atoi(arg);
  	int val = analogRead(pin);
  	Serial.print("ACK ");
  	Serial.print(val);
  	Serial.print("\r\n");
}

void digital_write_command() {
	char* arg = sCmd.next();
  	if(arg == NULL) {
  		argument_error();
    	return;
  	}
  	int pin = atoi(arg);
  	arg = sCmd.next();
  	if(arg == NULL) {
  		argument_error();
    	return;
  	}
  	int val = atoi(arg);
  	digitalWrite(pin, val);
  	Serial.print("ACK\r\n");
}

void analog_write_command() {
	char* arg = sCmd.next();
  	if(arg == NULL) {
  		argument_error();
    	return;
  	}
  	int pin = atoi(arg);
  	arg = sCmd.next();
  	if(arg == NULL) {
  		argument_error();
    	return;
  	}
  	int val = atoi(arg);
  	analogWrite(pin, val);
  	Serial.print("ACK\r\n");
}

void pin_mode_command() {
	char* arg = sCmd.next();
  	if(arg == NULL) {
  		argument_error();
    	return;
  	}
  	int pin = atoi(arg);
  	arg = sCmd.next();
  	if(arg == NULL) {
  		argument_error();
    	return;
  	}
  	// INPUT 0x0
	// OUTPUT 0x1
  	int mode = atoi(arg);
  	pinMode(pin, mode);
    Serial.print("ACK\r\n");	
}


void ping_command() {
  char *arg;
  Serial.print("PING");
  arg = sCmd.next();
  if (arg != NULL) {
    Serial.write(' ');
    Serial.print(arg);
  }
  Serial.print("\r\n");
}


void reset_command() {
  RFControl::stopReceiving();
  Serial.print("ready\r\n");
}

void ts_command() {
  char *arg;
  arg = sCmd.next();
  if (arg != NULL) {
    mySwitch.setPulseLength(125);
    mySwitch.sendTriState(arg);
  }
  Serial.print("TS ");
  Serial.println(arg);
}


void argument_error() {
	Serial.print("ERR argument_error\r\n");
}
// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
	Serial.print("ERR unknown_command\r\n");
}

