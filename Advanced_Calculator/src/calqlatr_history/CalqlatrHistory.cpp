#include "CalqlatrHistory.h"

qint32 CalqlatrHistory::getIndex(qint32 id) const {
    if (id < 0 || id > historyRecords.last().id()) {
        return -1;
    }

    int left = 0;
    int right = historyRecords.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        qint32 midId = historyRecords.at(mid).id();

        if (midId == id) {
            return mid;
        } else if (midId < id) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

CalqlatrHistory::CalqlatrHistory() {
}

void CalqlatrHistory::writeTo(QDataStream &out) const {
    out << this->historyRecords;
}

void CalqlatrHistory::readFrom(QDataStream &in) {
    in >> this->historyRecords;
}

const QList<CalqlatrRecord> &CalqlatrHistory::records() const {
    return this->historyRecords;
}

const CalqlatrRecord &CalqlatrHistory::getRecord(qint32 id) const {
    qint32 index = getIndex(id);
    if (index < 0)
        return CalqlatrRecord::EMPTY;
    else
        return historyRecords.at(index);
}

void CalqlatrHistory::appendRecord(const CalqlatrRecord &rec) {
    historyRecords.append(rec);
}

void CalqlatrHistory::deleteRecord(qint32 id) {
    qint32 index = getIndex(id);
    if (index >= 0)
        historyRecords.remove(index);
}

QDataStream &operator<<(QDataStream &out, const CalqlatrHistory &history) {
    history.writeTo(out);
    return out;
}

QDataStream &operator>>(QDataStream &in, CalqlatrHistory &history) {
    history.readFrom(in);
    return in;
}
