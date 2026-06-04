#include "settings_page.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace {
void addProviderActions(QFormLayout *form, QWidget *parent, QPushButton *&setupButton, QPushButton *&resetButton) {
    auto *actionsWidget = new QWidget(parent);
    auto *actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->addStretch(1);
    setupButton = new QPushButton("Setup", actionsWidget);
    resetButton = new QPushButton("Reset setup", actionsWidget);
    actionsLayout->addWidget(setupButton);
    actionsLayout->addWidget(resetButton);
    form->addRow("", actionsWidget);
}
}

SettingsPage::SettingsPage(Settings &settings, QWidget *parent)
        : QWidget(parent), settings(settings), ocrBox(nullptr), aiBox(nullptr), geminiBox(nullptr), ollamaBox(nullptr),
            ocrLanguageDropdown(nullptr), ocrLanguageCustomInput(nullptr),
      defaultProviderInput(nullptr), geminiApiKeyInput(nullptr), geminiApiUrlInput(nullptr),
      ollamaIpInput(nullptr), ollamaPortInput(nullptr), ollamaModelInput(nullptr),
      ollamaKeepAliveInput(nullptr) {
    buildUi();
    loadSettings();
}

void SettingsPage::buildUi() {
    auto *layout = new QVBoxLayout(this);

    // Top bar
    auto *topBar = new QHBoxLayout();
    auto *backButton = new QPushButton("⌂ Home", this);
    auto *title = new QLabel("Settings", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    title->setFont(titleFont);

    topBar->addWidget(backButton);
    topBar->addWidget(title, 1);
    layout->addLayout(topBar);

    layout->addSpacing(20);

    // Create scroll area
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);

    // Create centered content container
    auto *contentContainer = new QWidget();
    contentContainer->setMaximumWidth(600);
    auto *contentLayout = new QVBoxLayout(contentContainer);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    // OCR Settings
    ocrBox = new QGroupBox("OCR Settings", contentContainer);
    auto *ocrForm = new QFormLayout(ocrBox);

    ocrLanguageDropdown = new QComboBox(ocrBox);
    // European languages
    ocrLanguageDropdown->addItem("English", "eng");
    ocrLanguageDropdown->addItem("German (Deutsch)", "deu");
    ocrLanguageDropdown->addItem("French (Français)", "fra");
    ocrLanguageDropdown->addItem("Spanish (Español)", "spa");
    ocrLanguageDropdown->addItem("Italian (Italiano)", "ita");
    ocrLanguageDropdown->addItem("Portuguese (Português)", "por");
    ocrLanguageDropdown->addItem("Dutch (Nederlands)", "nld");
    ocrLanguageDropdown->addItem("Polish (Polski)", "pol");
    ocrLanguageDropdown->addItem("Swedish (Svenska)", "swe");
    ocrLanguageDropdown->addItem("Norwegian (Norsk)", "nor");
    ocrLanguageDropdown->addItem("Danish (Dansk)", "dan");
    ocrLanguageDropdown->addItem("Finnish (Suomi)", "fin");
    ocrLanguageDropdown->addItem("Czech (Čeština)", "ces");
    ocrLanguageDropdown->addItem("Romanian (Română)", "ron");
    ocrLanguageDropdown->addItem("Hungarian (Magyar)", "hun");
    ocrLanguageDropdown->addItem("Greek (Ελληνικά)", "ell");
    ocrLanguageDropdown->addItem("Russian (Русский)", "rus");
    ocrLanguageDropdown->addItem("Ukrainian (Українська)", "ukr");
    // Major non-European languages
    ocrLanguageDropdown->addItem("Chinese Simplified (简体中文)", "chi_sim");
    ocrLanguageDropdown->addItem("Chinese Traditional (繁體中文)", "chi_tra");
    ocrLanguageDropdown->addItem("Japanese (日本語)", "jpn");
    ocrLanguageDropdown->addItem("Korean (한국어)", "kor");
    ocrLanguageDropdown->addItem("Arabic (العربية)", "ara");
    ocrLanguageDropdown->addItem("Hindi (हिन्दी)", "hin");
    ocrLanguageDropdown->addItem("Other (custom code)", "other");

    ocrLanguageCustomInput = new QLineEdit(ocrBox);
    ocrLanguageCustomInput->setPlaceholderText("Enter custom language code (e.g., 'lat' for Latin)");
    ocrLanguageCustomInput->setVisible(false);

    ocrForm->addRow("Language:", ocrLanguageDropdown);
    ocrForm->addRow("", ocrLanguageCustomInput);

    contentLayout->addWidget(ocrBox);

    // AI Provider Settings
    aiBox = new QGroupBox("AI Provider Settings", contentContainer);
    auto *aiForm = new QFormLayout(aiBox);

    defaultProviderInput = new QComboBox(aiBox);
    aiForm->addRow("Default Provider:", defaultProviderInput);

    contentLayout->addWidget(aiBox);

    // Gemini Settings
    geminiBox = new QGroupBox("Gemini Settings", contentContainer);
    auto *geminiForm = new QFormLayout(geminiBox);

    geminiApiKeyInput = new QLineEdit(geminiBox);
    geminiApiKeyInput->setEchoMode(QLineEdit::Password);
    geminiApiUrlInput = new QLineEdit(geminiBox);

    geminiForm->addRow("API Key:", geminiApiKeyInput);
    geminiForm->addRow("API URL:", geminiApiUrlInput);

    addProviderActions(geminiForm, geminiBox, setupGeminiButton, resetGeminiButton);

    contentLayout->addWidget(geminiBox);

    // Ollama Settings
    ollamaBox = new QGroupBox("Ollama Settings", contentContainer);
    auto *ollamaForm = new QFormLayout(ollamaBox);

    ollamaIpInput = new QLineEdit(ollamaBox);
    ollamaPortInput = new QSpinBox(ollamaBox);
    ollamaPortInput->setRange(1, 65535);
    ollamaModelInput = new QLineEdit(ollamaBox);
    ollamaKeepAliveInput = new QSpinBox(ollamaBox);
    ollamaKeepAliveInput->setRange(0, 3600);

    ollamaForm->addRow("IP Address:", ollamaIpInput);
    ollamaForm->addRow("Port:", ollamaPortInput);
    ollamaForm->addRow("Model:", ollamaModelInput);
    ollamaForm->addRow("Keep Alive (seconds):", ollamaKeepAliveInput);

    addProviderActions(ollamaForm, ollamaBox, setupOllamaButton, resetOllamaButton);

    contentLayout->addWidget(ollamaBox);

    // Action buttons
    auto *actions = new QHBoxLayout();
    auto *saveButton = new QPushButton("Save Settings", contentContainer);
    saveButton->setDefault(true);

    actions->addStretch(1);
    actions->addWidget(saveButton);
    contentLayout->addLayout(actions);

    contentLayout->addStretch(1);

    // Add container to scroll area and center it
    scrollArea->setWidget(contentContainer);
    scrollArea->setAlignment(Qt::AlignHCenter);
    layout->addWidget(scrollArea);

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, [this]() {
        emit backRequested();
    });

    connect(saveButton, &QPushButton::clicked, this, [this]() {
        saveSettings();
        QMessageBox::information(this, "Settings", "Settings saved successfully.");
    });

    // Setup/Reset button handlers
    connect(setupGeminiButton, &QPushButton::clicked, this, [this]() {
        emit geminiSetupRequested();
    });

    connect(resetGeminiButton, &QPushButton::clicked, this, [this]() {
        auto reply = QMessageBox::question(this, "Reset Gemini", "Reset Gemini configuration?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            settings.resetAllGeminiSettings();
            settings.saveSettings();
            emit settingsChanged();
            loadSettings();
        }
    });

    connect(setupOllamaButton, &QPushButton::clicked, this, [this]() {
        emit ollamaSetupRequested();
    });

    connect(resetOllamaButton, &QPushButton::clicked, this, [this]() {
        auto reply = QMessageBox::question(this, "Reset Ollama", "Reset Ollama configuration?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            settings.resetAllOllamaSettings();
            settings.saveSettings();
            emit settingsChanged();
            loadSettings();
        }
    });

    // Show/hide custom language input based on dropdown selection
    connect(ocrLanguageDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        bool isOther = ocrLanguageDropdown->currentData().toString() == "other";
        ocrLanguageCustomInput->setVisible(isOther);
    });

    updateProviderVisibility();
    rebuildDefaultProviderOptions();
}

void SettingsPage::updateProviderVisibility() {
    const bool ollamaConfigured = settings.getOllamaConfigured();
    const bool geminiConfigured = settings.getGeminiConfigured();
    const bool anyProviderConfigured = settings.getAnyProviderConfigured();

    aiBox->setVisible(anyProviderConfigured);

    // Always show provider boxes so users can setup providers from here.
    ollamaBox->setVisible(true);
    geminiBox->setVisible(true);

    // Enable/disable provider-specific inputs based on configuration
    geminiApiKeyInput->setEnabled(geminiConfigured);
    geminiApiUrlInput->setEnabled(geminiConfigured);

    ollamaIpInput->setEnabled(ollamaConfigured);
    ollamaPortInput->setEnabled(ollamaConfigured);
    ollamaModelInput->setEnabled(ollamaConfigured);
    ollamaKeepAliveInput->setEnabled(ollamaConfigured);

    // Show appropriate action buttons
    if (setupGeminiButton && resetGeminiButton) {
        setupGeminiButton->setVisible(!geminiConfigured);
        resetGeminiButton->setVisible(geminiConfigured);
    }
    if (setupOllamaButton && resetOllamaButton) {
        setupOllamaButton->setVisible(!ollamaConfigured);
        resetOllamaButton->setVisible(ollamaConfigured);
    }
}

void SettingsPage::rebuildDefaultProviderOptions() {
    const QString currentProvider = defaultProviderInput->currentData().toString();

    defaultProviderInput->clear();
    if (settings.getOllamaConfigured()) {
        defaultProviderInput->addItem("Ollama", "ollama");
    }
    if (settings.getGeminiConfigured()) {
        defaultProviderInput->addItem("Gemini", "gemini");
    }

    if (defaultProviderInput->count() == 0) {
        return;
    }

    QString defaultProvider = QString::fromStdString(settings.getDefaultAiProvider());
    int index = defaultProviderInput->findData(defaultProvider);
    if (index < 0 && !currentProvider.isEmpty()) {
        index = defaultProviderInput->findData(currentProvider);
    }
    if (index < 0) {
        index = 0;
    }
    defaultProviderInput->setCurrentIndex(index);
}

void SettingsPage::loadSettings() {
    updateProviderVisibility();

    QString currentLang = QString::fromStdString(settings.getOCRLanguage());
    int langIndex = ocrLanguageDropdown->findData(currentLang);
    if (langIndex >= 0) {
        ocrLanguageDropdown->setCurrentIndex(langIndex);
        ocrLanguageCustomInput->clear();
    } else {
        // Set to "Other" and populate custom input
        int otherIndex = ocrLanguageDropdown->findData("other");
        if (otherIndex >= 0) {
            ocrLanguageDropdown->setCurrentIndex(otherIndex);
        }
        ocrLanguageCustomInput->setText(currentLang);
    }

    if (settings.getOllamaConfigured()) {
        ollamaIpInput->setText(QString::fromStdString(settings.getOllamaIP()));
        ollamaPortInput->setValue(settings.getOllamaPort());
        ollamaModelInput->setText(QString::fromStdString(settings.getOllamaModel()));
        ollamaKeepAliveInput->setValue(settings.getOllamaKeepAlive());
    }

    if (settings.getGeminiConfigured()) {
        geminiApiKeyInput->setText(QString::fromStdString(settings.getGeminiApiKey()));
        geminiApiUrlInput->setText(QString::fromStdString(settings.getGeminiApiUrl()));
    }

    rebuildDefaultProviderOptions();
}

void SettingsPage::saveSettings() {
    QString langCode;
    if (ocrLanguageDropdown->currentData().toString() == "other") {
        langCode = ocrLanguageCustomInput->text().trimmed();
    } else {
        langCode = ocrLanguageDropdown->currentData().toString();
    }
    settings.setOcrLanguage(langCode.toStdString());

    if (settings.getOllamaConfigured()) {
        settings.setOllamaIP(ollamaIpInput->text().trimmed().toStdString());
        settings.setOllamaPort(ollamaPortInput->value());
        settings.setOllamaModel(ollamaModelInput->text().trimmed().toStdString());
        settings.setOllamaKeepAlive(ollamaKeepAliveInput->value());
    }

    if (settings.getGeminiConfigured()) {
        settings.setGeminiApiKey(geminiApiKeyInput->text().trimmed().toStdString());
        settings.setGeminiApiUrl(geminiApiUrlInput->text().trimmed().toStdString());
    }

    if (settings.getAnyProviderConfigured() && defaultProviderInput->count() > 0) {
        settings.setDefaultAiProvider(defaultProviderInput->currentData().toString().toStdString());
    }

    settings.saveSettings();
}
