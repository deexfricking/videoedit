#include <QApplication>
#include <QSurfaceFormat>
#include <QDir>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("VideoEditor");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("VideoEditor");
    app.setOrganizationDomain("videoeditor.local");

    // OpenGL surface for video rendering
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow win;
    win.show();

    return app.exec();
}
