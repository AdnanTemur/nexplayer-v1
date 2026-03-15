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

// File logging for Windows GUI mode
#ifdef Q_OS_WIN
static QFile *logFile = nullptr;
static QTextStream *logStream = nullptr;

void messageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    if (logStream)
    {
        QString prefix;
        switch (type)
        {
        case QtDebugMsg:
            prefix = "[DEBUG]";
            break;
        case QtInfoMsg:
            prefix = "[INFO]";
            break;
        case QtWarningMsg:
            prefix = "[WARN]";
            break;
        case QtCriticalMsg:
            prefix = "[ERROR]";
            break;
        case QtFatalMsg:
            prefix = "[FATAL]";
            break;
        }
        *logStream << prefix << " " << msg << "\n";
        logStream->flush();
    }
}
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // Setup file logging
    QString logPath = QCoreApplication::applicationDirPath() + "/nexplayer.log";
    logFile = new QFile(logPath);
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        logStream = new QTextStream(logFile);
        qInstallMessageHandler(messageHandler);
    }
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
#ifdef Q_OS_WIN
        if (logStream)
        {
            delete logStream;
            logFile->close();
            delete logFile;
        }
#endif
        return -1;
    }

    qDebug() << "Entering event loop...";
    int result = app.exec();

#ifdef Q_OS_WIN
    if (logStream)
    {
        delete logStream;
        logFile->close();
        delete logFile;
    }
#endif

    return result;
}