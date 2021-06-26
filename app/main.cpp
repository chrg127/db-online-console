// java fa schifo!!

#include <QApplication>
#include <QScreen>
#include <QSurfaceFormat>
#include "qthelpers.hpp"
#include "window.hpp"
#include "database.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);
    format.setDepthBufferSize(16);
    QSurfaceFormat::setDefaultFormat(format);

    Window window;
    window.adjustSize();
    window.move(QGuiApplication::primaryScreen()->availableGeometry().center() - window.rect().center());
    window.show();

    if (!connect_db("OnlineConsole")) {
        msgbox("Couldn't connect to the database. Quitting.");
        return 1;
    }

    return app.exec();
}
