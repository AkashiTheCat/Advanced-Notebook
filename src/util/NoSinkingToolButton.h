#ifndef NOSINKINGTOOLBUTTON_H
#define NOSINKINGTOOLBUTTON_H

#include <QObject>
#include <QToolButton>

class NoSinkingToolButton : public QToolButton
{
    Q_OBJECT
public:
    NoSinkingToolButton(QWidget* parent = nullptr);
    ~NoSinkingToolButton();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // NOSINKINGTOOLBUTTON_H
