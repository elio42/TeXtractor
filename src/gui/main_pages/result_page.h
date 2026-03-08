#pragma once

#include <QComboBox>
#include <QFutureWatcher>
#include <QLabel>
#include <QTextEdit>
#include <QWidget>

#include "../../core/settings.h"

class ResultPage : public QWidget {
    Q_OBJECT

public:
    explicit ResultPage(Settings &settings, QWidget *parent = nullptr);

    void setImagePath(const QString &imagePath);
    void runOcr();

signals:
    void settingsRequested();
    void backRequested();

private:
    void buildUi();
    void runAiExtraction();
    bool hasValidImage() const;
    QString selectedProvider() const;

    Settings &settings;

    QLabel *currentImageLabel;
    QTextEdit *ocrOutput;
    QComboBox *providerDropdown;
    QTextEdit *aiOutput;
    QLabel *statusLabel;

    QString currentImagePath;

    QFutureWatcher<QString> ocrWatcher;
    QFutureWatcher<QString> aiWatcher;
};
