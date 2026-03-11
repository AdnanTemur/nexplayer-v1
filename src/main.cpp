#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "core/PlayerController.h"
#include "video/VideoRenderer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Application metadata
    app.setOrganizationName("Nexylius");
    app.setOrganizationDomain("nexylius.com");
    app.setApplicationName("NexPlayer");
    app.setApplicationVersion("1.0.0");
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Register C++ types with QML
    qmlRegisterType<PlayerController>("NexPlayer", 1, 0, "PlayerController");
    qmlRegisterType<VideoRenderer>("NexPlayer", 1, 0, "VideoRenderer");
    
    // Create player controller
    PlayerController playerController;
    
    // Expose to QML
    engine.rootContext()->setContextProperty("player", &playerController);
    
    // Load main QML file
    // For development: use file path for hot reload
    // For production: use qrc resource
    
    #ifdef QML_HOT_RELOAD
        // Development mode: load from filesystem (hot reload enabled!)
        const QUrl url = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/../qml/main.qml");
        qDebug() << "🔥 Hot Reload Mode: Edit QML files and save to see changes!";
        qDebug() << "📂 Loading from:" << url;
    #else
        // Production mode: load from embedded resources
        const QUrl url(QStringLiteral("qrc:/main.qml"));
    #endif
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}
