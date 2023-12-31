// TeaTypes.h
#ifndef TEA_TYPES_H
#define TEA_TYPES_H

struct teaType {
    const char* teaName;
    int teaTime;
    int teaTemp;
};

const teaType[] = {
    {"White Tea", 270000, 79},
    {"Green Tea", 240000, 79},
    {"Black Tea", 210000, 91},
    {"Oolong Tea", 210000, 91},
    {"Herbal Tea", 800000, 99},
};

#endif // TEA_TYPES_H
