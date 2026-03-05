#pragma once
#include <QIcon>
#include <QString>

class IconProvider
{
public:
    // Returns an appropriate icon for the file extension
    static QIcon forExtension(const QString &ext);
    static QIcon forExtensionPath(const QString &filePath);

    // App icons
    static QIcon appIcon();
    static QIcon openIcon();
    static QIcon addIcon();
    static QIcon resetIcon();
    static QIcon playIcon();
    static QIcon pauseIcon();
    static QIcon screenshotIcon();
    static QIcon darkThemeIcon();
    static QIcon lightThemeIcon();
};
