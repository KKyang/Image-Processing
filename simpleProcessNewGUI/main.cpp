#include "mainwindow.h"
#include <QtPlugin>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QtSvg/QSvgRenderer>
#include <QReadWriteLock>
Q_DECL_EXPORT QReadWriteLock lock;

//Q_IMPORT_PLUGIN(ExtendPlugins)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile f(":qdarkstyle/style.qss");
        if (!f.exists())
        {
            printf("Unable to set stylesheet, file not found\n");
        }
        else
        {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            a.setStyleSheet(ts.readAll());
        }
        a.setStyle("fusion");

    MainWindow w;
    w.show();

    return a.exec();
}
