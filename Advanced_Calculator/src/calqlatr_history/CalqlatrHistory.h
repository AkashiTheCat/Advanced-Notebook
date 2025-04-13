#ifndef CALQLATRHISTORY_H
#define CALQLATRHISTORY_H

#include <QObject>
#include "CalqlatrRecord.h"

class CalqlatrHistory {
private:
    QList<CalqlatrRecord> historyRecords;

public:
    CalqlatrHistory();

    void writeTo(QDataStream &out) const;
    void readFrom(QDataStream &in);

    const QList<CalqlatrRecord> &records() const;
    qint32 getIndex(qint32 id) const;
    const CalqlatrRecord &getRecord(qint32 id) const;
    void appendRecord(const CalqlatrRecord &rec);
    void deleteRecord(qint32 id);
};

QDataStream &operator<<(QDataStream &out, const CalqlatrHistory &history);
QDataStream &operator>>(QDataStream &in, CalqlatrHistory &history);

#endif // CALQLATRHISTORY_H
