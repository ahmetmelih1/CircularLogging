#include <QCoreApplication>
#include "Logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Logger logger;
    if (!logger.init("config.json")) {
        qWarning() << "Logger initialization failed.";
        return -1;
    }

    logger.logMessage("Logger initialized successfully.");
    logger.logMessage("This is a test log message.");
    logger.logMessage("Application finished.");

    return 0;
}
