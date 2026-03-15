#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QDebug>
#include <QDir>
#include "core/PlayerController.h"
#include "video/VideoRenderer.h"

int main(int argc, char *argv[])
{
// Redirect debug to console on Windows
#ifdef _WIN32
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
#endif

    QGuiApplication app(argc, argv);

    // Application metadata
    app.setOrganizationName("Nexylius");
    app.setOrganizationDomain("nexylius.com");
    app.setApplicationName("NexPlayer");
    app.setApplicationVersion("1.0.0");

    qDebug() << "=== NexPlayer Starting ===";
    qDebug() << "Application dir:" << QCoreApplication::applicationDirPath();

    // Check QML folder
    QString qmlPath = QCoreApplication::applicationDirPath() + "/qml";
    qDebug() << "QML path:" << qmlPath;
    qDebug() << "QML folder exists:" << QDir(qmlPath).exists();

    // Create QML engine
    QQmlApplicationEngine engine;

    // Add QML import path
    engine.addImportPath(qmlPath);

    qDebug() << "QML Import Paths:";
    for (const QString &path : engine.importPathList())
    {
        qDebug() << "  " << path;
    }

    // Register C++ types with QML
    qmlRegisterType<PlayerController>("NexPlayer", 1, 0, "PlayerController");
    qmlRegisterType<VideoRenderer>("NexPlayer", 1, 0, "VideoRenderer");

    // Create player controller
    PlayerController playerController;

    // Expose to QML
    engine.rootContext()->setContextProperty("player", &playerController);

// Load main QML file
#ifdef QML_HOT_RELOAD
    const QUrl url = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/../qml/main.qml");
    qDebug() << "🔥 Hot Reload Mode";
    qDebug() << "Loading from:" << url;
#else
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    qDebug() << "Loading QML from resources:" << url;
#endif

    // Connect to see if object was created
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl)
                     {
        if (!obj && url == objUrl) {
            qDebug() << "ERROR: Failed to load QML!";
            QCoreApplication::exit(-1);
        } else if (obj) {
            qDebug() << "QML loaded successfully!";
        } }, Qt::QueuedConnection);

    // Connect to see QML warnings
    QObject::connect(&engine, &QQmlApplicationEngine::warnings,
                     [](const QList<QQmlError> &warnings)
                     {
                         for (const QQmlError &warning : warnings)
                         {
                             qDebug() << "QML Warning:" << warning.toString();
                         }
                     });

    qDebug() << "Loading QML...";
    engine.load(url);

    if (engine.rootObjects().isEmpty())
    {
        qDebug() << "ERROR: No root objects created!";
        return -1;
    }

    qDebug() << "Entering event loop...";
    return app.exec();
}