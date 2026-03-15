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
    QGuiApplication app(argc, argv);
    
    // Application metadata
    app.setOrganizationName("Nexylius");
    app.setOrganizationDomain("nexylius.com");
    app.setApplicationName("NexPlayer");
    app.setApplicationVersion("1.0.0");
    
    // DIAGNOSTIC: Show where we're running from
    qDebug() << "=== NexPlayer Diagnostics ===";
    qDebug() << "Application dir:" << QCoreApplication::applicationDirPath();
    qDebug() << "Current dir:" << QDir::currentPath();
    
    // Check if qml folder exists
    QString qmlPath = QCoreApplication::applicationDirPath() + "/qml";
    qDebug() << "Looking for QML modules at:" << qmlPath;
    qDebug() << "QML folder exists:" << QDir(qmlPath).exists();
    
    if (QDir(qmlPath).exists()) {
        qDebug() << "QML folder contents:";
        QDir qmlDir(qmlPath);
        for (const QString &entry : qmlDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            qDebug() << "  -" << entry;
            
            // Check for qmldir files
            QString qmldirPath = qmlPath + "/" + entry + "/qmldir";
            if (QFile::exists(qmldirPath)) {
                qDebug() << "    ✓ Has qmldir";
            } else {
                qDebug() << "    ✗ Missing qmldir";
            }
        }
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Add QML import path for deployed modules
    engine.addImportPath(qmlPath);
    
    // Show what import paths Qt is using
    qDebug() << "\nQML Import Paths:";
    for (const QString &path : engine.importPathList()) {
        qDebug() << "  -" << path;
    }
    qDebug() << "=============================\n";
    
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
        qDebug() << "🔥 Hot Reload Mode: Edit QML files and save to see changes!";
        qDebug() << "📂 Loading from:" << url;
    #else
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