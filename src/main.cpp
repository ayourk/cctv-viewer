#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>

#include "qmlavplayer.h"
#include "context.h"
#include "singleapplication.h"
#include "context.h"
#include "viewportslayoutscollectionmodel.h"

void registerQmlTypes()
{
    qmlRegisterSingletonType<SingleApplication>("CCTV_Viewer.Utils", 1, 0, "SingleApplication",
                                                [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return new SingleApplication();
    });
    qmlRegisterSingletonType<Context>("CCTV_Viewer.Core", 1, 0, "Context",
                                                [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return new Context();
    });

    qmlRegisterType<QmlAVPlayer>("CCTV_Viewer.Multimedia", 1, 0, "QmlAVPlayer");
    qmlRegisterType<ViewportsLayoutItem>("CCTV_Viewer.Models", 1, 0, "ViewportsLayoutItem");
    qmlRegisterType<ViewportsLayoutModel>("CCTV_Viewer.Models", 1, 0, "ViewportsLayoutModel");
    qmlRegisterType<ViewportsLayoutsCollectionModel>("CCTV_Viewer.Models", 1, 0, "ViewportsLayoutsCollectionModel");
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#if defined(APP_NAME)
    QCoreApplication::setApplicationName(QLatin1String(APP_NAME));
#endif
#if defined(APP_VERSION)
    QCoreApplication::setApplicationVersion(QLatin1String(APP_VERSION));
#endif
#if defined(ORG_NAME)
    QCoreApplication::setOrganizationName(QLatin1String(ORG_NAME));

    // Migration to new org. name
    if (QSettings().allKeys().size() == 0) {
        QStringList keys;
        QVariantMap values;
        QCoreApplication::setOrganizationName(QLatin1String("T171RU"));

        keys = QSettings().allKeys();
        for (int i = 0; i < keys.size(); ++i) {
            values.insert(keys[i], QSettings().value(keys[i]));
        }

        QCoreApplication::setOrganizationName(QLatin1String(ORG_NAME));

        for (int i = 0; i < keys.size(); ++i) {
            QSettings().setValue(keys[i], values[keys[i]]);
        }
    }

#endif
#if defined(ORG_DOMAIN)
    QCoreApplication::setOrganizationDomain(QLatin1String(ORG_DOMAIN));
#endif

    registerQmlTypes();

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QTranslator translator;
    const QString locale = QLocale::system().name();
    translator.load(QLatin1String("cctv-viewer_") + locale, QLatin1String(":/translations/"));
    app.installTranslator(&translator);
    app.setWindowIcon(QIcon(QLatin1String(":/images/cctv-viewer.svg")));

    Context::init();

    engine.addImportPath(":/src/imports");
    const QUrl url(QStringLiteral("qrc:/src/RootWindow.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    // NOTE: Debug
    // Testing Right-to-left User Interfaces...
    // (This code must be removed!!!)
//    QGuiApplication::setLayoutDirection(Qt::RightToLeft);

    return app.exec();
}
