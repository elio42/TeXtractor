#pragma once

#include <QStackedWidget>
#include <QWidget>

#include "../core/settings.h"
#include "main_pages/home_page.h"
#include "main_pages/result_page.h"
#include "main_pages/settings_page.h"

class MainWindow : public QWidget {
public:
    explicit MainWindow(const QString &startupImagePath, QWidget *parent = nullptr);

private:
    void buildUi();
    void connectSignals();

    Settings settings;
    QStackedWidget *pages;

    HomePage *homePage;
    ResultPage *resultPage;
    SettingsPage *settingsPage;
};
