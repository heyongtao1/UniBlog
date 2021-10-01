#ifndef FILEMAG_H
#define FILEMAG_H

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
class FileMag
{
public:
    FileMag(QFileInfo filePath);
    unsigned char* readPicFile();
    void writePicFile(unsigned char *Pic);
    void setFileName(const QString filename);
    void setFileSize(const qint64 filesize);
    void setUploadTime(QString currtime);
    QString getFileName();
    int getFileSize();
    QString getUploadTime();
    QJsonDocument getJsonFile();
private:
    QFile file;
    qint64 filesize;
    QString filename;
    qint64 sendsize;
    QString filePath;
    QString uploadTime;
};

#endif // FILEMAG_H
