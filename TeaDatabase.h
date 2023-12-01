// TeaDatabase.h
#ifndef TEA_DATABASE_H
#define TEA_DATABASE_H

#include <Arduino.h>

struct TeaType {
	const char* name;
	int brewTime;  // Brew time in seconds
	int brewTemp;  // Brew temperature in °C
};

extern const TeaType teaTypes[];

#endif
