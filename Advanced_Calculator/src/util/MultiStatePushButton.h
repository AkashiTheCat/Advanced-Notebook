#ifndef MULTISTATEPUSHBUTTON_H
#define MULTISTATEPUSHBUTTON_H

#include <QPushButton>
#include "ButtonState.h"

class MultiStatePushButton : public QPushButton {
    Q_OBJECT
public:
    MultiStatePushButton(QWidget *parent = nullptr);

    const ButtonState &getCurrentState() const;
    int getCurrentStateIndex() const;

    void addState(const ButtonState &state);
    void setStates(const QList<ButtonState> &states);

private:
    QList<ButtonState> states;
    int currentStateIndex = 0;

signals:
    void stateSwitched(int index);

public slots:
    void switchState(int index = -1);
};

#endif // MULTISTATEPUSHBUTTON_H
