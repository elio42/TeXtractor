#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QWidget>

#include "../../core/settings.h"
#include "../../core/wizard.h"

class OllamaWizardPage : public QWidget {
    Q_OBJECT

public:
    explicit OllamaWizardPage(Settings &settings, QWidget *parent = nullptr);

    void refreshFromSettings();

signals:
    void finished();
    void cancelled();

private:
    void buildUi();
    void showStep(int index);
    void loadDefaults();
    void saveWizardSettings();

    Settings &settings;
    Wizard wizard;

    QStackedWidget *stepStack;
    QLabel *statusLabel;

    QLineEdit *modelDownloadInput;
    QLineEdit *ollamaIpInput;
    QSpinBox *ollamaPortInput;
    QLineEdit *ollamaModelInput;
    QSpinBox *ollamaKeepAliveInput;
};