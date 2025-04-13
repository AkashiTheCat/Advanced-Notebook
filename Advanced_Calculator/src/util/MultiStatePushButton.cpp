#include "MultiStatePushButton.h"

MultiStatePushButton::MultiStatePushButton(QWidget *parent) : QPushButton(parent) {}

const ButtonState &MultiStatePushButton::getCurrentState() const {
    return this->states.at(currentStateIndex);
}

int MultiStatePushButton::getCurrentStateIndex() const { return currentStateIndex; }

void MultiStatePushButton::addState(const ButtonState &state) {
    this->states.append(state);
    if (this->states.length() == 1) {
        this->setIcon(state.getIcon());
        this->setText(state.getText());

        emit stateSwitched(0);
    }
}

void MultiStatePushButton::setStates(const QList<ButtonState> &states) {
    this->states = states;
    int index = qMin(this->currentStateIndex, states.length() - 1);
    const ButtonState &state = states.at(index);
    this->setIcon(state.getIcon());
    this->setText(state.getText());

    emit stateSwitched(index);
}

void MultiStatePushButton::switchState(int index) {
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

    emit stateSwitched(index);
}
