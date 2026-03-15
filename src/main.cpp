#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "core/PlayerController.h"
#include "video/VideoRenderer.h"

// Custom message handler to write to file
QFile *logFile = nullptr;
QTextStream *logStream = nullptr;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString formattedMsg;
    switch (type) {
        case QtDebugMsg:    formattedMsg = QString("[DEBUG] %1").arg(msg); break;
        case QtInfoMsg:     formattedMsg = QString("[INFO] %1").arg(msg); break;
        case QtWarningMsg:  formattedMsg = QString("[WARNING] %1").arg(msg); break;
        case QtCriticalMsg: formattedMsg = QString("[CRITICAL] %1").arg(msg); break;
        case QtFatalMsg:    formattedMsg = QString("[FATAL] %1").arg(msg); break;
    }
    
    if (logStream) {
        *logStream << formattedMsg << "\n";
        logStream->flush();
    }
}

int main(int argc, char *argv[])
{
    // Setup log file FIRST - before anything else
    QString logPath = QCoreApplication::applicationDirPath() + "/nexplayer-debug.log";
    logFile = new QFile(logPath);
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        logStream = new QTextStream(logFile);
        qInstallMessageHandler(messageHandler);
        qDebug() << "=== NexPlayer Debug Log ===";
        qDebug() << "Log file:" << logPath;
    }
    
    QGuiApplication app(argc, argv);
    
    app.setOrganizationName("Nexylius");
    app.setOrganizationDomain("nexylius.com");
    app.setApplicationName("NexPlayer");
    app.setApplicationVersion("1.0.0");
    
    qDebug() << "Application directory:" << QCoreApplication::applicationDirPath();
    
    QQmlApplicationEngine engine;
    
    // Add QML import path
    QString qmlPath = QCoreApplication::applicationDirPath() + "/qml";
    engine.addImportPath(qmlPath);
    
    qDebug() << "QML import path added:" << qmlPath;
    qDebug() << "QML folder exists:" << QDir(qmlPath).exists();
    
    qDebug() << "All QML import paths:";
    for (const QString &path : engine.importPathList()) {
        qDebug() << "  -" << path;
    }
    
    // Register types
    qmlRegisterType<PlayerController>("NexPlayer", 1, 0, "PlayerController");
    qmlRegisterType<VideoRenderer>("NexPlayer", 1, 0, "VideoRenderer");
    
    PlayerController playerController;
    engine.rootContext()->setContextProperty("player", &playerController);
    
    // Connect warnings
    QObject::connect(&engine, &QQmlApplicationEngine::warnings,
                     [](const QList<QQmlError> &warnings) {
        qDebug() << "=== QML WARNINGS/ERRORS ===";
        for (const QQmlError &warning : warnings) {
            qDebug() << warning.toString();
        }
    });
    
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    qDebug() << "Loading QML from:" << url.toString();
    
    engine.load(url);
    
    if (engine.rootObjects().isEmpty()) {
        qDebug() << "FATAL: No root objects created!";
        qDebug() << "Check the warnings above for details.";
        
        if (logStream) {
            delete logStream;
            delete logFile;
        }
        return -1;
    }
    
    qDebug() << "SUCCESS: QML loaded, root objects created:" << engine.rootObjects().size();
    
    int result = app.exec();
    
    // Cleanup
    if (logStream) {
        delete logStream;
        delete logFile;
    }
    
    return result;
}