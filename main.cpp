#include "MainWindow.h"
#include "CalqlatrRecord.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    qRegisterMetaType<CalqlatrRecord>("CalqlatrRecord");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Advanced_Notebook_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
