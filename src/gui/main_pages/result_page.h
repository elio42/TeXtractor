#pragma once

#include <QComboBox>
#include <QFutureWatcher>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QStackedWidget>
#include <QToolButton>
#include <QWidget>

#include "../../core/settings.h"

class ResultPage : public QWidget {
    Q_OBJECT

public:
    explicit ResultPage(Settings &settings, QWidget *parent = nullptr);

    void setImagePath(const QString &imagePath);
    void runOcr();
    void refreshAiSection();

signals:
    void settingsRequested();
    void ollamaSetupRequested();
    void geminiSetupRequested();
    void backRequested();

private:
    void buildUi();
    void runAiExtraction();
    void rebuildProviderDropdown();
    void updateAiSectionVisibility();
    bool hasValidImage() const;
    QString selectedProvider() const;

    Settings &settings;

    QLabel *currentImageLabel;
    QTextEdit *ocrOutput;
    QStackedWidget *aiControlsStack;
    QWidget *configuredAiControls;
    QWidget *setupAiControls;
    QComboBox *providerDropdown;
    QPushButton *extractAiButton;
    QPushButton *setupOllamaButton;
    QPushButton *setupGeminiButton;
    QTextEdit *aiOutput;
    QLabel *statusLabel;
    QToolButton *settingsButton;

    QString currentImagePath;

    QFutureWatcher<QString> ocrWatcher;
    QFutureWatcher<QString> aiWatcher;
};
