#include "CalqlatrRecord.h"

const CalqlatrRecord CalqlatrRecord::EMPTY = []() -> CalqlatrRecord {
    CalqlatrRecord rec;
    rec.setId(-1);
    return rec;
}();

CalqlatrRecord::CalqlatrRecord(const QList<Element> &expression, const double &result, const QString &text)
    : expression(expression), result(result) {
    this->text = text + " = " + QString::number(result);
}

CalqlatrRecord::CalqlatrRecord(const QList<Element> &expression, const double &result)
    : expression(expression), result(result) {
    this->text = CalqlatrElement::convertElementListToString(expression) + " = " + QString::number(result);
}

CalqlatrRecord::CalqlatrRecord() {
}

bool CalqlatrRecord::isValid() const {
    return !this->expression.empty();
}

const QList<Element> &CalqlatrRecord::getExpression() const {
    return this->expression;
}

const double &CalqlatrRecord::getResult() const {
    return this->result;
}

const QString &CalqlatrRecord::getText() const {
    return this->text;
}

const qint32 CalqlatrRecord::id() const {
    return rec_id;
}

void CalqlatrRecord::setId(qint32 id) {
    this->rec_id = id;
}

const bool CalqlatrRecord::writeTo(QDataStream &stream) const {
    stream << expression << result << text << rec_id;
    return stream.status() == QDataStream::Ok;
}

const bool CalqlatrRecord::readFrom(QDataStream &stream) {
    this->expression.clear();
    stream >> expression >> result >> text >> rec_id;
    return stream.status() == QDataStream::Ok;
}


QDataStream &operator<<(QDataStream &out, const CalqlatrRecord &record) {
    record.writeTo(out);
    return out;
}

QDataStream &operator>>(QDataStream &in, CalqlatrRecord &record) {
    record.readFrom(in);
    return in;
}
