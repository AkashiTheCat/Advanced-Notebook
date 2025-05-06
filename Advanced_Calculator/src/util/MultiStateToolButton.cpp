#include "MultiStateToolButton.h"

MultiStateToolButton::MultiStateToolButton(QWidget *parent) : QToolButton(parent) {}

const ButtonState &MultiStateToolButton::getCurrentState() const {
    return this->states.at(currentStateIndex);
}

int MultiStateToolButton::getCurrentStateIndex() const { return currentStateIndex; }

void MultiStateToolButton::addState(const ButtonState &state) {
    this->states.append(state);
    if (this->states.length() == 1) {
        this->setIcon(state.getIcon());
        this->setText(state.getText());

        emit stateSwitched(0);
    }
}

void MultiStateToolButton::setStates(const QList<ButtonState> &states) {
    this->states = states;
    int index = qMin(this->currentStateIndex, states.length() - 1);
    const ButtonState &state = states.at(index);
    this->setIcon(state.getIcon());
    this->setText(state.getText());

    emit stateSwitched(index);
}

void MultiStateToolButton::switchState(int index) {
    if (index == -1) {
        int maxStateIndex = states.length() - 1;
        if (maxStateIndex <= 0)
            return;
        currentStateIndex = currentStateIndex == maxStateIndex ? 0 : currentStateIndex + 1;
    } else {
        currentStateIndex = index;
    }

    const ButtonState &state = states.at(currentStateIndex);

    this->setIcon(state.getIcon());
    this->setText(state.getText());

    emit stateSwitched(currentStateIndex);
}

void MultiStateToolButton::switchNextState() {
    int maxStateIndex = states.length() - 1;
    if (maxStateIndex <= 0)
        return;
    currentStateIndex = currentStateIndex == maxStateIndex ? 0 : currentStateIndex + 1;

    const ButtonState &state = states.at(currentStateIndex);

    this->setIcon(state.getIcon());
    this->setText(state.getText());

    emit stateSwitched(currentStateIndex);
}
