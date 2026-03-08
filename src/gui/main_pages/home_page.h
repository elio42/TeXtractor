#pragma once

#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class HomePage : public QWidget {
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);

signals:
    void openImageRequested(const QString &imagePath);
    void settingsRequested();

private:
    void buildUi();

    QLineEdit *imagePathInput;
};
