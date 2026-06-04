#pragma once

#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>
#include <QPushButton>

#include "../../core/settings.h"

class SettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsPage(Settings &settings, QWidget *parent = nullptr);

    void loadSettings();
    void saveSettings();

signals:
    void backRequested();
    void settingsChanged();
    void ollamaSetupRequested();
    void geminiSetupRequested();

private:
    void buildUi();
    void updateProviderVisibility();
    void rebuildDefaultProviderOptions();

    Settings &settings;

    QGroupBox *ocrBox;
    QGroupBox *aiBox;
    QGroupBox *geminiBox;
    QGroupBox *ollamaBox;

    QComboBox *ocrLanguageDropdown;
    QLineEdit *ocrLanguageCustomInput;
    QComboBox *defaultProviderInput;
    QLineEdit *geminiApiKeyInput;
    QLineEdit *geminiApiUrlInput;
    QLineEdit *ollamaIpInput;
    QSpinBox *ollamaPortInput;
    QLineEdit *ollamaModelInput;
    QSpinBox *ollamaKeepAliveInput;
    QPushButton *setupGeminiButton;
    QPushButton *resetGeminiButton;
    QPushButton *setupOllamaButton;
    QPushButton *resetOllamaButton;
};
