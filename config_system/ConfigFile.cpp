#include <QJsonParseError>

#include "ConfigFile.h"

ConfigFile::ConfigFile(const QString &fileName) : file(QFile(fileName)) {
    if (!this->file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << this->file.fileName();
        this->obj = QJsonObject();
        return;
    }

    QByteArray data = this->file.readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        this->file.rename(this->file.fileName() + ".bak");
        qWarning() << "Error occured during json parsing";
        this->obj = QJsonObject();
        return;
    }

    file.close();
    this->obj = doc.object();
}

ConfigFile::~ConfigFile() {
    QByteArray data = QJsonDocument(this->obj).toJson();
    if (!this->file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open" << this->file.fileName();
        return;
    }
    this->file.resize(data.size());
    this->file.write(data);
    this->file.close();
}
