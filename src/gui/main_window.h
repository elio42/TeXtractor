#pragma once

#include <QComboBox>
#include <QFutureWatcher>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTextEdit>
#include <QWidget>

#include "../core/settings.h"

class MainWindow : public QWidget {
public:
    explicit MainWindow(const QString &startupImagePath, QWidget *parent = nullptr);

private:
    void buildUi();
    QWidget *buildHomePage();
    QWidget *buildResultPage();
    void connectSignals();

    void loadSettingsToUi();
    void saveSettingsFromUi();

    void setCurrentImagePath(const QString &imagePath);
    void runOcrForCurrentImage();
    void runAiForCurrentImage();

    bool hasValidCurrentImage() const;
    QString selectedProvider() const;

    Settings settings;

    QStackedWidget *pages;

    QWidget *homePage;
    QWidget *resultPage;

    QLineEdit *imagePathInput;

    QLineEdit *ocrLanguageInput;
    QComboBox *defaultProviderInput;
    QLineEdit *geminiApiKeyInput;
    QLineEdit *geminiApiUrlInput;
    QLineEdit *ollamaIpInput;
    QSpinBox *ollamaPortInput;
    QLineEdit *ollamaModelInput;
    QSpinBox *ollamaKeepAliveInput;

    QLabel *currentImageLabel;
    QTextEdit *ocrOutput;
    QComboBox *providerDropdown;
    QTextEdit *aiOutput;
    QLabel *statusLabel;

    QString currentImagePath;

    QFutureWatcher<QString> ocrWatcher;
    QFutureWatcher<QString> aiWatcher;
};
