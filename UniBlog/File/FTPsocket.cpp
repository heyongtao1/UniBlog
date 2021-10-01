#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <synchapi.h>
#include <QJsonValue>
#include "FTPsocket.h"
#include "myapp.h"
//实例化唯一Socket对象和销毁Socket对象的对象
FTPsocket* FTPsocket::m_FTPsocket = new FTPsocket();
FTPsocket::deleteFTPsocket FTPsocket::m_deleteFTPsocket;
FTPsocket::FTPsocket(QObject *parent) : QObject(parent)
{
    qDebug() << "FTPsocket::FTPsocket";
    qRegisterMetaType<QJsonValue >("QJsonValue ");
    keeplivetime = 0;
}
/**
 * @brief FTPsocket::~FTPsocket
 */
FTPsocket::~FTPsocket()
{
    qDebug()<<"~FTPsocket() start";
    readDatathread->requestInterruption();
    readDatathread->quit();
    readDatathread->wait();
    if(readDatathread != nullptr){
        readDatathread->deleteLater();
        readDatathread = nullptr;
    }

    m_timer->stop();

    threadWorker->deleteLater();
    threadWorker = nullptr;
    m_tcpSocket->deleteLater();
    m_tcpSocket = nullptr;
    m_timer->deleteLater();
    m_timer = nullptr;
    qDebug()<<"~FTPsocket() end";
}
/**
 * @brief FTPsocket::initFTPsocket
 */
void FTPsocket::initFTPsocket()
{
    m_timer = new QTimer();
    m_tcpSocket = new QTcpSocket();
    connect(m_timer,&QTimer::timeout,this,&FTPsocket::autoConnectToHost);
    connect(m_tcpSocket,&QTcpSocket::disconnected,this,&FTPsocket::serverTodisConnected);
    //子线程去读取数据
    readDatathread = new QThread();
    threadWorker = new ThreadWorker();
    QObject::connect(m_timer,SIGNAL(timeout()),threadWorker,SLOT(doFtpWorker()));
    threadWorker->moveToThread(readDatathread);
    readDatathread->start();

    m_timer->start(200);
}
/**
 * @brief FTPsocket::connectToHost
 * @param host
 * @param port
 * 连接服务器
 */
void FTPsocket::connectToHost(const QString &host, const int &port)
{
    if (m_tcpSocket->isOpen()) m_tcpSocket->abort();
    m_tcpSocket->connectToHost(host, port);
}
/**
 * @brief FTPsocket::closeClientConnected
 * 客户端主动断开连接
 */
void FTPsocket::closeClientConnected()
{
    if (m_tcpSocket->isOpen())
    {
        m_tcpSocket->abort();
        m_timer->start(200);
    }
}
/**
 * @brief FTPsocket::isConnected
 * 是否已经连接状态
 */
bool FTPsocket::isConnected()
{
    if(m_tcpSocket->state() != QTcpSocket::ConnectedState)
    {
        qDebug()<< "connected FTPServer fail";
        return false;
    }
    return true;
}


/**
 * @brief FTPsocket::serverTodisConnected
 * 服务器主动断开连接
 */
void FTPsocket::serverTodisConnected()
{
    if (m_tcpSocket->isOpen())
    {
        m_tcpSocket->abort();
        //m_timer->start(2000);
        connectState = true;
    }
}
/**
 * @brief FTPsocket::autoConnectToHost
 * @param host
 * @param port
 * 定时器心跳包检测连接服务器
 */
void FTPsocket::autoConnectToHost()
{
    if(!isConnected()){
        emit connectFTPServerSignalToUi(false);
        m_timer->setInterval(200);
        connectToHost(MyApp::m_strHostAddr,MyApp::m_nFtpMsgPort);
    }else{
        connectState = true;
        emit connectFTPServerSignalToUi(true);
        m_timer->setInterval(15);//18000/15
        keeplivetime += 15;
        if(keeplivetime == 18000){
            //当客户端空闲时隔18秒发送一次心跳包
            keepaliveConnetcted();
        }
    }
}
/**
 * @brief FTPsocket::sendData
 */
void FTPsocket::sendData(QJsonDocument document)
{
    //发送JSON文档
    if(isConnected()){
         QByteArray bufarray= document.toJson(QJsonDocument::Compact);
         char *sendbuf = bufarray.data();       
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
             //发送包头信息 + 数据信息
             recvsize = m_tcpSocket->write((char *)&tcppackage,16+tcppackage.subPackage.subpackage_len);
             recvsize -=16;
             qDebug() << "send tcppackage.data size = " << recvsize;
             qDebug() << "write subPackage total_len = " << tcppackage.subPackage.total_len;
             qDebug() << "write subPackage total_packageNum = " << tcppackage.subPackage.total_packageNum;
             qDebug() << "write subPackage subpackage_seq = " << tcppackage.subPackage.subpackage_seq;
             qDebug() << "write subPackage subpackage_len = " << tcppackage.subPackage.subpackage_len;
             sendlen += recvsize;
             surplen -= recvsize;
             memset(&tcppackage.subPackage,0,sizeof(tcppackage.subPackage));
             memset(tcppackage.data,'\0',sizeof(tcppackage.data));
             //客户端活动状态，保活参数置零
             keeplivetime = 0;
         }

    }

}

/**
 * @brief FTPsocket::readyrecvData
 */
void FTPsocket::readyrecvData()
{
    int recvlen = 0;
    TcpPackage m_tcppackage;
    int headlen = 0;
    //接收包头信息
    if((headlen = m_tcpSocket->read((char *)&m_tcppackage.subPackage,16)) < 16){
        return ;
    }
    if(m_tcppackage.subPackage.subpackage_seq == 1){
        recvBuffer.recvData = (char *)malloc(51200*sizeof(char));
        memset(recvBuffer.recvData,0,51200);
        //接收数据包的第一包，将包长消息发送到文件下载对象，设置进度条最大值
        emit setDownloadProcessBarMaxValue(m_tcppackage.subPackage.total_len);
    }
    qDebug() << "total_len = " << m_tcppackage.subPackage.total_len;
    qDebug() << "total_packageNum = " << m_tcppackage.subPackage.total_packageNum;
    qDebug() << "subpackage_seq = " << m_tcppackage.subPackage.subpackage_seq;
    qDebug() << "subpackage_len = " << m_tcppackage.subPackage.subpackage_len;
    if(m_tcppackage.subPackage.subpackage_seq > 100000 || m_tcppackage.subPackage.subpackage_seq < 0){
        //错误包
        qDebug() << "error package";
        m_tcpSocket->readAll();
        if(recvBuffer.recvData != nullptr){
            free(recvBuffer.recvData);
            recvBuffer.recvData = nullptr;
        }
        recvBuffer.expandSeq = 1;
        recvBuffer.totalLen = 51200;
        memset(&recvBuffer.usedLen,0,sizeof(recvBuffer.usedLen));
        memset(&recvPack,0,sizeof(recvPack));
        return;
    }
    //判断是否需要扩容
    if(recvBuffer.totalLen-recvBuffer.usedLen < m_tcppackage.subPackage.subpackage_len){
        char *tempBuff = (char *)malloc(recvBuffer.usedLen*sizeof(char));
        memset(tempBuff,0,recvBuffer.usedLen);
        memcpy(tempBuff,recvBuffer.recvData,recvBuffer.usedLen);
        free(recvBuffer.recvData);
        recvBuffer.recvData = nullptr;
        recvBuffer.expandSeq++;
        recvBuffer.recvData = (char *)malloc(recvBuffer.expandSeq*51200*sizeof(char));//增加容量10240
        memset(recvBuffer.recvData,0,recvBuffer.expandSeq*51200);
        memcpy(recvBuffer.recvData,tempBuff,recvBuffer.usedLen);
        recvBuffer.totalLen = recvBuffer.expandSeq*51200;
        free(tempBuff);
        tempBuff = nullptr;
    }
    recvlen = m_tcpSocket->read(recvBuffer.recvData+recvPack.recvlen,m_tcppackage.subPackage.subpackage_len);
    if(recvlen > 0){
        recvPack.recvPackageNum = m_tcppackage.subPackage.total_packageNum;
        recvPack.recvPackageSeq = m_tcppackage.subPackage.subpackage_seq;
        recvPack.recvPackageLen = m_tcppackage.subPackage.subpackage_len;
        recvPack.recvlen += recvlen;
        recvBuffer.usedLen = recvPack.recvlen;
        emit updateDownloadProcessBar(recvlen);
    }else{
        //丢包

    }
    //判断包是否接收完整
    if(recvPack.recvPackageSeq != recvPack.recvPackageNum){
        return ;
    }
    //接收到完整的数据包，处理解析数据包
    QString recvDataString = QString(QLatin1String(recvBuffer.recvData));
    qDebug()<< "recv size = "<< recvDataString.size();
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
    //释放资源，重置变量
    free(recvBuffer.recvData);
    recvBuffer.recvData = nullptr;
    recvBuffer.expandSeq = 1;
    recvBuffer.totalLen = 51200;
    memset(&recvBuffer.usedLen,0,sizeof(recvBuffer.usedLen));
    memset(&recvPack,0,sizeof(recvPack));
}
/**
 * @brief FTPsocket::handleServerMag
 * @param dataVal
 * @param _type
 * 处理文件服务器各种类型信息
 */
void FTPsocket::handleServerMag(QJsonValue &dataVal,int _type)
{
    qDebug()<<"handleServerMag(QJsonValue &dataVal,int _type)";
    if(dataVal.isObject() || dataVal.isArray()){
        QJsonObject dataobj = dataVal.toObject();
        switch(_type){
              case FileMsgTyPe::FILE_SUCCESS: emit picMsgSignal(dataVal);break;

              case FILEDownLoadType::FILE_DOWNLOAD_SUCCESS : emit downloadFileSignal(dataVal);break;
        }
    }
}
/**
 * @brief FTPsocket::keepaliveConnetcted
 * 发送心跳包，保活
 */
void FTPsocket::keepaliveConnetcted()
{
    qDebug()<<"ftp keeplive data send";
    QJsonObject object;
    QJsonObject data_obj;

    object.insert("data",data_obj);
    object.insert("type",KeepLiveType::KEEPLIVE_MAG);

    QJsonDocument document;
    document.setObject(object);
    sendData(document);
}
