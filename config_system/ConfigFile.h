#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QFile>
#include <QJsonObject>

class ConfigFile {
private:
    QFile file;
public:
    ConfigFile(const QString &fileName);
    ~ConfigFile();
    QJsonObject obj;
};

#endif // CONFIGFILE_H
