#include "filemag.h"
#include <QFileInfo>
#include <QJsonObject>
#include "myapp.h"
#include <QDebug>
#include <fstream>
#include <iostream>
#include <QDateTime>
#include "event/base64.h"
#include "../myapp.h"
using namespace std;

FileMag::FileMag(QFileInfo fileinfo)
{
    this->filePath = fileinfo.filePath();
    //初始化
    filesize = 0;
    sendsize = 0;
    setFileName(fileinfo.fileName());
    setFileSize(fileinfo.size());
}

unsigned char* FileMag::readPicFile()
{
     //图像数据长度
     int length;
     //文件指针
     FILE* fp;
     //输入要读取的图像名
     qDebug()<<"Enter Image name:";
     //以二进制方式打开图像  r 以只读方式打开文件，该文件必须存在。
     if ( (fp=fopen(filePath.toStdString().c_str(), "rb" ))==NULL )
     {
        qDebug()<<"Open " << filename <<" failed!";
        //exit(0);
        return nullptr;
     }
     //获取图像数据总长度
     fseek(fp, 0, SEEK_END);
     length=ftell(fp);
     rewind(fp);
     //根据图像数据长度分配内存buffer
     unsigned char* fileBuffer=(unsigned char*)malloc( length* sizeof(unsigned char) );
     //将图像数据读入buffer
     fread(fileBuffer, length, 1, fp);
     fclose(fp);
     return fileBuffer;
}

void FileMag::writePicFile(unsigned char *Pic)
{
    unsigned char* ImgBuffer = Pic;
    FILE* fp;
    //输入要保存的文件名
     cout<<"Enter the name you wanna to save:";
     //以二进制写入方式
     if ( (fp=fopen("mytest.jpg", "wb"))==NULL)
     {
      cout<<"Open File failed!"<<endl;
      exit(0);
     }
     int length = filesize;
     //从buffer中写数据到fp指向的文件中
     fwrite(ImgBuffer, length, 1, fp);
     cout<<"Done!"<<endl;
     //关闭文件指针，释放buffer内存
     fclose(fp);
     free(ImgBuffer);
}

void FileMag::setFileName(const QString filename)
{
    this->filename = filename;
}

void FileMag::setFileSize(const qint64 filesize)
{
    this->filesize = filesize;
}

void FileMag::setUploadTime(QString currtime)
{
    this->uploadTime = currtime;
}

QString FileMag::getFileName()
{
    return this->filename;
}

int FileMag::getFileSize()
{
    return this->filesize;
}

QString FileMag::getUploadTime()
{
    return this->uploadTime;
}

QJsonDocument FileMag::getJsonFile()
{
    //考虑子线程去读取文件
    unsigned char* filetext = this->readPicFile();
    //设置上传时间
    setUploadTime(QDateTime::currentDateTime().toString("yyyy.MM.dd"));
	//编码
    string encodeText;
    CBase64::Encode(filetext,this->getFileSize(),encodeText);

    QJsonObject object;
    QJsonObject data_obj;
    data_obj.insert("userid",MyApp::userid);
    data_obj.insert("filetext",encodeText.data());
    data_obj.insert("filesize",getFileSize());
    data_obj.insert("filename",getFileName());
    data_obj.insert("uploadtime",getUploadTime());
    data_obj.insert("filetype",FileType::FILE_TYPE_PICTURE);

    object.insert("data",data_obj);
    object.insert("type",FileMsgTyPe::FILE_MAG);

    QJsonDocument document;
    document.setObject(object);
    return document;
}
