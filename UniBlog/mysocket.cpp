#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <synchapi.h>
#include <QJsonValue>
#include "mysocket.h"
#include "myapp.h"
//实例化唯一Socket对象和销毁Socket对象的对象
MySocket* MySocket::m_mySocket = new MySocket();
MySocket::deleteMySocket MySocket::m_deleteMySocket;
MySocket::MySocket(QObject *parent) : QObject(parent)
{
    qDebug()<<"MySocket";
    //在不同的线程中使用同一对象，必须在构造函数中注册
    qRegisterMetaType<QJsonValue >("QJsonValue ");
    keeplivetime = 0;
}
/**
 * @brief MySocket::~MySocket
 */
MySocket::~MySocket()
{
    qDebug()<<"~MySocket() start";
    workthread->requestInterruption();
    workthread->quit();
    workthread->wait();
    if(workthread != nullptr){
        workthread->deleteLater();
        workthread = nullptr;
    }
    m_timer->stop();
    threadWorker->deleteLater();
    threadWorker = nullptr;
    m_tcpSocket->deleteLater();
    m_tcpSocket = nullptr;
    m_timer->deleteLater();
    m_timer = nullptr;
    qDebug()<<"~MySocket() end";
}
/**
 * @brief MySocket::initMySocket
 */
void MySocket::initMySocket()
{
    m_timer = new QTimer();
    m_tcpSocket = new QTcpSocket();
    connect(m_timer,&QTimer::timeout,this,&MySocket::autoConnectToHost);
    connect(m_tcpSocket,&QTcpSocket::disconnected,this,&MySocket::serverTodisConnected);
    workthread = new QThread();
    threadWorker = new ThreadWorker();
    QObject::connect(m_timer,SIGNAL(timeout()),threadWorker,SLOT(doTcpWorker()));
    threadWorker->moveToThread(workthread);
    workthread->start();
    m_timer->start(200);
}
/**
 * @brief MySocket::connectToHost
 * @param host
 * @param port
 * 连接服务器
 */
void MySocket::connectToHost(const QString &host, const int &port)
{
    if (m_tcpSocket->isOpen()) m_tcpSocket->abort();
    m_tcpSocket->connectToHost(host, port);
}
/**
 * @brief MySocket::closeClientConnected
 * 客户端主动断开连接
 */
void MySocket::closeClientConnected()
{
    if (m_tcpSocket->isOpen())
    {
        m_tcpSocket->abort();
        m_timer->start(200);
    }
}
/**
 * @brief MySocket::isConnected
 * 是否已经连接状态
 */
bool MySocket::isConnected()
{
    if(m_tcpSocket->state() != QTcpSocket::ConnectedState)
    {
        qDebug()<< "connected fail";
        return false;
    }
    //qDebug()<< "connected success";
    return true;
}


/**
 * @brief MySocket::serverTodisConnected
 * 服务器主动断开连接
 */
void MySocket::serverTodisConnected()
{
    if (m_tcpSocket->isOpen())
    {
        m_tcpSocket->abort();
        //m_timer->start(2000);
        connectState = true;
    }
}
/**
 * @brief MySocket::autoConnectToHost
 * @param host
 * @param port
 * 定时器心跳包检测连接服务器
 */
void MySocket::autoConnectToHost()
{
    if(!isConnected()){
        emit connectTCPServerSignalToUi(false);
        m_timer->setInterval(200);
        connectToHost(MyApp::m_strHostAddr,MyApp::m_nTcpMsgPort);
    }else{
        connectState = true;
        emit connectTCPServerSignalToUi(true);
        m_timer->setInterval(50);//1s 20
        keeplivetime += 50;
        if(keeplivetime == 18000){
            keepaliveConnetcted();
        }
    }
}
/**
 * @brief MySocket::sendData
 */
void MySocket::sendData(QJsonDocument document)
{
    qDebug()<<"void MySocket::sendData(QJsonDocument document)";
    //发送JSON文档
    if(isConnected()){
         QByteArray bufarray= document.toJson(QJsonDocument::Compact);
         char *sendbuf = bufarray.data();
         qDebug() << "sendbuf = " << sendbuf;
         int totallen = document.toJson(QJsonDocument::Compact).size();
         int surplen = totallen;//剩余包的长度
         int sendlen = 0;//实际发送的长度
         int recvsize;
         qDebug() << "package number = " << totallen/10240+1;
         TcpPackage tcppackage;
         for(int i=0;i<totallen/10240+1;i++){
             tcppackage.subPackage.total_len = totallen;
             tcppackage.subPackage.total_packageNum = totallen/10240+1;
             tcppackage.subPackage.subpackage_seq = i+1;
             if(surplen >= 10240){
                 memcpy(tcppackage.data,sendbuf+sendlen,10240);
                 tcppackage.subPackage.subpackage_len = static_cast<int>(strlen(tcppackage.data));
             }else{
                 memcpy(tcppackage.data,sendbuf+sendlen,surplen);
                 tcppackage.subPackage.subpackage_len = surplen;
             }
             //发送包头信息
             recvsize = m_tcpSocket->write((char *)&tcppackage,16+tcppackage.subPackage.subpackage_len);
             recvsize -=16;
             qDebug() << "send tcppackage.data" << recvsize;
             qDebug() << "write subPackage sizeof = " << sizeof(tcppackage.subPackage);
             qDebug() << "write subPackage total_len = " << tcppackage.subPackage.total_len;
             qDebug() << "write subPackage total_packageNum = " << tcppackage.subPackage.total_packageNum;
             qDebug() << "write subPackage subpackage_seq = " << tcppackage.subPackage.subpackage_seq;
             qDebug() << "write subPackage subpackage_len = " << tcppackage.subPackage.subpackage_len;
             qDebug() << "write data package = " << tcppackage.data;
             sendlen += recvsize;
             surplen -= recvsize;
             memset(&tcppackage.subPackage,0,sizeof(tcppackage.subPackage));
             memset(tcppackage.data,0,sizeof(tcppackage.data));
             //客户端活动时，保活参数置零
             keeplivetime = 0;
         }
    }
}

/**
 * @brief MySocket::readyrecvData
 */
void MySocket::readyrecvData()
{
    //qDebug()<<"void MySocket::readyrecvData()";
    int recvlen = 0;
    TcpPackage m_tcppackage;
    //接收包头信息
    if(m_tcpSocket->read((char *)&m_tcppackage.subPackage,16) <= 0){
        return ;
    }
    if(m_tcppackage.subPackage.subpackage_seq == 1){
        recvBuffer.recvData = (char *)malloc(51200*sizeof(char));
        memset(recvBuffer.recvData,0,51200);
    }
    qDebug() << "total_len = " << m_tcppackage.subPackage.total_len;
    qDebug() << "total_packageNum = " << m_tcppackage.subPackage.total_packageNum;
    qDebug() << "subpackage_seq = " << m_tcppackage.subPackage.subpackage_seq;
    qDebug() << "subpackage_len = " << m_tcppackage.subPackage.subpackage_len;
    //判断是否需要扩容
    if(recvBuffer.totalLen-recvBuffer.usedLen < m_tcppackage.subPackage.subpackage_len){
        qDebug() << "expend mement";
        char *tempBuff = (char *)malloc(recvBuffer.usedLen*sizeof(char));
        memset(tempBuff,0,recvBuffer.usedLen);
        memcpy(tempBuff,recvBuffer.recvData,recvBuffer.usedLen);
        free(recvBuffer.recvData);
        recvBuffer.recvData = nullptr;
        recvBuffer.expandSeq++;
        recvBuffer.recvData = (char *)malloc(recvBuffer.expandSeq*51200*sizeof(char));//增加容量51200
        memset(recvBuffer.recvData,0,recvBuffer.expandSeq*51200);
        memcpy(recvBuffer.recvData,tempBuff,recvBuffer.usedLen);
        recvBuffer.totalLen = recvBuffer.expandSeq*51200;
        free(tempBuff);
        tempBuff = nullptr;
    }
    recvlen = m_tcpSocket->read(recvBuffer.recvData+recvPack.recvlen,m_tcppackage.subPackage.subpackage_len);
    if(recvlen > 0){
        qDebug() << "recvlen success len = " << recvlen;
        recvPack.recvPackageNum = m_tcppackage.subPackage.total_packageNum;
        recvPack.recvPackageSeq = m_tcppackage.subPackage.subpackage_seq;
        recvPack.recvPackageLen = m_tcppackage.subPackage.subpackage_len;
        recvPack.recvlen += recvlen;
        recvBuffer.usedLen = recvPack.recvlen;
    }
    //判断包是否接收完整
    if(recvPack.recvPackageSeq != recvPack.recvPackageNum){
        return ;
    }
    qDebug() << "recv data = " << strlen(recvBuffer.recvData);
    QString recvDataString = QString(QLatin1String(recvBuffer.recvData));
    QJsonParseError jsonerror;
    QJsonDocument document = QJsonDocument::fromJson(recvDataString.toUtf8(),&jsonerror);
    if(!document.isNull() && (jsonerror.error == QJsonParseError::NoError))
    {
        if(document.isObject())
        {
              QJsonObject jsonObj = document.object();
              QJsonValue dataVal = jsonObj.value("data");
              int _type = jsonObj.value("type").toInt();
              qDebug() << "type = " << _type;
              handleServerMag(dataVal,_type);
        }
    }else{
        qDebug()<<jsonerror.errorString();
    }
    free(recvBuffer.recvData);
    recvBuffer.recvData = nullptr;
    recvBuffer.expandSeq = 1;
    recvBuffer.totalLen = 51200;
    memset(&recvBuffer.usedLen,0,sizeof(recvBuffer.usedLen));
    memset(&recvPack,0,sizeof(recvPack));
}
/**
 * @brief MySocket::handleServerMag
 * @param dataVal
 * @param _type
 * 处理服务器各种类型信息
 */
void MySocket::handleServerMag(QJsonValue &dataVal,int _type)
{
    qDebug()<<"handleServerMag(QJsonValue &dataVal,int _type)";
    if(dataVal.isObject() || dataVal.isArray()){
        QJsonObject dataobj = dataVal.toObject();
        switch(_type){
              case LoginMagType::LOGIN_SUCCESS   : MyApp::userid = dataobj.value("userid").toInt();
                                                   emit loginMagSignal(true);break;
              case LoginMagType::LOGIN_FAIL      : emit loginMagSignal(false);break;
              case RegistMagType::REGIST_SUCCESS : MyApp::userid = dataobj.value("userid").toInt();
                                                   emit registerMagSignal(true);break;
              case RegistMagType::REGIST_FAIL    : emit registerMagSignal(false);break;
              case BlogMagType::BLOG_SUCCESS    : emit pubBlogEditMagSignal(true);qDebug()<<"emit";break;
              case BlogMagType::BLOG_FAIL    : emit pubBlogEditMagSignal(false);break;

              case LoadBlogMagType::LOADBLOG_SUCCESS : emit userAllBlogEditMagSignal(dataVal);break;

              case FindBlogMagType::FINDBLOG_SUCCESS : emit findBlogEditMagSignal(dataVal);break;

              case RecomBlogMagType::RECOMBLOG_SUCCESS : emit recomBlogEditMsgSignal(dataVal);break;

              //case FileMsgTyPe::FILE_SUCCESS: emit picMsgSignal(dataVal);break;

              //case FILEDownLoadType::FILE_DOWNLOAD_SUCCESS : emit downloadFileSignal(dataVal);break;
        }
    }
}

void MySocket::keepaliveConnetcted()
{
    qDebug()<<"void MySocket::keepaliveConnetcted()";
    QJsonObject object;
    QJsonObject data_obj;

    object.insert("data",data_obj);
    object.insert("type",KeepLiveType::KEEPLIVE_MAG);

    QJsonDocument document;
    document.setObject(object);
    sendData(document);
}
