#include "result_page.h"

#include <QFileInfo>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>

#include "../../ai-recognition/gemini_client.h"
#include "../../ai-recognition/ollama_client.h"
#include "../../ocr/ocr.h"

ResultPage::ResultPage(Settings &settings, QWidget *parent)
    : QWidget(parent), settings(settings), currentImageLabel(nullptr), ocrOutput(nullptr), aiControlsStack(nullptr),
        configuredAiControls(nullptr), setupAiControls(nullptr), providerDropdown(nullptr), extractAiButton(nullptr),
        setupOllamaButton(nullptr), setupGeminiButton(nullptr), aiOutput(nullptr), statusLabel(nullptr),
        settingsButton(nullptr) {
    buildUi();

    connect(&ocrWatcher, &QFutureWatcher<QString>::finished, this, [this]() {
        ocrOutput->setPlainText(ocrWatcher.result());
        statusLabel->setText("OCR completed.");
    });

    connect(&aiWatcher, &QFutureWatcher<QString>::finished, this, [this]() {
        aiOutput->setPlainText(aiWatcher.result());
        statusLabel->setText("AI extraction completed.");
    });

    refreshAiSection();
}

void ResultPage::refreshAiSection() {
    rebuildProviderDropdown();
    updateAiSectionVisibility();
}

void ResultPage::buildUi() {
    auto *layout = new QVBoxLayout(this);

    // Top bar
    auto *topRow = new QHBoxLayout();
    auto *backButton = new QPushButton("⌂ Home", this);
    currentImageLabel = new QLabel("Image: (none)", this);

    settingsButton = new QToolButton(this);
    settingsButton->setText("⚙");
    settingsButton->setStyleSheet(
        "QToolButton { font-size: 20px; border: none; }"
        "QToolButton:hover { background-color: #7d7d7d; border-radius: 4px; }"
    );
    settingsButton->setToolTip("Settings");

    topRow->addWidget(backButton);
    topRow->addWidget(currentImageLabel, 1);
    topRow->addWidget(settingsButton);
    layout->addLayout(topRow);

    // OCR section
    auto *ocrTitle = new QLabel("OCR Output", this);
    QFont titleFont = ocrTitle->font();
    titleFont.setBold(true);
    ocrTitle->setFont(titleFont);

    ocrOutput = new QTextEdit(this);
    ocrOutput->setReadOnly(true);
    layout->addWidget(ocrTitle);
    layout->addWidget(ocrOutput, 1);

    // AI controls
    aiControlsStack = new QStackedWidget(this);

    configuredAiControls = new QWidget(this);
    auto *configuredLayout = new QVBoxLayout(configuredAiControls);
    auto *providerRow = new QHBoxLayout();
    providerDropdown = new QComboBox(configuredAiControls);
    extractAiButton = new QPushButton("Extract with AI", configuredAiControls);
    providerRow->addWidget(new QLabel("Provider:", configuredAiControls));
    providerRow->addWidget(providerDropdown);
    providerRow->addWidget(extractAiButton);
    providerRow->addStretch(1);

    aiOutput = new QTextEdit(configuredAiControls);
    aiOutput->setReadOnly(true);

    statusLabel = new QLabel("Ready", configuredAiControls);
    statusLabel->setStyleSheet("color: gray;");

    configuredLayout->addLayout(providerRow);
    configuredLayout->addWidget(aiOutput, 1);
    configuredLayout->addWidget(statusLabel);

    setupAiControls = new QWidget(this);
    auto *setupLayout = new QVBoxLayout(setupAiControls);
    auto *setupInfo = new QLabel("No AI provider is configured yet. Choose one setup path below.", setupAiControls);
    setupInfo->setWordWrap(true);
    auto *setupButtons = new QHBoxLayout();
    setupOllamaButton = new QPushButton("Setup Ollama", setupAiControls);
    setupGeminiButton = new QPushButton("Setup Gemini", setupAiControls);
    setupButtons->addWidget(setupOllamaButton);
    setupButtons->addWidget(setupGeminiButton);
    setupButtons->addStretch(1);
    setupLayout->addWidget(setupInfo);
    setupLayout->addLayout(setupButtons);
    setupLayout->addStretch(1);

    aiControlsStack->addWidget(configuredAiControls);
    aiControlsStack->addWidget(setupAiControls);
    layout->addWidget(aiControlsStack);

    // Load default provider
    rebuildProviderDropdown();

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, [this]() {
        emit backRequested();
    });

    connect(extractAiButton, &QPushButton::clicked, this, [this]() {
        runAiExtraction();
    });

    connect(setupOllamaButton, &QPushButton::clicked, this, [this]() {
        emit ollamaSetupRequested();
    });

    connect(setupGeminiButton, &QPushButton::clicked, this, [this]() {
        emit geminiSetupRequested();
    });

    connect(settingsButton, &QToolButton::clicked, this, [this]() {
        emit settingsRequested();
    });

    refreshAiSection();
}

void ResultPage::rebuildProviderDropdown() {
    const QString currentProvider = providerDropdown ? providerDropdown->currentData().toString() : QString();
    providerDropdown->clear();

    if (settings.getOllamaConfigured()) {
        providerDropdown->addItem("Ollama", "ollama");
    }

    if (settings.getGeminiConfigured()) {
        providerDropdown->addItem("Gemini", "gemini");
    }

    if (providerDropdown->count() == 0) {
        return;
    }

    QString defaultProvider = QString::fromStdString(settings.getDefaultAiProvider());
    int index = providerDropdown->findData(defaultProvider);
    if (index < 0 && !currentProvider.isEmpty()) {
        index = providerDropdown->findData(currentProvider);
    }
    if (index < 0) {
        index = 0;
    }
    providerDropdown->setCurrentIndex(index);
}

void ResultPage::updateAiSectionVisibility() {
    const bool anyProviderConfigured = settings.getAnyProviderConfigured();
    aiControlsStack->setCurrentIndex(anyProviderConfigured ? 0 : 1);
    configuredAiControls->setVisible(anyProviderConfigured);
    setupAiControls->setVisible(!anyProviderConfigured);
    if (extractAiButton) {
        extractAiButton->setEnabled(anyProviderConfigured);
    }
}

void ResultPage::setImagePath(const QString &imagePath) {
    currentImagePath = imagePath;
    currentImageLabel->setText("Image: " + currentImagePath);
}

void ResultPage::runOcr() {
    if (!hasValidImage()) {
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

void ResultPage::runAiExtraction() {
    if (!hasValidImage()) {
        QMessageBox::warning(this, "Invalid image", "Please provide a valid image path.");
        return;
    }

    if (!settings.getAnyProviderConfigured()) {
        QMessageBox::information(this, "No provider configured", "Set up an AI provider before extracting text with AI.");
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

bool ResultPage::hasValidImage() const {
    QFileInfo info(currentImagePath);
    return info.exists() && info.isFile();
}

QString ResultPage::selectedProvider() const {
    return providerDropdown->currentData().toString();
}
