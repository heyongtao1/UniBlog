#include "clog.h"

#include <QApplication>
#include <QDir>
#include <QDate>
#include <QMetaEnum>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <stdarg.h>

#include <QDebug>

const char PATH_LogPath[] = "/../logs";
const char Suffix[] = ".log";

#define MAX_LOG_LENGH 1024

bool CLog::isFileReady = false;
bool CLog::isRecord2File = true;
CLog::CLOG_LEVEL CLog::logLevel = CLog::RINFO;           //默认是info级

QFile localFile;
QMutex mutex;

CLog::CLog()
{
}

void CLog::setLogLevel(const CLog::CLOG_LEVEL &level)
{
    logLevel = level;
}

bool CLog::init(LogConfig &logConfig)
{
    isRecord2File = logConfig.isRecord2File;
    logLevel = (CLog::CLOG_LEVEL)logConfig.level;

    QString logDir = qApp->applicationDirPath() +  QString(PATH_LogPath);
    if(createDir(logDir))
    {
        QString fileName = logDir + QDir::separator() + QDate::currentDate().toString("yyyy-MM-dd") + QString(Suffix);
        localFile.setFileName(fileName);
        if(localFile.open(QFile::WriteOnly|QFile::Append|QFile::Text))
        {
            isFileReady = true;
        }
    }
    return isFileReady;
}

bool CLog::createDir(QString dirPath)
{
    QFileInfo fileInfo(dirPath);
    if(!fileInfo.exists())
    {
        QDir tmpDir;
        return tmpDir.mkpath(dirPath);
    }

    return true;
}

void CLog::log(CLOG_LEVEL nLevel, const char *fileDesc, const char *functionDesc, int lineNum, const char* data, ...)
{
    QMutexLocker locker(&mutex);
    if(isFileReady && nLevel >= logLevel)
    {
        QString recordInfo = QString("[%1]").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
        recordInfo.append(getLeveDesc(nLevel));

#ifndef QT_NO_DEBUG
        recordInfo.append(QString("[%1:%2:%3]").arg(fileDesc).arg(functionDesc).arg(lineNum));
#endif
        va_list vlist;
        va_start(vlist,data);

        QByteArray byteArray;
#if defined(Q_OS_WIN)
        int recordLen = _vscprintf(data,vlist);
        byteArray.resize(recordLen);
#else
        byteArray.resize(1024);
#endif
        vsprintf(byteArray.data(),data,vlist);
        recordInfo.append(byteArray);
        va_end(vlist);

        recordInfo.append("\n");

        if(isRecord2File){
            localFile.write(recordInfo.toLocal8Bit().data(),recordInfo.toLocal8Bit().length());
            localFile.flush();
        }else{
//            qDebug()<<recordInfo;
        }
    }
}

QString CLog::getLeveDesc(CLog::CLOG_LEVEL level)
{
#if (QT_VERSION > 0x050500)
    static QMetaEnum metaEnum = QMetaEnum::fromType<CLog::CLOG_LEVEL>();
    return QString("[%1]").arg(metaEnum.key(level));
#else
    switch(level)
    {
        case CLOG_LEVEL::INFO:
                                return "[INFO]";
        case CLOG_LEVEL::INFO:
                                return "[WARNING]";
        case CLOG_LEVEL::INFO:
                                return "[ERROR]";
    }
#endif
}

