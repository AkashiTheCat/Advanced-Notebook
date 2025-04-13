#ifndef CALQLATRRECORD_H
#define CALQLATRRECORD_H
#include <QFile>

#include "CalqlatrElement.h"

typedef CalqlatrElement::Element Element;

class CalqlatrRecord {
private:
    QList<Element> expression;
    double result;
    QString text;
    qint32 rec_id = -1;

public:
    const static CalqlatrRecord EMPTY;

    CalqlatrRecord(const QList<Element> &expression, const double &result, const QString &text);
    CalqlatrRecord(const QList<Element> &expression, const double &result);
    CalqlatrRecord();

    bool isValid() const;

    const QList<Element> &getExpression() const;
    const double &getResult() const;
    const QString &getText() const;
    const qint32 id() const;
    void setId(qint32 id);

    const bool writeTo(QDataStream &stream) const;
    const bool readFrom(QDataStream &stream);
};

Q_DECLARE_METATYPE(CalqlatrRecord);

QDataStream &operator<<(QDataStream &out, CalqlatrRecord const &record);
QDataStream &operator>>(QDataStream &in, CalqlatrRecord &record);

#endif // CALQLATRRECORD_H
