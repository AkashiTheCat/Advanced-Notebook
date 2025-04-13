#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>
#include <QLineEdit>
#include <QToolButton>

#include "ColorEditBox.h"

class ColorPicker : public QWidget {
    Q_OBJECT
private:
    ColorEditBox *colorEditBox;
    QToolButton *colorButton;
    QMenu *colorMenu;
    QColor currentColor;
    inline static const QList<QColor> standardColors = {
        Qt::black, Qt::white, Qt::darkRed, Qt::red,
        QColor::fromRgb(QRgb(0xFFC000)), Qt::yellow,
        Qt::green, Qt::darkGreen, Qt::cyan, Qt::blue,
        Qt::darkBlue, QColor::fromRgb(QRgb(0x7030A0))
    };

    void initColorMenu();

public:
    explicit ColorPicker(QWidget *parent = nullptr);
    const QColor &color() const;
    void setColor(const QColor &color);

signals:
    void colorChanged(const QColor &color);
    void colorButtonClicked();

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // COLORPICKER_H
