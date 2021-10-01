#include "filemanager.h"
#include "ui_filemanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include "../mysocket.h"
#include "File/FTPsocket.h"
#include "customwidget.h"
FileManager::FileManager(QString filename,QString uploadtime,int filesize,int resourceid) :
    ui(new Ui::FileManager)
{
    ui->setupUi(this);
    this->recvSize = 0;
    this->fileName = filename;
    this->fileSize = filesize;
    this->uploadTime = uploadtime;
    this->resourceId = resourceid;
    setFileName();
    setFileSize();
    serUploadTime();
    setFileIcon();
}

FileManager::~FileManager()
{
    delete ui;
}

void FileManager::setFileName()
{
    ui->filenamelabel->setText(fileName);
}

void FileManager::setFileSize()
{
    int fromSize;
    if(fileSize >= 1024 && fileSize < 1024*1024){
        fromSize = fileSize/1024;//KB
        ui->filesizelabel->setText(QString("%1KB").arg(fromSize));
    }else if(fileSize >= 1024*1024){
        fromSize = fileSize/(1024*1024);//M
        ui->filesizelabel->setText(QString("%1M").arg(fromSize));
    }else if(fileSize < 1024)
        ui->filesizelabel->setText(QString(QStringLiteral("%1字节")).arg(fileSize));
}

void FileManager::serUploadTime()
{
    ui->timelabel->setText(uploadTime);
}

void FileManager::setFileIcon()
{
    QString filetype;
    if(fileName.contains('.'))
    filetype = fileName.split(".").at(1);
    else
    filetype = fileName;
    qDebug()<<filetype;
    if(filetype == "zip"){
        ui->fileIcoLabel->setPixmap(QPixmap(":/resources/image/File/zip.png"));
    }else if(filetype == "rar"){
        ui->fileIcoLabel->setPixmap(QPixmap(":/resources/image/File/rar.png"));
    }else if(filetype == "txt"){
        ui->fileIcoLabel->setPixmap(QPixmap(":/resources/image/File/txt.png"));
    }else{
        ui->fileIcoLabel->setPixmap(QPixmap(":/resources/image/File/pic.png"));
    }
}
/*
void FileManager::setFileText(unsigned char *filetext)
{
    //不能直接赋值，必须先申请内存
    this->fileText = new unsigned char[fileSize];
    memcpy(this->fileText,filetext,fileSize);
}
*/
/**
 * @brief FileManager::on_downloadBt_clicked
 */
void FileManager::on_downloadBt_clicked()
{
    //发送获取资源请求，下载资源
    QJsonObject object;
    QJsonObject data_obj;
    data_obj.insert("userid",MyApp::userid);
    data_obj.insert("resourceid",resourceId);
    data_obj.insert("filename",fileName);
    data_obj.insert("filesize",fileSize);

    object.insert("data",data_obj);
    object.insert("type",FILEDownLoadType::FILE_DOWNLOAD_MAG);

    QJsonDocument document;
    document.setObject(object);

    qDebug()<<QStringLiteral("发送内容数据")<<document.toJson();
    FTPsocket::getTcpSocket()->sendData(document);
    startTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");

    connect(FTPsocket::getTcpSocket(),SIGNAL(updateDownloadProcessBar(int)),this,SLOT(updateProcessBar(int)));
    connect(FTPsocket::getTcpSocket(),SIGNAL(setDownloadProcessBarMaxValue(int)),this,SLOT(setProcessBarMaxValue(int)));
    /*
    qDebug()<<"filesize = "<<fileSize;
    FILE* fp;
    //输入要保存的文件名
     qDebug()<<"Enter the name you wanna to save:";
     //以二进制写入方式
     if ( (fp=fopen(fileName.toStdString().c_str(), "wb"))==NULL)
     {
      qDebug()<<"Open File failed!"<<endl;
     }
     //从buffer中写数据到fp指向的文件中
     fwrite(fileText,fileSize, 1, fp);
     //关闭文件指针
     fclose(fp);
     */
}

void FileManager::updateProcessBar(int value)
{
    recvSize += value;
    //更新进度条
    ui->progressBar->setValue(recvSize);
    if(recvSize == ui->progressBar->maximum()){
        qDebug()<<QStringLiteral("取消信号");
        disconnect(FTPsocket::getTcpSocket(),SIGNAL(updateDownloadProcessBar(int)),0,0);
        disconnect(FTPsocket::getTcpSocket(),SIGNAL(setDownloadProcessBarMaxValue(int)),0,0);
        this->recvSize = 0;
        qDebug()<<"recv start time = " << startTime;
        qDebug()<<"recv end time = " << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    }
}

void FileManager::setProcessBarMaxValue(int maxValue)
{
    //设置进度条最大值
    ui->progressBar->setMaximum(maxValue);
    //清空上次下载的进度条
    ui->progressBar->setValue(0);
}

