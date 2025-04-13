#include "CalculationObject.h"

const Element &CalculationObject::getElement() const {
    return this->element;
}

CalculationObject::CalculationObject(const Element &e) : element(e) {
}

CalculationObject::CalculationObject(const double &value) : value(value) {
    this->isNum = true;
}

CalculationObject::CalculationObject(const double &value, const int &zerosAtStart)
    : value(value), zerosAtStart(zerosAtStart) {
    this->isNum = true;
}

const bool CalculationObject::isLBracket() const {
    return this->element == Element::BRACKET_L;
}

const bool CalculationObject::isRBracket() const {
    return this->element == Element::BRACKET_R;
}

const bool CalculationObject::isNumber() const {
    return isNum;
}

const QString CalculationObject::toString() const {
    return isNumber() ? QString::number(value) : CalqlatrElement::getStringForElement(this->element);
}

const bool CalculationObject::isFunction() const {
    return CalqlatrElement::isFunction(this->element);
}

const bool CalculationObject::isOperator() const {
    return CalqlatrElement::isOperator(this->element);
}

const bool CalculationObject::isBracket() const {
    return CalqlatrElement::isBracket(this->element);
}

const int CalculationObject::getOperatorPriority() const {
    return CalqlatrElement::getOperatorPriority(this->element);
}

const int CalculationObject::cmpPriority(const CalculationObject &o) {
    return CalqlatrElement::cmpOperatorPriority(this->element, o.element);
}
