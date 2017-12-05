#include <QQmlApplicationEngine>
#include <QtWebEngine>

#include <clocale>

#define APP_TITLE "Stremio - All You Can Watch"

#define DESKTOP true

#define BREAKPAD_ENDPOINT "http://ivo_strem_io.bugsplat.com/post/bp/crash/postBP.php"

#ifdef DESKTOP
#include <QtWidgets/QApplication>
typedef QApplication Application;

#include <QQmlEngine>

#include <QBreakpadHandler.h>
#include <QStandardPaths>

#include "mainapplication.h"
#include "stremioprocess.h"
#include "mpv.h"
#include "screensaver.h"
#include "razerchroma.h"
#include "qclipboardproxy.h"

#else
#include <QGuiApplication>
#endif

#include "luminati.h"

Luminati luminati;

void lumChoiceChanged(void) {
	luminati.emitEvent();
}

void InitializeParameters(QQmlApplicationEngine& engine, MainApp& app) {
    QQmlContext *ctx = engine.rootContext();

    ctx->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
    ctx->setContextProperty("appTitle", QString(APP_TITLE));
    ctx->setContextProperty("autoUpdater", app.autoupdater);

    #ifdef QT_DEBUG
        ctx->setContextProperty("debug", true);
    #else
        ctx->setContextProperty("debug", false);
    #endif
}

void CrashHandler(){
    QBreakpadInstance.setDumpPath(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    //QBreakpadInstance.setUploadUrl(QUrl(BREAKPAD_ENDPOINT));
    //QBreakpadInstance.sendDumps();
}

int main(int argc, char **argv)
{
	lum_sdk_set_choice_change_cb((lum_sdk_choice_change_t)&lumChoiceChanged);

	#if defined(DESKTOP) && !defined(QT_DEBUG)
    CrashHandler();
    #endif

    #ifdef _WIN32
    // Default to ANGLE (DirectX), because that seems to eliminate so many issues on Windows
    // Also, according to the docs here: https://wiki.qt.io/Qt_5_on_Windows_ANGLE_and_OpenGL, ANGLE is also preferrable
    // We do not need advanced OpenGL features but we need more universal support
    Application::setAttribute(Qt::AA_UseOpenGLES);
    #endif

    Application::setAttribute(Qt::AA_EnableHighDpiScaling);

    Application::setApplicationName("Stremio");
    Application::setApplicationVersion(STREMIO_SHELL_VERSION);
    Application::setOrganizationName("Smart Code ltd");
    Application::setOrganizationDomain("stremio.com");

    MainApp app(argc, argv, true);
    if( app.isSecondary() ) {
        if( app.arguments().count() > 1)
            app.sendMessage( app.arguments().at(1).toUtf8() );
        else
            app.sendMessage( "SHOW" );
        //app.sendMessage( app.arguments().join(' ').toUtf8() );
        return 0;
    }

    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");
    
    qmlRegisterType<Process>("com.stremio.process", 1, 0, "Process");
    qmlRegisterType<ScreenSaver>("com.stremio.screensaver", 1, 0, "ScreenSaver");
    qmlRegisterType<MpvObject>("com.stremio.libmpv", 1, 0, "MpvObject");
    qmlRegisterType<RazerChroma>("com.stremio.razerchroma", 1, 0, "RazerChroma");
    //qmlRegisterType<Luminati>("com.stremio.luminati", 1, 0, "Luminati");
    qmlRegisterType<ClipboardProxy>("com.stremio.clipboard", 1, 0, "Clipboard");

    QQmlApplicationEngine engine;
    InitializeParameters(engine, app);
    QtWebEngine::initialize();
	engine.rootContext()->setContextProperty("luminati", &luminati);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject::connect( &app, &SingleApplication::receivedMessage, &app, &MainApp::processMessage );
    QObject::connect( &app, SIGNAL(receivedMessage(QVariant, QVariant)), engine.rootObjects().value(0), SLOT(onAppMessageReceived(QVariant, QVariant)) );

    return app.exec();
}
