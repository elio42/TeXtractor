#include <QApplication>
#include <iostream>

#include "gui/main_window.h"

int main(int argc, char *argv[]) {
    if (argc > 1) {
        QString arg = QString::fromLocal8Bit(argv[1]);
        if (arg == "--version" || arg == "-v") {
            std::cout << "TeXtractor version " << TEXTRACTOR_VERSION << std::endl;
            return 0;
        }
    }

    QApplication app(argc, argv);

    QString startupImagePath;
    if (argc > 1) {
        startupImagePath = QString::fromLocal8Bit(argv[1]);
    }

    MainWindow window(startupImagePath);
    window.show();

    return app.exec();
}