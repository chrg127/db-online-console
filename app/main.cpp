// java fa schifo!!

#include <QApplication>
#include <QScreen>
#include <QSurfaceFormat>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "qthelpers.hpp"
#include "gui.hpp"
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

    QFile file{"DBINFO.txt"};
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        msgbox("Prima di usare l'applicazione bisogna che un file chiamato \"DBINFO.txt\" esista. "
               "Questo file deve avere il nome utente e la password per connettersi al database, rispettivamente, "
               "nella prima e seconda riga");
        return 1;
    }

    QTextStream filestream{&file};
    QString namestr = filestream.readLine();
    QString passstr = filestream.readLine();

    if (!db::connect("OnlineConsole", namestr, passstr)) {
        msgbox("Impossibile collegarsi al database, verifica di aver scritto correttamente nome e password nel file DBINFO.txt.");
        return 1;
    }

    return app.exec();
}

