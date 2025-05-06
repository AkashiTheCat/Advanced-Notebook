#ifndef MULTISTATETOOLBUTTON_H
#define MULTISTATETOOLBUTTON_H

#include <QToolButton>
#include "ButtonState.h"

class MultiStateToolButton : public QToolButton {
    Q_OBJECT
public:
    MultiStateToolButton(QWidget *parent = nullptr);

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
    void switchNextState();
};

#endif // MULTISTATETOOLBUTTON_H
