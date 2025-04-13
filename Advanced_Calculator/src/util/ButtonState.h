#ifndef BUTTONSTATE_H
#define BUTTONSTATE_H

#include <QIcon>

class ButtonState {
private:
    QIcon icon;
    QString text;

public:
    ButtonState(const QIcon &icon, const QString &text);
    ButtonState(const QIcon &icon);
    ButtonState(const QString &text);
    ButtonState();

    const QIcon &getIcon() const;
    const QString &getText() const;
};

#endif // BUTTONSTATE_H
