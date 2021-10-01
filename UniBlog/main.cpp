#ifdef _DEBUG
//#include "vld.h"
#include <QSplashScreen>
/*日志信息头文件*/
#include <QMutex>
#include <QFile>
#include <QMessageLogContext>
#include <QDateTime>
#include "event/globalapplocation.h"
#include "bloglogin.h"
#include <QTextCodec>

/*qInstallMessageHandler自定义消息处理函数，然后安装该函数，qInstallMessageHandler(outputMessage)
 注意此时QDebug的消息将会输出在日志文件，Qt程序调式时不再打印。*/
void outputMessage(QtMsgType type,const QMessageLogContext &context,const QString &msg);
int main(int argc, char *argv[])
{
    //qInstallMessageHandler(outputMessage);//安装消息处理程序
    //全局Ui窗口事件对象
    QApplication a(argc, argv);
    BlogLogin w;
    w.show();
    return a.exec();
}
void outputMessage(QtMsgType type,const QMessageLogContext &context,const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QByteArray localMsg = msg.toLocal8Bit();
    QString text;
    switch (type)
    {
        case QtDebugMsg:
            text = QString("Debug:");
            break;
        case QtWarningMsg:
            text = QString("Warning:");
            break;
        case QtCriticalMsg:
            text = QString("Critical:");
            break;
        case QtFatalMsg:
            text = QString("Fatal:");
            break;
        default:
            text = QString("Debug:");
    }

    // 设置输出信息格式
   QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
   QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
   //QString strMessage = QString("%1 %2 \t%3 \t%4").arg(text).arg(context_info).arg(strDateTime).arg(msg);
   QString strMessage = QString("%1 \t%2 \t%3").arg(text).arg(strDateTime).arg(msg);
   // 输出信息至文件中（读写、追加形式）
   QFile file(QString("./log/"+QDateTime::currentDateTime().toString("yyyy-MM-dd").append("-log.txt")));
   file.open(QIODevice::ReadWrite | QIODevice::Append);
   QTextStream stream(&file);
   stream << strMessage << "\r\n";
   file.flush();
   file.close();
   //解锁
   mutex.unlock();
}
#endif
