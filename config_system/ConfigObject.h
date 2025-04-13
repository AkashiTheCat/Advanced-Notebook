#ifndef CONFIGOBJECT_H
#define CONFIGOBJECT_H

#include <QString>
#include <QJsonObject>

#include "ConfigFile.h"

template <class T>
class ConfigObject {
private:
    QJsonObject &obj;

    QString key;
    T defaultValue;

public:
    ConfigObject(QJsonObject &obj, const QString &key, const T &defaultValue = 0);
    ConfigObject(ConfigFile &obj, const QString &key, const T &defaultValue = 0);
    T value();
    void setValue(const T &value);
    QString toString();
};

template <class T>
ConfigObject<T>::ConfigObject(QJsonObject &obj, const QString &key, const T &defaultValue)
    : key(key), obj(obj), defaultValue(defaultValue) {}

template <class T>
ConfigObject<T>::ConfigObject(ConfigFile &file, const QString &key, const T &defaultValue)
    : ConfigObject(file.obj, key, defaultValue) {}

template <class T>
T ConfigObject<T>::value() {
    QJsonValue value = obj.value(key);
    if (value.isUndefined()) {
        setValue(defaultValue);
        return defaultValue;
    }
    return value.toVariant().value<T>();
}

template <class T>
void ConfigObject<T>::setValue(const T &value) {
    obj.insert(key, QJsonValue::fromVariant(QVariant::fromValue(value)));
}

template <class T>
inline QString ConfigObject<T>::toString() {
    QJsonValue value = obj.value(key);
    if (value.isUndefined()) {
        setValue(defaultValue);
        value = QJsonValue(defaultValue);
    }
    return value.toVariant().value<QString>();
}

#endif // CONFIGOBJECT_H
