#include "configfile.h"

bool MeasureConfig::ToDefaultEventParam(int fq, float volnor, float curnor, int vl, float mc)
{
    if (fq < 0 || fq >= MAX_FREQ_MODE || volnor < 0)
        return false;
    if (vl < 0 || vl >= MAX_VOLT_LEVEL || mc < 0)
        return false;

    freqMode = fq;
    voltNominal = volnor;
    currNominal = curnor;
    voltLevel = vl;
    miniCapacity = mc;
    freqUpLimit = freqMode == FREQ_50HZ ? 50.02 : 60.02;
    freqDownLimit = freqMode == FREQ_50HZ ? 49.98 : 59.98;

    voltUpLimit = 110;
    voltDownLimit = 90;
    if (voltLevel == VOLT_LEVEL_380V)
        voltUpLimit = 107;
    else if (voltLevel >= VOLT_LEVEL_35KV) {
        voltUpLimit = 107;
        voltDownLimit = 93;
    }

    if (voltNominal > 400)
        voltRange = VOLT_RANGE_1000V;
    else if (voltNominal > 100)
        voltRange = VOLT_RANGE_400V;
    else if (voltNominal > 10)
        voltRange = VOLT_RANGE_100V;
    else
        voltRange = VOLT_RANGE_10V;

    voltUnbaLimit = 4;
    pstLimit = pltLimit = 1;

    if (voltLevel > VOLT_LEVEL_110KV)
        pltLimit = 0.8;

    if (voltLevel < VOLT_LEVEL_6KV) {
        voltThdLimit = 5;
        voltOddHarmLimit = 4;
        voltEvenHarmLimit = 2;
    }
    else if (voltLevel < VOLT_LEVEL_35KV) {
        voltThdLimit = 4;
        voltOddHarmLimit = 3.2;
        voltEvenHarmLimit = 1.6;
    }
    else if (voltLevel < VOLT_LEVEL_110KV) {
        voltThdLimit = 3;
        voltOddHarmLimit = 2.4;
        voltEvenHarmLimit = 1.2;
    }
    else {
        voltThdLimit = 2;
        voltOddHarmLimit = 1.6;
        voltEvenHarmLimit = 0.8;
    }

    float currHarmDef[6][24] = {
        { 78, 62, 39, 62, 26, 44, 19, 21, 16, 28, 13, 24,
          11, 12, 9.7, 18, 8.6, 16, 7.8, 8.9, 7.1, 14, 6.5, 12 },
        { 43, 34, 21, 34, 14, 24, 11, 11, 8.5, 16, 7.1, 13,
          6.1, 6.8, 5.3, 10, 4.7, 9, 4.3, 4.9, 3.9, 7.4, 3.6, 6.8 },
        { 26, 20, 13, 20, 8.5, 15, 6.4, 6.8, 5.1, 9.3, 4.3, 7.9,
          3.7, 4.1, 3.2, 6, 2.8, 5.4, 2.6, 2.9, 2.3, 4.5, 2.1, 4.1 },
        { 15, 12, 7.7, 12, 5.1, 8.8, 3.8, 4.1, 3.1, 5.6, 2.6, 4.7,
          2.2, 2.5, 1.9, 3.6, 1.7, 3.2, 1.5, 1.8, 1.4, 2.7, 1.3, 2.5 },
        { 16, 13, 8.1, 13, 5.4, 9.3, 4.1, 4.3, 3.3, 5.9, 2.7, 5,
          2.3, 2.6, 2, 3.8, 1.8, 3.4, 1.6, 1.9, 1.5, 2.8, 1.4, 2.6 },
        { 12, 9.6, 6, 9.6, 4, 6.8, 3, 3.2, 2.4, 4.3, 2, 3.7,
          1.7, 1.9, 1.5, 2.8, 1.3, 2.5, 1.2, 1.4, 1.1, 2.1, 1, 1.9 }};
    float standCapacity[7] = { 10, 100, 100, 250, 500, 750, 2000 };
    for (int i = 0; i != 24; i++) {
        if (voltLevel < VOLT_LEVEL_6KV)
            currHarmLimit[i] = currHarmDef[0][i]*miniCapacity/standCapacity[0];
        else if (voltLevel < VOLT_LEVEL_10KV)
            currHarmLimit[i] = currHarmDef[1][i]*miniCapacity/standCapacity[1];
        else if (voltLevel < VOLT_LEVEL_35KV)
            currHarmLimit[i] = currHarmDef[2][i]*miniCapacity/standCapacity[2];
        else if (voltLevel < VOLT_LEVEL_66KV)
            currHarmLimit[i] = currHarmDef[3][i]*miniCapacity/standCapacity[3];
        else if (voltLevel < VOLT_LEVEL_110KV)
            currHarmLimit[i] = currHarmDef[4][i]*miniCapacity/standCapacity[4];
        else if (voltLevel < VOLT_LEVEL_220KV)
            currHarmLimit[i] = currHarmDef[5][i]*miniCapacity/standCapacity[5];
        else
            currHarmLimit[i] = currHarmDef[5][i]*miniCapacity/standCapacity[6];
    }

    voltSwell = 115;
    voltSag = 85;
    voltInterrupt = 10;
    voltSagLag = voltInterruptLag = voltSwellLag = 2;
    currShock = 120;
    currShockLag = 2;

    return true;
}

bool MeasureConfig::IsDiffEventParam(const _MeasureConfig &other)
{
    if (voltNominal != other.voltNominal || voltRange != other.voltRange ||
        currNominal != other.currNominal || pt != other.pt || ct != other.ct ||
        voltLevel != other.voltLevel || miniCapacity != other.miniCapacity ||
        freqUpLimit != other.freqUpLimit || freqDownLimit != other.freqDownLimit ||
        voltUpLimit != other.voltUpLimit || voltDownLimit != other.voltDownLimit ||
        voltUnbaLimit != other.voltUnbaLimit || pstLimit != other.pstLimit ||
        pltLimit != other.pltLimit || voltThdLimit != other.voltThdLimit ||
        voltOddHarmLimit != other.voltOddHarmLimit || voltEvenHarmLimit != other.voltEvenHarmLimit)
        return true;
    return false;
}

QString MeasureConfig::VoltLevelToString(int voltLevel)
{
    QString voltLevels[] = { "380V", "3kV", "6kV", "10kV", "20kV", "35kV",
                             "66kV", "110kV", "220kV", "330kV", "500kV", "750kV", "1000kV"};
    if (voltLevel < 0 || voltLevel >= sizeof(voltLevels))
        return "";
    return voltLevels[voltLevel];
}
