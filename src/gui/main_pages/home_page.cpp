#include "home_page.h"

#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

HomePage::HomePage(QWidget *parent) : QWidget(parent), imagePathInput(nullptr) {
    buildUi();
}

void HomePage::buildUi() {
    auto *layout = new QVBoxLayout(this);

    // Top bar with settings button
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel("TeXtractor", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    title->setFont(titleFont);

    auto *settingsButton = new QToolButton(this);
    settingsButton->setText("⚙");
    settingsButton->setStyleSheet(
        "QToolButton { font-size: 20px; border: none; }"
        "QToolButton:hover { background-color: #7d7d7d; border-radius: 4px; }"
    );
    settingsButton->setToolTip("Settings");

    topBar->addWidget(title);
    topBar->addStretch(1);
    topBar->addWidget(settingsButton);
    layout->addLayout(topBar);

    // Subtitle
    auto *subtitle = new QLabel("Extract text from images using OCR and AI", this);
    subtitle->setStyleSheet("color: gray;");
    layout->addWidget(subtitle);

    layout->addSpacing(40);

    // Image input section
    auto *openBox = new QGroupBox("Open Image", this);
    auto *openLayout = new QHBoxLayout(openBox);

    imagePathInput = new QLineEdit(openBox);
    imagePathInput->setPlaceholderText("/path/to/image.png");

    auto *browseButton = new QPushButton("Browse", openBox);
    auto *analyzeButton = new QPushButton("Analyze Image", openBox);
    analyzeButton->setDefault(true);

    openLayout->addWidget(imagePathInput, 1);
    openLayout->addWidget(browseButton);
    openLayout->addWidget(analyzeButton);

    layout->addWidget(openBox);
    layout->addStretch(1);

    // Connect signals
    connect(browseButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this, "Select Image", QString(), "Images (*.png *.jpg *.jpeg *.bmp *.tif *.tiff)");
        if (!filePath.isEmpty()) {
            imagePathInput->setText(filePath);
        }
    });

    connect(analyzeButton, &QPushButton::clicked, this, [this]() {
        QString path = imagePathInput->text().trimmed();
        if (!path.isEmpty()) {
            emit openImageRequested(path);
        }
    });

    connect(settingsButton, &QToolButton::clicked, this, [this]() {
        emit settingsRequested();
    });
}
