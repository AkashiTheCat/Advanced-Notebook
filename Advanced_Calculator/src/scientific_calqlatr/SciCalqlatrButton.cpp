#include "SciCalqlatrButton.h"

SciCalqlatrButton::SciCalqlatrButton(const QList<ButtonState> &states, const QList<Element> &contents,
    QWidget *parent)
    : MultiStateToolButton(parent), contents(contents) {
    this->setStates(states);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

SciCalqlatrButton::SciCalqlatrButton(const ButtonState &state, const Element &content, QWidget *parent)
    : MultiStateToolButton(parent) {
    this->addState(state);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contents.append(content);
}

const Element &SciCalqlatrButton::getContent() const {
    return this->contents.at(getCurrentStateIndex());
}
