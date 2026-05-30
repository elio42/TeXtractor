#include "main_window.h"

#include <QFileInfo>
#include <QDialog>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(const QString &startupImagePath, QWidget *parent)
        : QWidget(parent), settings(true), pages(nullptr), homePage(nullptr), resultPage(nullptr), settingsPage(nullptr),
            ollamaWizardPage(nullptr) {
    buildUi();
    connectSignals();
    refreshUiState();

    if (!startupImagePath.isEmpty() && QFileInfo(startupImagePath).exists()) {
        resultPage->setImagePath(startupImagePath);
        pages->setCurrentWidget(resultPage);
        resultPage->runOcr();
    } else {
        pages->setCurrentWidget(homePage);
    }
}

void MainWindow::buildUi() {
    setWindowTitle("TeXtractor");
    resize(980, 680);
    setMinimumSize(375, 250);

    pages = new QStackedWidget(this);

    // Create pages
    homePage = new HomePage(this);
    resultPage = new ResultPage(settings, this);
    settingsPage = new SettingsPage(settings, this);
    ollamaWizardPage = new OllamaWizardPage(settings, this);

    pages->addWidget(homePage);
    pages->addWidget(resultPage);
    pages->addWidget(settingsPage);
    pages->addWidget(ollamaWizardPage);

    auto *root = new QVBoxLayout(this);
    root->addWidget(pages);
    setLayout(root);
}

void MainWindow::refreshUiState() {
    resultPage->refreshAiSection();
}

void MainWindow::connectSignals() {
    // Home page signals
    connect(homePage, &HomePage::openImageRequested, this, [this](const QString &path) {
        QFileInfo info(path);
        if (!info.exists() || !info.isFile()) {
            QMessageBox::warning(this, "Invalid image", "Please provide a valid image path.");
            return;
        }
        resultPage->setImagePath(path);
        pages->setCurrentWidget(resultPage);
        resultPage->runOcr();
    });

    connect(homePage, &HomePage::settingsRequested, this, [this]() {
        settingsPage->loadSettings();
        pages->setCurrentWidget(settingsPage);
    });

    // Result page signals
    connect(resultPage, &ResultPage::backRequested, this, [this]() {
        pages->setCurrentWidget(homePage);
    });

    connect(resultPage, &ResultPage::settingsRequested, this, [this]() {
        settingsPage->loadSettings();
        pages->setCurrentWidget(settingsPage);
    });

    connect(resultPage, &ResultPage::ollamaSetupRequested, this, [this]() {
        ollamaWizardPage->refreshFromSettings();
        pages->setCurrentWidget(ollamaWizardPage);
    });

    connect(resultPage, &ResultPage::geminiSetupRequested, this, [this]() {
        GeminiSetupDialog dialog(settings, this);
        if (dialog.exec() == QDialog::Accepted) {
            settings.updateSettings();
            refreshUiState();
        }
    });

    // Settings page signals
    connect(settingsPage, &SettingsPage::backRequested, this, [this]() {
        pages->setCurrentWidget(homePage);
    });

    connect(settingsPage, &SettingsPage::settingsChanged, this, [this]() {
        settings.updateSettings();
        refreshUiState();
    });

    connect(settingsPage, &SettingsPage::ollamaSetupRequested, this, [this]() {
        ollamaWizardPage->refreshFromSettings();
        pages->setCurrentWidget(ollamaWizardPage);
    });

    connect(settingsPage, &SettingsPage::geminiSetupRequested, this, [this]() {
        GeminiSetupDialog dialog(settings, this);
        if (dialog.exec() == QDialog::Accepted) {
            settings.updateSettings();
            refreshUiState();
        }
    });

    connect(ollamaWizardPage, &OllamaWizardPage::finished, this, [this]() {
        settings.updateSettings();
        refreshUiState();
        pages->setCurrentWidget(resultPage);
    });

    connect(ollamaWizardPage, &OllamaWizardPage::cancelled, this, [this]() {
        pages->setCurrentWidget(homePage);
    });
}
