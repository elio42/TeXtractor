#include "main_window.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>

#include "../ai-recognition/gemini_client.h"
#include "../ai-recognition/ollama_client.h"
#include "../ocr/ocr.h"

MainWindow::MainWindow(const QString &startupImagePath, QWidget *parent)
    : QWidget(parent), settings(true), pages(nullptr), homePage(nullptr), resultPage(nullptr), imagePathInput(nullptr),
      ocrLanguageInput(nullptr), defaultProviderInput(nullptr), geminiApiKeyInput(nullptr), geminiApiUrlInput(nullptr),
      ollamaIpInput(nullptr), ollamaPortInput(nullptr), ollamaModelInput(nullptr), ollamaKeepAliveInput(nullptr),
      currentImageLabel(nullptr), ocrOutput(nullptr), providerDropdown(nullptr), aiOutput(nullptr), statusLabel(nullptr) {
    buildUi();
    loadSettingsToUi();
    connectSignals();

    if (!startupImagePath.isEmpty() && QFileInfo(startupImagePath).exists()) {
        setCurrentImagePath(startupImagePath);
        pages->setCurrentWidget(resultPage);
        runOcrForCurrentImage();
    } else {
        pages->setCurrentWidget(homePage);
        if (!startupImagePath.isEmpty()) {
            statusLabel->setText("Startup image path was not valid.");
        }
    }
}

void MainWindow::buildUi() {
    setWindowTitle("TeXtractor");
    resize(980, 680);

    pages = new QStackedWidget(this);
    homePage = buildHomePage();
    resultPage = buildResultPage();
    pages->addWidget(homePage);
    pages->addWidget(resultPage);

    auto *root = new QVBoxLayout(this);
    root->addWidget(pages);
    setLayout(root);
}

QWidget *MainWindow::buildHomePage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);

    auto *title = new QLabel("TeXtractor", page);
    auto *subtitle = new QLabel("Open an image and extract text with OCR and AI", page);
    layout->addWidget(title);
    layout->addWidget(subtitle);

    auto *openBox = new QGroupBox("Open Image", page);
    auto *openLayout = new QHBoxLayout(openBox);
    imagePathInput = new QLineEdit(openBox);
    imagePathInput->setPlaceholderText("/path/to/image.png");
    auto *browseButton = new QPushButton("Browse", openBox);
    auto *runOcrButton = new QPushButton("Open and Run OCR", openBox);
    openLayout->addWidget(imagePathInput, 1);
    openLayout->addWidget(browseButton);
    openLayout->addWidget(runOcrButton);
    openBox->setLayout(openLayout);
    layout->addWidget(openBox);

    auto *settingsBox = new QGroupBox("Settings", page);
    auto *form = new QFormLayout(settingsBox);

    ocrLanguageInput = new QLineEdit(settingsBox);

    defaultProviderInput = new QComboBox(settingsBox);
    defaultProviderInput->addItem("Ollama", "ollama");
    defaultProviderInput->addItem("Gemini", "gemini");

    geminiApiKeyInput = new QLineEdit(settingsBox);
    geminiApiKeyInput->setEchoMode(QLineEdit::Password);
    geminiApiUrlInput = new QLineEdit(settingsBox);
    ollamaIpInput = new QLineEdit(settingsBox);

    ollamaPortInput = new QSpinBox(settingsBox);
    ollamaPortInput->setRange(1, 65535);

    ollamaModelInput = new QLineEdit(settingsBox);

    ollamaKeepAliveInput = new QSpinBox(settingsBox);
    ollamaKeepAliveInput->setRange(0, 3600);

    form->addRow("OCR Language", ocrLanguageInput);
    form->addRow("Default AI Provider", defaultProviderInput);
    form->addRow("Gemini API Key", geminiApiKeyInput);
    form->addRow("Gemini API URL", geminiApiUrlInput);
    form->addRow("Ollama IP", ollamaIpInput);
    form->addRow("Ollama Port", ollamaPortInput);
    form->addRow("Ollama Model", ollamaModelInput);
    form->addRow("Ollama Keep Alive", ollamaKeepAliveInput);

    settingsBox->setLayout(form);
    layout->addWidget(settingsBox);

    auto *actions = new QHBoxLayout();
    auto *saveSettingsButton = new QPushButton("Save Settings", page);
    auto *goToResultButton = new QPushButton("Go to Results", page);
    actions->addWidget(saveSettingsButton);
    actions->addWidget(goToResultButton);
    actions->addStretch(1);
    layout->addLayout(actions);
    layout->addStretch(1);

    connect(browseButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Select Image", QString(), "Images (*.png *.jpg *.jpeg *.bmp *.tif *.tiff)");
        if (!filePath.isEmpty()) {
            imagePathInput->setText(filePath);
        }
    });

    connect(runOcrButton, &QPushButton::clicked, this, [this]() {
        saveSettingsFromUi();
        setCurrentImagePath(imagePathInput->text());
        if (!hasValidCurrentImage()) {
            QMessageBox::warning(this, "Invalid image", "Please provide a valid image path.");
            return;
        }
        pages->setCurrentWidget(resultPage);
        runOcrForCurrentImage();
    });

    connect(saveSettingsButton, &QPushButton::clicked, this, [this]() {
        saveSettingsFromUi();
        QMessageBox::information(this, "Settings", "Settings saved.");
    });

    connect(goToResultButton, &QPushButton::clicked, this, [this]() {
        saveSettingsFromUi();
        pages->setCurrentWidget(resultPage);
    });

    return page;
}

QWidget *MainWindow::buildResultPage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);

    auto *topRow = new QHBoxLayout();
    auto *backButton = new QPushButton("Back to Home", page);
    currentImageLabel = new QLabel("Image: (none)", page);
    topRow->addWidget(backButton);
    topRow->addWidget(currentImageLabel, 1);
    layout->addLayout(topRow);

    auto *ocrTitle = new QLabel("OCR Output", page);
    ocrOutput = new QTextEdit(page);
    ocrOutput->setReadOnly(true);
    layout->addWidget(ocrTitle);
    layout->addWidget(ocrOutput, 1);

    auto *aiControls = new QHBoxLayout();
    providerDropdown = new QComboBox(page);
    providerDropdown->addItem("Ollama", "ollama");
    providerDropdown->addItem("Gemini", "gemini");
    auto *extractAiButton = new QPushButton("Extract with AI", page);
    aiControls->addWidget(new QLabel("Provider", page));
    aiControls->addWidget(providerDropdown);
    aiControls->addWidget(extractAiButton);
    aiControls->addStretch(1);
    layout->addLayout(aiControls);

    auto *aiTitle = new QLabel("AI Output", page);
    aiOutput = new QTextEdit(page);
    aiOutput->setReadOnly(true);
    layout->addWidget(aiTitle);
    layout->addWidget(aiOutput, 1);

    statusLabel = new QLabel("Ready", page);
    layout->addWidget(statusLabel);

    connect(backButton, &QPushButton::clicked, this, [this]() {
        pages->setCurrentWidget(homePage);
    });

    connect(extractAiButton, &QPushButton::clicked, this, [this]() {
        saveSettingsFromUi();
        runAiForCurrentImage();
    });

    return page;
}

void MainWindow::connectSignals() {
    connect(&ocrWatcher, &QFutureWatcher<QString>::finished, this, [this]() {
        ocrOutput->setPlainText(ocrWatcher.result());
        statusLabel->setText("OCR completed.");
    });

    connect(&aiWatcher, &QFutureWatcher<QString>::finished, this, [this]() {
        aiOutput->setPlainText(aiWatcher.result());
        statusLabel->setText("AI extraction completed.");
    });
}

void MainWindow::loadSettingsToUi() {
    ocrLanguageInput->setText(QString::fromStdString(settings.getOCRLanguage()));
    geminiApiKeyInput->setText(QString::fromStdString(settings.getGeminiApiKey()));
    geminiApiUrlInput->setText(QString::fromStdString(settings.getGeminiApiUrl()));
    ollamaIpInput->setText(QString::fromStdString(settings.getOllamaIP()));
    ollamaPortInput->setValue(settings.getOllamaPort());
    ollamaModelInput->setText(QString::fromStdString(settings.getOllamaModel()));
    ollamaKeepAliveInput->setValue(settings.getOllamaKeepAlive());

    QString defaultProvider = QString::fromStdString(settings.getDefaultAiProvider());
    int homeIndex = defaultProviderInput->findData(defaultProvider);
    if (homeIndex < 0) {
        homeIndex = 0;
    }
    defaultProviderInput->setCurrentIndex(homeIndex);

    int resultIndex = providerDropdown->findData(defaultProvider);
    if (resultIndex < 0) {
        resultIndex = 0;
    }
    providerDropdown->setCurrentIndex(resultIndex);
}

void MainWindow::saveSettingsFromUi() {
    settings.setOcrLanguage(ocrLanguageInput->text().trimmed().toStdString());
    settings.setGeminiApiKey(geminiApiKeyInput->text().toStdString());
    settings.setGeminiApiUrl(geminiApiUrlInput->text().trimmed().toStdString());
    settings.setOllamaIP(ollamaIpInput->text().trimmed().toStdString());
    settings.setOllamaPort(ollamaPortInput->value());
    settings.setOllamaModel(ollamaModelInput->text().trimmed().toStdString());
    settings.setOllamaKeepAlive(ollamaKeepAliveInput->value());
    settings.setDefaultAiProvider(defaultProviderInput->currentData().toString().toStdString());
    settings.saveSettings();

    int index = providerDropdown->findData(defaultProviderInput->currentData());
    if (index >= 0) {
        providerDropdown->setCurrentIndex(index);
    }
}

void MainWindow::setCurrentImagePath(const QString &imagePath) {
    currentImagePath = imagePath;
    currentImageLabel->setText("Image: " + currentImagePath);
}

bool MainWindow::hasValidCurrentImage() const {
    QFileInfo info(currentImagePath);
    return info.exists() && info.isFile();
}

QString MainWindow::selectedProvider() const {
    return providerDropdown->currentData().toString();
}

void MainWindow::runOcrForCurrentImage() {
    if (!hasValidCurrentImage()) {
        QMessageBox::warning(this, "Invalid image", "Please provide a valid image path.");
        return;
    }

    if (ocrWatcher.isRunning()) {
        return;
    }

    statusLabel->setText("Running OCR...");
    ocrOutput->setPlainText(QString());

    const QString image = currentImagePath;
    ocrWatcher.setFuture(QtConcurrent::run([image]() -> QString {
        try {
            std::string imageStd = image.toStdString();
            std::string ocr = getOCR(imageStd);
            return QString::fromStdString(ocr);
        } catch (const std::exception &e) {
            return QString::fromStdString("OCR failed: " + std::string(e.what()));
        }
    }));
}

void MainWindow::runAiForCurrentImage() {
    if (!hasValidCurrentImage()) {
        QMessageBox::warning(this, "Invalid image", "Please provide a valid image path.");
        return;
    }

    if (aiWatcher.isRunning()) {
        return;
    }

    statusLabel->setText("Running AI extraction...");
    aiOutput->setPlainText(QString());

    const QString image = currentImagePath;
    const QString provider = selectedProvider();

    aiWatcher.setFuture(QtConcurrent::run([image, provider]() -> QString {
        try {
            Settings workerSettings;
            std::string imageStd = image.toStdString();

            if (provider == "gemini") {
                GeminiClient gemini(workerSettings);
                return QString::fromStdString(gemini.textract(imageStd));
            }

            OllamaClient ollama(workerSettings);
            return QString::fromStdString(ollama.textract(imageStd));
        } catch (const std::exception &e) {
            return QString::fromStdString("AI extraction failed: " + std::string(e.what()));
        }
    }));
}
