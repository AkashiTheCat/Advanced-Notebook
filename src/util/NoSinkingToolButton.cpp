#include <QStylePainter>
#include <QStyleOptionToolButton>

#include "NoSinkingToolButton.h"

NoSinkingToolButton::NoSinkingToolButton(QWidget *parent) : QToolButton(parent) {}

NoSinkingToolButton::~NoSinkingToolButton() {}

void NoSinkingToolButton::paintEvent(QPaintEvent *event) {
    QStylePainter painter(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    painter.drawPrimitive(QStyle::PE_PanelButtonTool, opt);

    opt.state.setFlag(QStyle::State_Sunken, false);
    painter.drawControl(QStyle::CE_ToolButtonLabel, opt);
    painter.drawText(rect(), Qt::AlignCenter, text());
}
