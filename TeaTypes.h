// TeaTypes.h
#ifndef TEA_TYPES_H
#define TEA_TYPES_H

struct TeaType {
	const char* name;
	int temperature;
};

extern const TeaType greenTea;
extern const TeaType blackTea;
extern const TeaType herbalTea;

#endif
