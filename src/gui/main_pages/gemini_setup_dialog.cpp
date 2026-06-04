#include "gemini_setup_dialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

GeminiSetupDialog::GeminiSetupDialog(Settings &settings, QWidget *parent)
    : QDialog(parent), settings(settings), apiUrlInput(nullptr), apiKeyInput(nullptr) {
    buildUi();
}

void GeminiSetupDialog::buildUi() {
    setWindowTitle("Gemini Setup");
    setModal(true);

    // Make the dialog a bit larger by default for better spacing
    resize(520, 260);

    auto *layout = new QVBoxLayout(this);

    auto *info = new QLabel("Enter your Gemini API key and confirm the endpoint URL.", this);
    info->setWordWrap(true);
    layout->addWidget(info);

    auto *form = new QFormLayout();
    apiUrlInput = new QLineEdit(this);
    apiUrlInput->setText(QString::fromStdString(settings.getGeminiApiUrl()));
    apiKeyInput = new QLineEdit(this);
    apiKeyInput->setEchoMode(QLineEdit::Password);
    apiKeyInput->setPlaceholderText("Required");

    form->addRow("API URL:", apiUrlInput);
    form->addRow("API Key:", apiKeyInput);
    layout->addLayout(form);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (saveSettings()) {
            accept();
        }
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttons);
}

bool GeminiSetupDialog::saveSettings() {
    const QString apiKey = apiKeyInput->text().trimmed();
    if (apiKey.isEmpty()) {
        QMessageBox::warning(this, "Gemini setup", "API key is required.");
        apiKeyInput->setFocus();
        return false;
    }

    settings.setGeminiApiUrl(apiUrlInput->text().trimmed().toStdString());
    settings.setGeminiApiKey(apiKey.toStdString());
    settings.setGeminiConfigured(true);
    settings.saveSettings();
    return true;
}