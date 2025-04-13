#ifndef SCICALQLATR_H
#define SCICALQLATR_H

#include <QQueue>
#include <QObject>

#include "CalqlatrElement.h"
#include "CalqlatrRecord.h"
#include "CalculationObject.h"

typedef CalqlatrElement::Element Element;

class SciCalqlatr : public QObject {
    Q_OBJECT
private:
    enum AngleUnit { DEG,
        RAD };

    qint32 id_counter = 0;
    qint32 max_recent_records = 3;

    AngleUnit angle_unit = RAD;
    QList<Element> _currentExp = { Element::D_0 };
    QQueue<CalqlatrRecord> _recentRecords;

    /*
     *      Calculation
     */

    // Returns a CalculationObject according to cursor's position, updates position automatically
    static const CalculationObject getNextObject(const QList<Element> &expression, int &cursor, bool &ok);
    // Convert a element form expression into CalculationObject form
    static bool getCalObjectExp(QList<CalculationObject> &res, const QList<Element> &exp);
    // Try to insert omitted multiply operators
    static void tryFixExp(QScopedPointer<QList<CalculationObject>> &exp);
    // Covert provided infix expression into RPN(Reverse Polish Notation)
    static const bool convertToRPN(QList<CalculationObject> &res, const QList<CalculationObject> &exp);
    // Calculate the final result from RPN
    static double calculateRPN(const QList<CalculationObject> &rpn, bool &ok,
        const AngleUnit &unit = AngleUnit::RAD);
    // Double operand calculation
    static double _calculate(const CalculationObject &operation, const CalculationObject &num1,
        const CalculationObject &num2, bool &ok, const AngleUnit &unit = AngleUnit::RAD);
    // Single operand calculation
    static double _calculate(const CalculationObject &operation, const CalculationObject &num, bool &ok,
        const AngleUnit &unit = AngleUnit::RAD);

public:
    SciCalqlatr(QObject *parent);

    bool calculate();

    void setMaxRecentRecordsCount(int count);
    const bool isCurrentExpCleared() const;
    const bool isRecentRecordsEmpty() const;

    const QList<Element> &currentExp();
    const QQueue<CalqlatrRecord> recentRecords();

    void readFrom(QDataStream &in);
    void writeTo(QDataStream &out) const;

signals:
    void currentExpChanged();
    void recentRecordsChanged();
    void recordProduced(const CalqlatrRecord &rec);
    void inserted(int index, const Element e);

public slots:
    void switchAngleUnit();
    void clearCurrentExp();
    void clearRecentRecords();
    void setCurrentExp(const QList<Element> &exp);
    void loadRecord(const CalqlatrRecord &rec);
    void insertToExp(int index, const Element e);
    void removeFromExp(int index);
};

#endif // SCICALQLATR_H
