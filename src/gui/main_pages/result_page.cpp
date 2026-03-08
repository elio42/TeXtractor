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
    : QWidget(parent), settings(settings), currentImageLabel(nullptr), ocrOutput(nullptr),
      providerDropdown(nullptr), aiOutput(nullptr), statusLabel(nullptr) {
    buildUi();

    connect(&ocrWatcher, &QFutureWatcher<QString>::finished, this, [this]() {
        ocrOutput->setPlainText(ocrWatcher.result());
        statusLabel->setText("OCR completed.");
    });

    connect(&aiWatcher, &QFutureWatcher<QString>::finished, this, [this]() {
        aiOutput->setPlainText(aiWatcher.result());
        statusLabel->setText("AI extraction completed.");
    });
}

void ResultPage::buildUi() {
    auto *layout = new QVBoxLayout(this);

    // Top bar
    auto *topRow = new QHBoxLayout();
    auto *backButton = new QPushButton("⌂ Home", this);
    currentImageLabel = new QLabel("Image: (none)", this);

    auto *settingsButton = new QToolButton(this);
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
    auto *aiControls = new QHBoxLayout();
    providerDropdown = new QComboBox(this);
    providerDropdown->addItem("Ollama", "ollama");
    providerDropdown->addItem("Gemini", "gemini");

    auto *extractAiButton = new QPushButton("Extract with AI", this);
    aiControls->addWidget(new QLabel("Provider:", this));
    aiControls->addWidget(providerDropdown);
    aiControls->addWidget(extractAiButton);
    aiControls->addStretch(1);
    layout->addLayout(aiControls);

    // AI output section
    auto *aiTitle = new QLabel("AI Output", this);
    aiTitle->setFont(titleFont);

    aiOutput = new QTextEdit(this);
    aiOutput->setReadOnly(true);
    layout->addWidget(aiTitle);
    layout->addWidget(aiOutput, 1);

    // Status bar
    statusLabel = new QLabel("Ready", this);
    statusLabel->setStyleSheet("color: gray;");
    layout->addWidget(statusLabel);

    // Load default provider
    QString defaultProvider = QString::fromStdString(settings.getDefaultAiProvider());
    int index = providerDropdown->findData(defaultProvider);
    if (index >= 0) {
        providerDropdown->setCurrentIndex(index);
    }

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, [this]() {
        emit backRequested();
    });

    connect(extractAiButton, &QPushButton::clicked, this, [this]() {
        runAiExtraction();
    });

    connect(settingsButton, &QToolButton::clicked, this, [this]() {
        emit settingsRequested();
    });
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
