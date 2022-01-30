#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtPlugin>

#include "caseinfomodel.h"
#include "manager.h"


#ifdef QT_STATIC
    Q_IMPORT_PLUGIN(QtQuick2Plugin)
    Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
    Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
    Q_IMPORT_PLUGIN(QtQmlPlugin)
    Q_IMPORT_PLUGIN(QtQuickControls1Plugin)
    Q_IMPORT_PLUGIN(QtQmlModelsPlugin)
#endif


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);


    qmlRegisterType<CaseInfoModel>("Model.CaseInfoModel", 1, 0, "CaseInfoModel");
    qmlRegisterType<Manager>("Manager",1,0,"Manager");


    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
