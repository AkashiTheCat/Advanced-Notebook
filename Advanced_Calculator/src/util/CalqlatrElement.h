#ifndef CALQLATRELEMENT_H
#define CALQLATRELEMENT_H

#include <QObject>
#include <QDebug>
#define CAL_MAX 9.9e16
#define CAL_MIN -9.9e16

class CalqlatrElement {
public:
    enum Element {
        D_0,
        D_1,
        D_2,
        D_3,
        D_4,
        D_5,
        D_6,
        D_7,
        D_8,
        D_9,
        D_PI,
        D_E,
        F_SIN,
        F_ARCSIN,
        F_COS,
        F_ARCCOS,
        F_TAN,
        F_ARCTAN,
        F_SEC,
        F_ARCSEC,
        F_LG,
        F_LN,
        F_ABS,
        F_SQRT,
        O_POWER,
        O_FACTORIAL,
        O_ADD,
        O_SUBTRACT,
        O_MULTIPLY,
        O_DIVIDE,
        O_MOD,
        O_DOT,
        BRACKET_L,
        BRACKET_R,
        INF,
        EMPTY,
    };

    inline static const QStringList ELEMENTS_STRING = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "π",
        "e", "sin(", "arcsin(", "cos(", "arccos(", "tan(", "arctan(", "sec(", "arcsec(", "lg(", "ln(", "abs(",
        "sqrt(", "^", "!", "+", "-", "×", "÷", " mod ", ".", "(", ")", "∞" };

    static const int getElementIndexFromStringIndex(const QList<Element> &list, const int &index);
    static const QString convertElementListToString(const QList<Element> &list);
    static const QString &getStringForElement(const Element &e);

    static void convertNumToElementList(QList<Element> &res, const double &num);
    static const Element getDigitElement(const int i);
    static const bool isDigit(const Element &e);
    static const bool isDigit0to9(const Element &e);
    static const bool isFunction(const Element &e);
    static const bool isOperator(const Element &e);
    static const bool isBracket(const Element &e);
    static const int cmpOperatorPriority(const Element &e1, const Element &e2);
    static const int getOperatorPriority(const Element &e);

private:
    static const int DIGIT_START = 0;
    static const int FUNCTION_START = 12;
    static const int OPERATOR_START = 24;
    static const int BRACKET_START = 32;
    static const int FLAGS_START = 34;
};

inline const int CalqlatrElement::getElementIndexFromStringIndex(const QList<Element> &list,
    const int &index) {
    int len = 0;
    for (int i = 0; i < list.length(); i++) {
        if (len >= index)
            return i;
        len += getStringForElement(list.at(i)).length();
    }

    // Index is at the very end
    return list.length();
}

inline const QString CalqlatrElement::convertElementListToString(const QList<Element> &list) {
    QString res;
    for (Element e : list) {
        res.append(getStringForElement(e));
    }
    return res;
}

inline const QString &CalqlatrElement::getStringForElement(const Element &e) {
    return ELEMENTS_STRING.at(e);
}

inline void CalqlatrElement::convertNumToElementList(QList<Element> &res, const double &num) {
    res.clear();
    if (num >= CAL_MAX) {
        res.append(Element::INF);
        return;
    }
    if (num < CAL_MIN) {
        res.append(Element::O_SUBTRACT);
        res.append(Element::INF);
    }

    unsigned long long pInt = static_cast<unsigned long long>(qFabs(num));
    double pFrac = qFabs(num) - pInt;
    bool isNegative = num < 0;

    do {
        res.insert(0, getDigitElement(pInt % 10));
        pInt /= 10;
    } while (pInt > 0);

    if (isNegative) {
        res.insert(0, Element::O_SUBTRACT);
    }

    if (pFrac < 1e-12)
        return;
    res.append(Element::O_DOT);

    double epsilon = 1e-12;
    const int maxDecimalDigits = 12;
    for (int decimalDigits = 0; pFrac > epsilon && decimalDigits < maxDecimalDigits; decimalDigits++) {
        pFrac *= 10;
        int digit = static_cast<int>(pFrac + epsilon);
        res.append(getDigitElement(digit));
        pFrac -= digit;
        epsilon *= 10;
    }
}

inline const CalqlatrElement::Element CalqlatrElement::getDigitElement(const int i) {
    if (i < 0 || i > 9)
        qFatal("FATAL: Digit element index out of bound");
    return Element(DIGIT_START + i);
}

inline const bool CalqlatrElement::isDigit(const Element &e) {
    return e < FUNCTION_START;
}

inline const bool CalqlatrElement::isDigit0to9(const Element &e) {
    return e <= 9;
}

inline const bool CalqlatrElement::isFunction(const Element &e) {
    return e >= FUNCTION_START && e < OPERATOR_START;
}

inline const bool CalqlatrElement::isOperator(const Element &e) {
    return e >= OPERATOR_START && e < BRACKET_START;
}

inline const bool CalqlatrElement::isBracket(const Element &e) {
    return e >= BRACKET_START && e < FLAGS_START;
}

inline const int CalqlatrElement::cmpOperatorPriority(const Element &e1, const Element &e2) {
    return getOperatorPriority(e1) - getOperatorPriority(e2);
}

inline const int CalqlatrElement::getOperatorPriority(const Element &e) {
    switch (e) {
    case Element::O_DOT:
        return 4;
    case Element::O_FACTORIAL:
        return 3;
    case Element::O_POWER:
        return 2;
    case Element::O_MULTIPLY:
    case Element::O_DIVIDE:
    case Element::O_MOD:
        return 1;
    default:
        return 0;
    }
}

#endif // CALQLATRELEMENT_H
