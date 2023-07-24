#include <QtWidgets/QApplication>
#include "MiniDump.h"
#include "excpt.h"
#include "BgiLogger.h"
#include <QtCore5Compat/qtextcodec.h>
#include <QSharedMemory>
#include <qmessagebox.h>
#include "UserLoginDlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 判断是否有实例已经运行
    QSharedMemory shared("BGI_Master_only_one");
    if (shared.attach())
    {
        QMessageBox::warning(NULL, "温馨提示", "当前软件已经有一个副本在运行，请您确认后重试！", QMessageBox::Yes);

        return 0;
    }
    shared.create(1); 

    QTextCodec* codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);

    QString path = QCoreApplication::applicationDirPath() + "\\log4cxx.properties";
    LoggerHelper::Configure(path.toStdString());

    UserLoginDlg dlg;
    dlg.show();
    
    //ShearDrivenMaster w;
    ////设置状态栏图标
    //w.setWindowIcon(QIcon(":/ShearDrivenMaster/2.ico"));
    //w.setWindowState(Qt::WindowMaximized);
    //w.show();
    return a.exec();
}
