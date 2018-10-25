#include "module_base_def.h"
#include "util.h"
#include <stdio.h>
#include <QDateTime>

using namespace std;

#ifdef WIN32
#include <windows.h>
TickTime::TickTime()
{
    LARGE_INTEGER tmp;
    QueryPerformanceFrequency(&tmp);
    freq = (double)tmp.QuadPart;
    QueryPerformanceCounter(&tmp);
    begTick = tmp.QuadPart;
}

TickTime::~TickTime()
{
    LARGE_INTEGER tmp;
    QueryPerformanceCounter(&tmp);
    float difTime = (tmp.QuadPart - begTick) / freq * 1000000;
    DebugLog("tick time %.3f us", difTime);
}
#else
#include <sys/time.h>
TickTime::TickTime()
{
    struct timeval tmp;
    gettimeofday(&tmp, NULL);
    begTick = 1000000 * tmp.tv_sec + tmp.tv_usec;
}

TickTime::~TickTime()
{
    struct timeval tmp;
    gettimeofday(&tmp, NULL);
    long long int endTick = 1000000 * tmp.tv_sec + tmp.tv_usec;
    long long int difTime = endTick - begTick;
    DebugLog("tick time %lld us", difTime);
}
#endif

_UtcTime DateTime::toUtcTime() const
{
    _UtcTime utc;
    QDateTime tmp(QDate(year, month, day), QTime(hour, minute, second, ms));
    utc.tv_sec = tmp.toUTC().toTime_t();
    utc.tv_usec = tmp.toUTC().time().msec() * 1000;
    return utc;
}

string DateTime::toString(int tsType) const
{
    char tmp[50];
    memset(tmp, 0, sizeof(tmp));
    switch (tsType) {
    case TS_DATETIME:
        sprintf(tmp, "%04d-%02d-%02d %02d:%02d:%02d.%03d", year, month, day,
                hour, minute, second, ms);
        break;
    case TS_DATE:
        sprintf(tmp, "%04d-%02d-%02d", year, month, day);
        break;
    case TS_DATE_NODAY:
        sprintf(tmp, "%04d-%02d", year, month);
        break;
    case TS_TIME:
        sprintf(tmp, "%02d:%02d:%02d.%03d", hour, minute, second, ms);
        break;
    case TS_TIME_NOMS:
        sprintf(tmp, "%02d:%02d:%02d", hour, minute, second);
        break;
    default:
        break;
    }
    return tmp;
}

bool DateTime::fromString(const char *strTime, int tsType)
{
    *this = DateTime();
    switch (tsType) {
    case TS_DATETIME:
        sscanf(strTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d", &year, &month, &day,
               &hour, &minute, &second, &ms);
        break;
    case TS_DATE:
        sscanf(strTime, "%04d-%02d-%02d", &year, &month, &day);
        break;
    case TS_DATE_NODAY:
        sscanf(strTime, "%04d-%02d", &year, &month);
        break;
    case TS_TIME:
        sscanf(strTime, "%02d:%02d:%02d.%03d", &hour, &minute, &second, &ms);
        break;
    case TS_TIME_NOMS:
        sscanf(strTime, "%02d:%02d:%02d", &hour, &minute, &second);
        break;
    default:
        break;
    }
    return true;
}

bool DateTime::toNowDateTime()
{
    QDateTime cur = QDateTime::currentDateTime();
    year = cur.date().year();
    month = cur.date().month();
    day = cur.date().day();
    hour = cur.time().hour();
    minute = cur.time().minute();
    second = cur.time().second();
    ms = cur.time().msec();
    return true;
}

DateTime UtcTime::toDateTime(bool bUtc) const
{
    DateTime dat;
    QDateTime tmp = QDateTime::fromTime_t(tv_sec);
    if (bUtc)
        tmp = tmp.toUTC();
    dat.year = tmp.date().year();
    dat.month = tmp.date().month();
    dat.day = tmp.date().day();
    dat.hour = tmp.time().hour();
    dat.minute = tmp.time().minute();
    dat.second = tmp.time().second();
    dat.ms = tv_usec / 1000;
    return dat;
}
