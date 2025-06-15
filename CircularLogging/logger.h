#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QJsonObject>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);
    bool init(const QString &configPath);
    void logMessage(const QString &message);

private:
    QString getCurrentLogFileName() const;
    void cleanOldLogs();
    void loadConfig(const QString &configPath);

    QFile m_logFile;
    QString m_logDirectory;
    QString m_logType;     // "daily", "hourly", "minutely", "secondly"
    int m_logInterval;     // Kaç zaman biriminde bir log dosyası oluşturulacak
    int m_maxLogs;
};

#endif // LOGGER_H
