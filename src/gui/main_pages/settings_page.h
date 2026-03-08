#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>

#include "../../core/settings.h"

class SettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsPage(Settings &settings, QWidget *parent = nullptr);

    void loadSettings();
    void saveSettings();

signals:
    void backRequested();

private:
    void buildUi();

    Settings &settings;

    QComboBox *ocrLanguageDropdown;
    QLineEdit *ocrLanguageCustomInput;
    QComboBox *defaultProviderInput;
    QLineEdit *geminiApiKeyInput;
    QLineEdit *geminiApiUrlInput;
    QLineEdit *ollamaIpInput;
    QSpinBox *ollamaPortInput;
    QLineEdit *ollamaModelInput;
    QSpinBox *ollamaKeepAliveInput;
};
