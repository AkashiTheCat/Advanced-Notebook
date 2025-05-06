#include <QStack>

#include "SciCalqlatr.h"

SciCalqlatr::SciCalqlatr(QObject *parent) : QObject(parent) {}

const QList<Element> &SciCalqlatr::currentExp() { return _currentExp; }

const QQueue<CalqlatrRecord>& SciCalqlatr::recentRecords() { return _recentRecords; }

void SciCalqlatr::switchAngleUnit() {
    this->angle_unit = (angle_unit == AngleUnit::DEG ? AngleUnit::RAD : AngleUnit::DEG);
}

void SciCalqlatr::setMaxRecentRecordsCount(int count) { this->max_recent_records = count; }

const bool SciCalqlatr::isCurrentExpCleared() const {
    return _currentExp.length() == 1 && _currentExp.at(0) == Element::D_0;
}

const bool SciCalqlatr::isRecentRecordsEmpty() const { return _recentRecords.empty(); }

void SciCalqlatr::clearCurrentExp() {
    _currentExp.clear();
    _currentExp.append(Element::D_0);
    emit currentExpChanged();
}

void SciCalqlatr::clearRecentRecords() {
    _recentRecords.clear();
    emit recentRecordsChanged();
}

void SciCalqlatr::setCurrentExp(const QList<Element> &exp) {
    _currentExp = exp;
    emit currentExpChanged();
}

void SciCalqlatr::loadRecord(const CalqlatrRecord &rec) {
    setCurrentExp(rec.getExpression());
}


void SciCalqlatr::readFrom(QDataStream &in) {
    in >> this->id_counter >> this->angle_unit >> this->_currentExp >> this->_recentRecords;
}

void SciCalqlatr::writeTo(QDataStream &out) const {
    out << this->id_counter << this->angle_unit << this->_currentExp << this->_recentRecords;
}


void SciCalqlatr::insertToExp(int index, const Element e) {
    // If Expression is Infinity, replace with 0
    if (_currentExp.length() == 1 && _currentExp.at(0) == Element::INF) {
        _currentExp.replace(0, Element::D_0);
    }

    // e is an operator
    if (CalqlatrElement::isOperator(e)) {
        const bool isSubtract = e == Element::O_SUBTRACT;
        // If Expression is a single 0 and e is subtract, check for replacement
        if (isSubtract && index == 1 && isCurrentExpCleared()) {
            _currentExp.replace(0, e);
            emit inserted(0, e);
            emit currentExpChanged();
            return;
        }

        // Cursor at very front or have left bracket ahead, insert additional 0
        if (index == 0 || _currentExp.at(index - 1) == Element::BRACKET_L ||
            CalqlatrElement::isFunction(_currentExp.at(index - 1))) {
            _currentExp.insert(index, e);
            if (!isSubtract) {
                _currentExp.insert(index, Element::D_0);
                index += 1;
            }
            emit inserted(index, e);
            emit currentExpChanged();
            return;
        }

        // Element before cursor is operator, check for replacement
        const Element &pre = _currentExp.at(index - 1);
        if (CalqlatrElement::isOperator(pre)) {
            // Elements are same, do nothing
            if (e == pre)
                return;

            // e is subtract, add bracket ahead
            if (isSubtract) {
                _currentExp.insert(index, e);
                _currentExp.insert(index, Element::BRACKET_L);
                emit inserted(index + 1, e);
                emit currentExpChanged();
                return;
            }

            _currentExp.replace(index - 1, e);
            emit inserted(index - 1, e);
            emit currentExpChanged();
            return;
        }

        // None match, insert directly
        _currentExp.insert(index, e);
        emit inserted(index, e);
        emit currentExpChanged();
        return;
    }

    // e is Digit, Bracket or Function
    // If Expression is a single 0 and the cursor is at the very end, check for replacement
    if (index == 1 && isCurrentExpCleared()) {
        // Element not 0, replace
        if (e != Element::D_0) {
            _currentExp.replace(0, e);
            emit inserted(0, e);
            emit currentExpChanged();
        }
        return;
    }

    // None match, insert directly
    _currentExp.insert(index, e);
    emit inserted(index, e);
    emit currentExpChanged();
    return;
}

void SciCalqlatr::removeFromExp(int index) {
    if (index < 0 || index > _currentExp.length()) {
        return;
    }

    _currentExp.remove(index);
    emit currentExpChanged();
}

/*
 *      Calculation
 */
void printCalObjList(QList<CalculationObject> &list);

bool SciCalqlatr::calculate() {
    QScopedPointer<QList<CalculationObject>> list(new QList<CalculationObject>);
    if (!getCalObjectExp(*list, _currentExp)) {
        qInfo() << "Error occured during Calculation Object Expression converting";
        return false;
    }
    tryFixExp(list);
    qInfo() << "Fixed expression:";
    printCalObjList(*list);

    QList<CalculationObject> rpn;
    if (!convertToRPN(rpn, *list)) {
        qInfo() << "Error occured during RPN converting";
        return false;
    }
    qInfo() << "RPN:";
    printCalObjList(rpn);

    bool ok;
    double res = calculateRPN(rpn, ok, this->angle_unit);
    if (!ok) {
        qInfo() << "Error occured during calculation";
        return false;
    }

    CalqlatrRecord rec(this->_currentExp, res);
    rec.setId(id_counter++);
    emit recordProduced(rec);

    _recentRecords.enqueue(rec);
    if (_recentRecords.length() > max_recent_records)
        _recentRecords.dequeue();
    emit recentRecordsChanged();

    CalqlatrElement::convertNumToElementList(_currentExp, res);
    emit currentExpChanged();

    return true;
}

const CalculationObject SciCalqlatr::getNextObject(const QList<Element> &expression, int &cursor, bool &ok) {
    ok = true;
    if (cursor >= expression.length()) {
        qInfo() << "Cursor is at the very end!";
        ok = false;
        return CalculationObject(Element::EMPTY);
    }

    const Element &e = expression.at(cursor);

    bool isDigit = CalqlatrElement::isDigit(e);
    bool isSubtract = isDigit ? false : e == Element::O_SUBTRACT;

    if (!isDigit && !isSubtract) {
        cursor++;
        return CalculationObject(e);
    }

    int sign = 1;
    double resValue = 0;
    // Check if subtract is a valid negative sign
    if (isSubtract) {
        if (cursor != 0 && expression.at(cursor - 1) != CalqlatrElement::BRACKET_L &&
            !CalqlatrElement::isFunction(expression.at(cursor - 1))) {
            cursor++;
            return CalculationObject(e);
        }
        sign = -1;
        cursor++;
    }

    // Check for pi and e
    if (e == Element::D_PI) {
        cursor++;
        return CalculationObject(M_PI * sign);
    }
    if (e == Element::D_E) {
        cursor++;
        return CalculationObject(M_E * sign);
    }

    // Calculate digit combination value
    int zerosAtStart = 0;
    bool encounteredDigit = false;
    while (cursor < expression.length()) {
        const Element &e = expression.at(cursor);
        if (!CalqlatrElement::isDigit0to9(e))
            break;

        if (!encounteredDigit && e == Element::D_0)
            zerosAtStart++;
        else
            encounteredDigit = true;

        resValue = resValue * 10 + e;
        cursor++;
    }
    if (resValue > CAL_MAX) {
        ok = false;
        return 0;
    }
    return CalculationObject(resValue * sign, zerosAtStart);
}

bool SciCalqlatr::getCalObjectExp(QList<CalculationObject> &res, const QList<Element> &exp) {
    res.clear();
    int cursor = 0;
    bool ok = true;
    while (cursor < exp.length() && ok) {
        CalculationObject obj = getNextObject(exp, cursor, ok);
        if (!ok)
            return false;
        res.append(obj);
    }
    return ok;
}

void SciCalqlatr::tryFixExp(QScopedPointer<QList<CalculationObject>> &exp) {
    QList<CalculationObject> *res = new QList<CalculationObject>;
    for (int i = 0; i < exp->length() - 1; i++) {
        const CalculationObject &obj = exp->at(i);
        const CalculationObject &next = exp->at(i + 1);
        res->append(obj);

        bool isObjValid = obj.isNumber() || obj.isRBracket() || obj.getElement() == Element::O_FACTORIAL;
        bool isNextValid = next.isLBracket() || next.isFunction() || next.isNumber();
        if (isObjValid && isNextValid)
            res->append(CalculationObject(Element::O_MULTIPLY));
    }
    res->append(exp->last());
    exp.reset(res);
}

const bool SciCalqlatr::convertToRPN(QList<CalculationObject> &res, const QList<CalculationObject> &exp) {
    res.clear();
    QStack<CalculationObject> stack;
    for (int i = 0; i < exp.length(); i++) {
        const CalculationObject &obj = exp.at(i);
        // If object is a number, append to rpn
        if (obj.isNumber()) {
            res.append(obj);
            continue;
        }

        // If object is RBracket, pop stack to result until meet an LBracket or function
        if (obj.isRBracket()) {
            bool matched = false;
            while (!stack.isEmpty()) {
                // LBracket
                if (stack.top().isLBracket()) {
                    stack.pop();
                    matched = true;
                    break;
                }
                // Function
                if (stack.top().isFunction()) {
                    res.append(stack.pop());
                    matched = true;
                    break;
                }
                // Operator
                res.append(stack.pop());
            }
            if (!matched)
                return false;
            else
                continue;
        }

        // If object is not a number nor RBracket
        // Stack is empty or object is LBracket/Function, simply push
        if (stack.isEmpty() || obj.isLBracket() || obj.isFunction()) {
            stack.push(obj);
            continue;
        }

        // Stack has an operator on top
        // If operator priority less or equal than stack top, pop to result
        while (!stack.isEmpty() && stack.top().isOperator() && stack.top().cmpPriority(obj) >= 0) {
            res.append(stack.pop());
        }
        stack.push(obj);
    }

    // Pop clear the operator stack
    while (!stack.isEmpty()) {
        res.append(stack.pop());
    }

    return true;
}

double SciCalqlatr::calculateRPN(const QList<CalculationObject> &rpn, bool &ok, const AngleUnit &unit) {
    ok = true;
    QStack<CalculationObject> stack;
    for (const CalculationObject &obj : rpn) {
        // If is number, push into stack
        if (obj.isNumber()) {
            stack.push(obj);
            continue;
        }

        if (obj.isFunction() || obj.getElement() == Element::O_FACTORIAL) {
            if (stack.isEmpty()) {
                ok = false;
                return 0;
            }
            double res = _calculate(obj, stack.pop(), ok, unit);
            if (!ok)
                return 0;

            stack.push(res);
            continue;
        }

        if (obj.isOperator()) {
            if (stack.length() < 2) {
                ok = false;
                return 0;
            }

            const CalculationObject &num2 = stack.pop();
            const CalculationObject &num1 = stack.pop();
            double res = _calculate(obj, num1, num2, ok, unit);
            if (!ok)
                return 0;

            stack.push(res);
            continue;
        }
    }

    if (stack.length() != 1) {
        ok = false;
        return 0;
    }

    return stack.pop().value;
}

double SciCalqlatr::_calculate(const CalculationObject &operation, const CalculationObject &num1,
    const CalculationObject &num2, bool &ok, const AngleUnit &unit) {
    if (!num1.isNumber() || !num2.isNumber()) {
        ok = false;
        return 0;
    }
    double n1 = num1.value;
    double n2 = num2.value;
    // bool isRad = unit == AngleUnit::RAD;
    // double radn1 = isRad ? n1 : qDegreesToRadians(n1);
    // double radn2 = isRad ? n2 : qDegreesToRadians(n2);
    ok = true;

    unsigned long long intN1 = static_cast<long long>(n1);
    unsigned long long intN2 = static_cast<long long>(n2);
    bool isN1Int = n1 - intN1 == 0;
    bool isN2Int = n2 - intN2 == 0;

    QScopedPointer<QString> pStr;

    switch (operation.getElement()) {
    case Element::O_ADD:
        return n1 + n2;
    case Element::O_SUBTRACT:
        return n1 - n2;
    case Element::O_MULTIPLY:
        return n1 * n2;
    case Element::O_DIVIDE:
        if (n2 == 0) {
            ok = false;
            return 0;
        }
        return n1 / n2;
    case Element::O_MOD:
        if (!isN1Int || !isN2Int) {
            ok = false;
            return 0;
        }
        return (int)n1 % (int)n2;
    case Element::O_POWER:
        return qPow(n1, n2);
    case Element::O_DOT:
        if (!isN1Int || !isN2Int || n2 < 0) {
            ok = false;
            return 0;
        }
        pStr.reset(new QString(QString::number(intN1) + '.' + QString::number(intN2)));
        return pStr->toDouble();
    default:
        ok = false;
        break;
    }
    return 0;
}

// Single operand calculation
double SciCalqlatr::_calculate(const CalculationObject &operation, const CalculationObject &num, bool &ok,
    const AngleUnit &unit) {
    if (!num.isNumber()) {
        ok = false;
        return 0;
    }
    bool isRad = unit == AngleUnit::RAD;
    double n = num.value;
    double radn = isRad ? n : qDegreesToRadians(n);
    ok = true;

    bool isInt = n - (int)n == 0;
    double tmp = 1;
    switch (operation.getElement()) {
    case Element::O_FACTORIAL:
        if (!isInt) {
            ok = false;
            return 0;
        }
        if (n == 0)
            return 1;
        for (int i = n; i > 1; i--) {
            tmp *= i;
        }
        return tmp;
    case Element::F_ABS:
        return qFabs(n);
    case Element::F_SQRT:
        if (n < 0) {
            ok = false;
            return 0;
        }
        return qSqrt(n);
    case Element::F_LG:
        if (n <= 0) {
            ok = false;
            return 0;
        }
        return qLn(n) / M_LN10;
    case Element::F_LN:
        if (n <= 0) {
            ok = false;
            return 0;
        }
        return qLn(n);
    case Element::F_SIN:
        return qSin(radn);
    case Element::F_ARCSIN:
        if (radn > 1 || radn < -1) {
            ok = false;
            return 0;
        }
        return isRad ? qAsin(radn) : qRadiansToDegrees(qAsin(radn));
    case Element::F_COS:
        return qCos(radn);
    case Element::F_ARCCOS:
        if (radn > 1 || radn < -1) {
            ok = false;
            return 0;
        }
        return isRad ? qAcos(radn) : qRadiansToDegrees(qAcos(radn));
    case Element::F_SEC:
        tmp = qCos(radn);
        if (tmp == 0) {
            ok = false;
            return 0;
        }
        return 1 / tmp;
    case Element::F_ARCSEC:
        if (radn > -1 && radn < 1) {
            ok = false;
            return 0;
        }
        tmp = 1 / radn;
        return isRad ? qAcos(tmp) : qRadiansToDegrees(qAcos(tmp));
    case Element::F_TAN:
        tmp = radn / M_PI;
        // Check if n == kπ+-π/2
        if (qFabs(qFabs(tmp) - 0.5) < 1e-6) {
            ok = false;
            return 0;
        }
        return qTan(radn);
    case Element::F_ARCTAN:
        return isRad ? qAtan(radn) : qRadiansToDegrees(qAtan(radn));
    default:
        ok = false;
        break;
    }
    return 0;
}

// Debug usage
void printCalObjList(QList<CalculationObject> &list) {
    QDebug info = qInfo();
    for (CalculationObject &obj : list) {
        info << QString().fill('0', obj.zerosAtStart) + obj.toString();
    }
}
