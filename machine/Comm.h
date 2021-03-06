#include "MessagesTypes.h"
#include <stdint.h>
#include <string.h>
#include <Arduino.h>

class Comm {
private:
	union RegisterMessage {
		byte raw[REGISTER_MESSAGE_SIZE];
		struct Data {
			byte type;
			uint32_t id;
			char name[10];
		} data;
	} register_msg;

	union UpdateMessage {
		byte raw[UPDATE_MESSAGE_SIZE];
		struct Data {
			byte type;
			uint32_t id;
			byte value[4];
		} data;
	} update_msg;

	const byte starting_signal = 0x3C, ending_signal = 0x3E;

public:
	void cregister(uint32_t id, char *name) {
		register_msg.data.type = REGISTER_MESSAGE;
		register_msg.data.id = id;
		strcpy(register_msg.data.name, name);
		Serial.write(starting_signal);
		Serial.write(register_msg.raw, REGISTER_MESSAGE_SIZE);
	}

	int explore(bool *receiving, byte *incoming_data, int *tail) {
		byte rb;

		if (Serial.available() == 0){ // No incoming data
			return 0;
		}
		while (Serial.available() > 0) {
			rb = Serial.read();
			if (*receiving) {
				if (rb != ending_signal) {
					incoming_data[*tail] = rb;
					*tail++;
				} else {
					*receiving = false;
					*tail = 0;
					return 1; // All data received
				}
			} else if (rb == starting_signal) {
				*receiving = true;
			}
		}
		return 2; // Missing data
	}

	void update(uint32_t id, byte value[4]) {
		update_msg.data.type = UPDATE_MESSAGE;
		update_msg.data.id = id;
		memcpy(update_msg.data.value, value, 4);
		Serial.write(starting_signal);
		Serial.write(update_msg.raw, UPDATE_MESSAGE_SIZE);
	}
};