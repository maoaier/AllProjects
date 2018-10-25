#ifndef CUTIL_H
#define CUTIL_H

#include <QString>

typedef enum _LogType
{
    Log_Normal = 1,
    Log_Error,
    Log_Debug,
}LogType;

#define NormalLog(...) CUtil::writeLog(Log_Normal, __VA_ARGS__)
#define ErrorLog(...) CUtil::writeLog(Log_Error, __VA_ARGS__)
#define DebugLog(...) CUtil::writeLog(Log_Debug, __VA_ARGS__)

class CUtil
{
public:
    static QString formatNumber(qreal number);
    static void msleep(ulong ms);
    static void getPathSpace(QString path, double &kb);
    static void writeLog(int logType, const char* buffer, ...);
    static bool removeDir(QString path);

protected:
    static QString _getLogPath();
};

#endif // CUTIL_H
