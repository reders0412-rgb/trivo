#pragma once
#include <QString>
#include <QStringList>

class FileUtils
{
public:
    // Qt file dialog filter string
    static QString filterString();

    // All supported extensions (lowercase, no dot)
    static QStringList supportedExtensions();

    // Returns true if the file extension is supported
    static bool isSupported(const QString &path);

    // Returns a display name for the format
    static QString formatName(const QString &ext);

    // Human-readable file size
    static QString humanSize(qint64 bytes);
};
