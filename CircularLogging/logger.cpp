#include "logger.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>

Logger::Logger(QObject *parent)
    : QObject(parent), m_maxLogs(7), m_logInterval(1), m_logType("daily"), m_logDirectory("log")
{
}

bool Logger::init(const QString &configPath)
{
    loadConfig(configPath);

    QDir dir(m_logDirectory);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString logFileName = getCurrentLogFileName();
    m_logFile.setFileName(m_logDirectory + "/" + logFileName);
    if (!m_logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Cannot open log file:" << m_logFile.fileName();
        return false;
    }

    cleanOldLogs();
    return true;
}

void Logger::loadConfig(const QString &configPath)
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open config file:" << configPath;
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return;
    }

    QJsonObject obj = doc.object();

    m_logType = obj.value("log_type").toString("daily");
    m_logInterval = obj.value("log_interval").toInt(1);
    m_maxLogs = obj.value("max_logs").toInt(7);
    m_logDirectory = obj.value("log_directory").toString("log");

    if (m_logInterval <= 0)
        m_logInterval = 1;

    qDebug() << "Config loaded:"
             << "log_type =" << m_logType << "|"
             << "log_interval (number of intervals) =" << m_logInterval << "|"
             << "max_logs (max number of log files) =" << m_maxLogs << "|"
             << "log_directory =" << m_logDirectory;
}

QString Logger::getCurrentLogFileName() const
{
    QDateTime now = QDateTime::currentDateTimeUtc();

    if (m_logType == "daily") {
        int dayGroup = now.date().dayOfYear() / m_logInterval;
        QDate groupStart = QDate(now.date().year(), 1, 1).addDays(dayGroup * m_logInterval);
        QDate groupEnd = groupStart.addDays(m_logInterval - 1);
        return groupStart.toString("yyyy-MM-dd") + "_to_" + groupEnd.toString("yyyy-MM-dd") + ".log";
    }
    else if (m_logType == "hourly") {
        int hourGroup = now.time().hour() / m_logInterval;
        QTime startTime(hourGroup * m_logInterval, 0);
        QTime endTime = startTime.addSecs(3600 * m_logInterval - 1);
        return now.date().toString("yyyy-MM-dd") + "_" +
               startTime.toString("HH") + "_to_" + endTime.toString("HH") + ".log";
    }
    else if (m_logType == "minutely") {
        int minuteGroup = now.time().minute() / m_logInterval;
        QTime startTime(now.time().hour(), minuteGroup * m_logInterval);
        QTime endTime = startTime.addSecs(60 * m_logInterval - 1);
        return now.date().toString("yyyy-MM-dd") + "_" +
               startTime.toString("HH-mm") + "_to_" + endTime.toString("HH-mm") + ".log";
    }
    else if (m_logType == "secondly") {
        int secGroup = now.time().second() / m_logInterval;
        QTime startTime(now.time().hour(), now.time().minute(), secGroup * m_logInterval);
        QTime endTime = startTime.addSecs(m_logInterval - 1);
        return now.date().toString("yyyy-MM-dd") + "_" +
               startTime.toString("HH-mm-ss") + "_to_" + endTime.toString("HH-mm-ss") + ".log";
    }

    return now.toString("yyyy-MM-dd") + ".log";
}

void Logger::logMessage(const QString &message)
{
    if (!m_logFile.isOpen()) {
        if (!m_logFile.open(QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Cannot open log file for writing:" << m_logFile.fileName();
            return;
        }
    }

    QString timeStampedMsg = QDateTime::currentDateTimeUtc().toString(Qt::ISODate) + " - " + message + "\n";
    m_logFile.write(timeStampedMsg.toUtf8());
    m_logFile.flush();
}

void Logger::cleanOldLogs()
{
    QDir dir(m_logDirectory);
    QStringList filters;
    filters << "*.log";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time);

    if (files.size() <= m_maxLogs)
        return;

    while (files.size() > m_maxLogs) {
        QFileInfo oldestFile = files.last();
        qDebug() << "Deleting old log file:" << oldestFile.fileName();
        QFile::remove(oldestFile.absoluteFilePath());
        files.removeLast();
    }
}
