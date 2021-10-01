#ifndef FTPsocket_H
#define FTPsocket_H

#include <QObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QVector>
#include <QList>
#include <QThread>
#include <QTimer>
#include "myapp.h"
#include "threadworker.h"

class FTPsocket : public QObject
{
    Q_OBJECT
private:
    /**
     * @brief FTPsocket
     * @param parent
     * 设置私有权限，防止被实例化
     */
    explicit FTPsocket(QObject *parent = nullptr);
public:
    ~FTPsocket();
    void initFTPsocket();
    void connectToHost(const QString &host, const int &port);
    void closeClientConnected();
    bool isConnected();
    void sendData(QJsonDocument mdocument);
    //单例模式饿汉式
    static FTPsocket *getTcpSocket(){
        return m_FTPsocket;
    }
    //为了销毁m_FTPsocket而创建的类
    class deleteFTPsocket
    {
    public:
        deleteFTPsocket(){}
        ~deleteFTPsocket(){
            m_FTPsocket->deleteLater();
            m_FTPsocket = nullptr;
            qDebug()<<"m_FTPsocket delete";
        }
    };
public:
    void handleServerMag(QJsonValue &dataVal,int _type);
    /*这个实际上是服务器的tcp连接的keeplive机制，
     * 当服务器检测到一个socket端长时间不活动的时候，
     * 就会发送一个探测包检测client端是否还在，
     * 而当client端收到不回应的时候，会关闭连接，回收资源。*/
    //增加socket保活参数配置，覆盖系统配置,每隔18s发送一个心跳包过去
    void keepaliveConnetcted();
signals:
    void connectFTPServerSignalToUi(bool state);
signals:
    void picMsgSignal(QJsonValue datval);
    void downloadFileSignal(QJsonValue datval);
    void updateDownloadProcessBar(int value);
    void setDownloadProcessBarMaxValue(int maxvalue);
public slots:
    void serverTodisConnected();
    void autoConnectToHost();
    void readyrecvData();
private:
    QTimer *m_timer;
    QTcpSocket* m_tcpSocket;
    bool connectState;
    static FTPsocket *m_FTPsocket;
    static deleteFTPsocket m_deleteFTPsocket;
    //接收缓冲区要设置扩容
    FTPrecvMemory recvBuffer;
    recvPackage recvPack;
    //TcpSubPackage sendPack;

    QThread *readDatathread;
    ThreadWorker *threadWorker;
    int work_id;
    //保活连接参数
    int keeplivetime;
};
#endif // FTPsocket_H
