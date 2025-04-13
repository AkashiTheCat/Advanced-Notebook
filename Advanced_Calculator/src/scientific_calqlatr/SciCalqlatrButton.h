#ifndef SCICALQLATRBUTTON_H
#define SCICALQLATRBUTTON_H

#include <QObject>
#include "MultiStateToolButton.h"
#include "CalqlatrElement.h"

typedef CalqlatrElement::Element Element;

class SciCalqlatrButton : public MultiStateToolButton {
    Q_OBJECT

public:
    SciCalqlatrButton(const ButtonState& state, const Element &content, QWidget *parent);
    SciCalqlatrButton(const QList<ButtonState>& states, const QList<Element> &contents, QWidget *parent);

    const Element &getContent() const;

private:
    QList<Element> contents;
};

#endif // SCICALQLATRBUTTON_H
