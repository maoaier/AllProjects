#include "util.h"
#include <unistd.h>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QStringList>
#include <QMutexLocker>

#ifdef WIN32
#include <windows.h>
#else
#include "sys/statfs.h"
#endif

//转化为2位小数
QString CUtil::formatNumber(qreal number)
{
    return QString::number(number,'f', 3);
}

void CUtil::msleep(ulong ms)
{
#ifndef WIN32
    usleep(ms * 1000);
#else
    Sleep(ms);
#endif
}

void CUtil::getPathSpace(QString path, double &kb)
{
#ifndef WIN32
    struct statfs diskInfo;
    statfs(path.toStdString().c_str(), &diskInfo);
    unsigned long long blocksize = diskInfo.f_bsize;
    unsigned long long freesize = diskInfo.f_bfree;
    kb = blocksize*freesize/1024.0;
#else
    ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
    GetDiskFreeSpaceEx(path.toStdWString().c_str(), &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes);
    kb =  liTotalFreeBytes.QuadPart/1024.0;
#endif
}

static QMutex s_logMut;
static QDateTime s_lastTime;

void CUtil::writeLog(int logType, const char* buffer, ...)
{
    QMutexLocker lock(&s_logMut);
    va_list ap;
    va_start(ap, buffer);

    QDateTime curTime = QDateTime::currentDateTime();
    QString strTmp;
    char *cTmp = new char[1024 * 1024];
    char *vsTmp = new char[1024 * 1024];

    if (curTime.date().day() != s_lastTime.date().day()) {
        // 每天判断一次Log文件数量，如超出则删除旧的Log文件
        QDir dir(_getLogPath());
        QStringList nameFilters;
        nameFilters << "*.log";
        QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
        if (files.size() >= 365)
            dir.remove(*files.begin());
    }

    vsprintf(vsTmp, buffer, ap);
    switch (logType) {
    case Log_Normal:
        sprintf(cTmp, "[%s Normal Log] %s \n",
            curTime.toString("yyyy-MM-dd hh:mm:ss.zzz").toStdString().c_str(), vsTmp);
        break;
    case Log_Error:
        sprintf(cTmp, "[%s Error Log] %s \n",
            curTime.toString("yyyy-MM-dd hh:mm:ss.zzz").toStdString().c_str(), vsTmp);
        break;
    case Log_Debug:
        sprintf(cTmp, "[%s Debug Log] %s \n",
            curTime.toString("yyyy-MM-dd hh:mm:ss.zzz").toStdString().c_str(), vsTmp);
        printf("%s", cTmp);
        goto RET_END;
    default:
        goto RET_END;
    }

    strTmp = cTmp;
    if (!strTmp.isEmpty()) {
        QString filePath = _getLogPath();
        char filename[50] = { 0 };
        sprintf(filename, "%s.log", curTime.toString("yyyyMMdd").toStdString().c_str());
        filePath += filename;
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            file.write(strTmp.toStdString().c_str());
            file.close();
        }
        printf("%s", strTmp.toStdString().c_str());
    }

RET_END:
    s_lastTime = curTime;
    delete[] cTmp;
    delete[] vsTmp;
    va_end(ap);
}

QString CUtil::_getLogPath()
{
#ifndef WIN32
    QString path = "/data/log/";
#else
    QString path = QDir::currentPath() + "/log/";
#endif
    QDir dir(path);
    if (!dir.exists())
        dir.mkdir(path);
    return path;
}

bool CUtil::removeDir(QString path)
{
    if (path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists())
        return true;

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList) {
        if (fi.isFile())
            fi.dir().remove(fi.fileName());
        else
            removeDir(fi.absoluteFilePath());
    }
    return dir.rmpath(dir.absolutePath());
}
