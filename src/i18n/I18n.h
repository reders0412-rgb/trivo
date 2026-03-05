#pragma once
#include <QString>
#include <QMap>
#include <QObject>

class I18n : public QObject
{
    Q_OBJECT
public:
    static I18n &instance();

    void    init();
    void    setLanguage(const QString &lang);
    QString language() const { return m_lang; }
    QString tr(const QString &key) const;

signals:
    void languageChanged(const QString &lang);

private:
    I18n() = default;
    void loadStrings();

    QString m_lang = "ko";
    QMap<QString, QMap<QString, QString>> m_strings; // lang -> key -> value
};

// Convenience macro
#define TR(key) I18n::instance().tr(key)
