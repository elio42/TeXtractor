#include "ollama_wizard_page.h"

#include <QFont>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

OllamaWizardPage::OllamaWizardPage(Settings &settings, QWidget *parent)
    : QWidget(parent), settings(settings), stepStack(nullptr), statusLabel(nullptr),
      modelDownloadInput(nullptr), ollamaIpInput(nullptr), ollamaPortInput(nullptr),
      ollamaModelInput(nullptr), ollamaKeepAliveInput(nullptr) {
    buildUi();
    refreshFromSettings();
}

void OllamaWizardPage::refreshFromSettings() {
    loadDefaults();
    showStep(0);
    statusLabel->setText("Ready to start the Ollama setup.");
}

void OllamaWizardPage::buildUi() {
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("Ollama Setup", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    title->setFont(titleFont);
    layout->addWidget(title);

    auto *subtitle = new QLabel("Follow the three setup steps below to install and configure Ollama.", this);
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);

    stepStack = new QStackedWidget(this);

    auto *installPage = new QWidget(this);
    auto *installLayout = new QVBoxLayout(installPage);
    auto *installInfo = new QLabel(
        "Placeholder: explain what the official Ollama install script does, what the user should expect, and any platform-specific notes you want to add later.",
        installPage);
    installInfo->setWordWrap(true);

    auto *installRow = new QHBoxLayout();
    auto *installButton = new QPushButton("Install Ollama", installPage);
    auto *installHint = new QLabel("Launch official Ollama script in a terminal", installPage);
    installHint->setWordWrap(true);
    installRow->addWidget(installButton);
    installRow->addWidget(installHint, 1);

    auto *continueRow1 = new QHBoxLayout();
    auto *continueButton1 = new QPushButton("Continue", installPage);
    auto *continueHint1 = new QLabel("Ollama has been installed or is installed already", installPage);
    continueHint1->setWordWrap(true);
    continueRow1->addWidget(continueButton1);
    continueRow1->addWidget(continueHint1, 1);

    auto *cancelRow1 = new QHBoxLayout();
    auto *cancelButton1 = new QPushButton("Cancel", installPage);
    cancelRow1->addWidget(cancelButton1);
    cancelRow1->addStretch(1);

    installLayout->addWidget(installInfo);
    installLayout->addSpacing(16);
    installLayout->addLayout(installRow);
    installLayout->addLayout(continueRow1);
    installLayout->addLayout(cancelRow1);
    installLayout->addStretch(1);

    auto *modelPage = new QWidget(this);
    auto *modelLayout = new QVBoxLayout(modelPage);
    auto *modelInfo = new QLabel(
        "Download one or more Ollama models here. You can press Install multiple times or skip this step entirely.",
        modelPage);
    modelInfo->setWordWrap(true);

    modelDownloadInput = new QLineEdit(modelPage);
    modelDownloadInput->setPlaceholderText("Model name, for example gemma4:e4b");

    auto *modelRow = new QHBoxLayout();
    auto *modelInstallButton = new QPushButton("Install Model", modelPage);
    auto *modelInstallHint = new QLabel("Launch ollama pull in a terminal", modelPage);
    modelInstallHint->setWordWrap(true);
    modelRow->addWidget(modelInstallButton);
    modelRow->addWidget(modelInstallHint, 1);

    auto *continueRow2 = new QHBoxLayout();
    auto *continueButton2 = new QPushButton("Continue", modelPage);
    auto *continueHint2 = new QLabel("Skip model installation and move to settings", modelPage);
    continueHint2->setWordWrap(true);
    continueRow2->addWidget(continueButton2);
    continueRow2->addWidget(continueHint2, 1);

    modelLayout->addWidget(modelInfo);
    modelLayout->addWidget(modelDownloadInput);
    modelLayout->addLayout(modelRow);
    modelLayout->addLayout(continueRow2);
    modelLayout->addStretch(1);

    auto *settingsPage = new QWidget(this);
    auto *settingsLayout = new QVBoxLayout(settingsPage);
    auto *settingsInfo = new QLabel(
        "Review the Ollama connection settings below. Change any value that does not match your installation, then save.",
        settingsPage);
    settingsInfo->setWordWrap(true);

    auto *formBox = new QGroupBox("Ollama Settings", settingsPage);
    auto *form = new QFormLayout(formBox);

    ollamaIpInput = new QLineEdit(formBox);
    ollamaPortInput = new QSpinBox(formBox);
    ollamaPortInput->setRange(1, 65535);
    ollamaModelInput = new QLineEdit(formBox);
    ollamaKeepAliveInput = new QSpinBox(formBox);
    ollamaKeepAliveInput->setRange(0, 3600);

    form->addRow("IP Address:", ollamaIpInput);
    form->addRow("Port:", ollamaPortInput);
    form->addRow("Model:", ollamaModelInput);
    form->addRow("Keep Alive (seconds):", ollamaKeepAliveInput);

    auto *saveRow = new QHBoxLayout();
    auto *backButton3 = new QPushButton("Back", settingsPage);
    auto *saveButton = new QPushButton("Save", settingsPage);
    saveButton->setDefault(true);
    saveRow->addWidget(backButton3);
    saveRow->addStretch(1);
    saveRow->addWidget(saveButton);

    settingsLayout->addWidget(settingsInfo);
    settingsLayout->addWidget(formBox);
    settingsLayout->addLayout(saveRow);
    settingsLayout->addStretch(1);

    stepStack->addWidget(installPage);
    stepStack->addWidget(modelPage);
    stepStack->addWidget(settingsPage);
    layout->addWidget(stepStack, 1);

    statusLabel = new QLabel("Ready", this);
    statusLabel->setStyleSheet("color: gray;");
    layout->addWidget(statusLabel);

    connect(installButton, &QPushButton::clicked, this, [this]() {
        wizard.install_ollama();
        statusLabel->setText("Ollama install script launched in a terminal.");
    });

    connect(continueButton1, &QPushButton::clicked, this, [this]() {
        showStep(1);
        statusLabel->setText("Step 2: download a model or continue without one.");
    });

    connect(cancelButton1, &QPushButton::clicked, this, [this]() {
        emit cancelled();
    });

    connect(modelInstallButton, &QPushButton::clicked, this, [this]() {
        const QString model = modelDownloadInput->text().trimmed();
        if (model.isEmpty()) {
            statusLabel->setText("Enter a model name before installing.");
            return;
        }

        wizard.download_ollama_model(model.toStdString());
        statusLabel->setText("Model download launched in a terminal.");
    });

    connect(continueButton2, &QPushButton::clicked, this, [this]() {
        if (!modelDownloadInput->text().trimmed().isEmpty()) {
            ollamaModelInput->setText(modelDownloadInput->text().trimmed());
        }
        showStep(2);
        statusLabel->setText("Step 3: review and save Ollama settings.");
    });

    connect(backButton3, &QPushButton::clicked, this, [this]() {
        showStep(1);
        statusLabel->setText("Step 2: download a model or continue without one.");
    });

    connect(saveButton, &QPushButton::clicked, this, [this]() {
        saveWizardSettings();
        statusLabel->setText("Ollama setup saved.");
        emit finished();
    });
}

void OllamaWizardPage::showStep(int index) {
    stepStack->setCurrentIndex(index);
}

void OllamaWizardPage::loadDefaults() {
    const QString ip = QString::fromStdString(settings.getOllamaIP());
    const int port = settings.getOllamaPort();
    const QString model = QString::fromStdString(settings.getOllamaModel());
    const int keepAlive = settings.getOllamaKeepAlive();

    modelDownloadInput->setText(model.isEmpty() ? QStringLiteral("gemma4:e4b") : model);
    ollamaIpInput->setText(ip.isEmpty() ? QStringLiteral("127.0.0.1") : ip);
    ollamaPortInput->setValue(port > 0 ? port : 11434);
    ollamaModelInput->setText(model.isEmpty() ? QStringLiteral("gemma4:e4b") : model);
    ollamaKeepAliveInput->setValue(keepAlive >= 0 ? keepAlive : 0);
}

void OllamaWizardPage::saveWizardSettings() {
    const QString model = modelDownloadInput->text().trimmed();
    if (!model.isEmpty()) {
        ollamaModelInput->setText(model);
    }

    wizard.setup_ollama(
        ollamaIpInput->text().trimmed().toStdString(),
        ollamaPortInput->value(),
        ollamaModelInput->text().trimmed().toStdString(),
        ollamaKeepAliveInput->value());
}