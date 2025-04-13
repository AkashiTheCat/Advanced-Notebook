#ifndef CALCULATIONOBJECT_H
#define CALCULATIONOBJECT_H

#include "CalqlatrElement.h"

typedef CalqlatrElement::Element Element;

class CalculationObject {
private:
    Element element = Element::EMPTY;
    bool isNum = false;

public:
    double value = 0;
    int zerosAtStart = 0;
    CalculationObject(const Element &e);
    CalculationObject(const double &value);
    CalculationObject(const double &value, const int &zerosAtStart);

    const Element &getElement() const;
    const bool isNumber() const;

    const QString toString() const;
    const bool isFunction() const;
    const bool isOperator() const;
    const bool isBracket() const;
    const bool isLBracket() const;
    const bool isRBracket() const;
    const int getOperatorPriority() const;

    const int cmpPriority(const CalculationObject &o);
    static const int cmpOperatorPriority(const CalculationObject &o1, const CalculationObject &o2);
};

inline const int CalculationObject::cmpOperatorPriority(const CalculationObject &o1,
    const CalculationObject &o2) {
    return CalqlatrElement::cmpOperatorPriority(o1.element, o2.element);
}

#endif // CALCULATIONOBJECT_H
