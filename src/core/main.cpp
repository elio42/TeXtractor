#include <QApplication>

#include "../gui/main_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString startupImagePath;
    if (argc > 1) {
        startupImagePath = QString::fromLocal8Bit(argv[1]);
    }

    MainWindow window(startupImagePath);
    window.show();

    return app.exec();
}