#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QWidget>

namespace Ui {
class FileManager;
}

class FileManager : public QWidget
{
    Q_OBJECT

public:
    explicit FileManager(QString filename,QString uploadtime,int filesize,int resourceid);
    ~FileManager();
    void setFileName();
    void setFileSize();
    void serUploadTime();
    void setFileIcon();
    //void setFileText(unsigned char *filetext);
private slots:
    void on_downloadBt_clicked();
private slots:
    void updateProcessBar(int value);
    void setProcessBarMaxValue(int maxValue);
private:
    Ui::FileManager *ui;
    QString fileName;
    QString uploadTime;
    int fileSize;
    int resourceId;
    //unsigned char *fileText;
    int recvSize;
    int recvTotalSize;
    QString startTime;
};

#endif // FILEMANAGER_H
