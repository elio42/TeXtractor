#pragma once

#include <QDialog>

#include "../../core/settings.h"

class QLineEdit;

class GeminiSetupDialog : public QDialog {
    Q_OBJECT

public:
    explicit GeminiSetupDialog(Settings &settings, QWidget *parent = nullptr);

private:
    void buildUi();
    bool saveSettings();

    Settings &settings;
    QLineEdit *apiUrlInput;
    QLineEdit *apiKeyInput;
};