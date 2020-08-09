#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>
#include <QScopedPointer>

#include "../scpi/iscpi.h"
#include "chartcontrol.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    auto scpi = IScpiClient::make();
    engine.rootContext()->setContextProperty("scpiClient", scpi.get());
    QScopedPointer<ChartControl> chart;

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                     [url, &chart, &scpi](QObject *obj, const QUrl &objUrl) {
        if (!obj) {
            qWarning() << "Failed to load file" << objUrl.url();
            QCoreApplication::exit(-1);
            return;
        }

        auto chartObject = obj->findChild<QObject*>("_chart");
        chart.reset(new ChartControl(chartObject));
        scpi->addUpdater(chart.get());

    }, Qt::QueuedConnection);
    engine.load(url);

//    auto rootObject = engine.rootObjects().first();
//    auto chartObject = rootObject->findChild<QObject*>("_chart");
//    QScopedPointer<ChartControl> chart(new ChartControl(chartObject));
//    scpi->addUpdater(chart.get());

    return app.exec();
}
