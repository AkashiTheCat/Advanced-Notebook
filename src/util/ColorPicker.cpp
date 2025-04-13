#include <QLayout>
#include <QToolButton>
#include <QRegularExpressionValidator>
#include <QMenu>
#include <QWidgetAction>
#include <QColorDialog>
#include <QResizeEvent>
#include <QComboBox>

#include "ColorPicker.h"
#include "IconHelper.h"

void ColorPicker::initColorMenu() {
    // Standard built-in color buttons
    colorMenu = new QMenu(this);
    colorMenu->setBackgroundRole(QPalette::Base);

    QWidget *colorWidget = new QWidget(this);
    QGridLayout *grid = new QGridLayout(colorWidget);
    grid->setSpacing(2);
    int row = 0, col = 0;
    for (const QColor &color : standardColors) {
        QToolButton *stdColorBtn = new QToolButton;
        stdColorBtn->setFixedSize(16, 16);
        stdColorBtn->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        connect(stdColorBtn, &QToolButton::clicked, this,
            [this, color]() {
                setColor(color);
            });
        grid->addWidget(stdColorBtn, row, col++);
        if (col > 5) {
            col = 0;
            row++;
        }
    }

    QWidgetAction *stdColorAction = new QWidgetAction(colorMenu);
    stdColorAction->setDefaultWidget(colorWidget);
    colorMenu->addAction(stdColorAction);

    // Custom color button
    QWidgetAction *moreColorAction = new QWidgetAction(colorMenu);
    QToolButton *moreColorBtn = new QToolButton(this);
    moreColorBtn->setText(tr("More Colors..."));

    connect(moreColorBtn, &QToolButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(currentColor, this, tr("Select Color"));
        if (color.isValid()) {
            setColor(color);
        }
        colorMenu->close();
    });
    moreColorAction->setDefaultWidget(moreColorBtn);

    colorMenu->addSeparator();
    colorMenu->addAction(moreColorAction);
}

ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent) {
    initColorMenu();

    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    colorButton = new QToolButton(this);
    colorButton->setFixedSize(this->height(), this->height());
    colorButton->setAutoRaise(true);
    QIcon icon(":/assets/icon/fontcolor.png");
    IconHelper::setIconColor(icon, Qt::black);
    colorButton->setIcon(icon);
    connect(colorButton, &QToolButton::clicked, this, [this]() {
        emit colorButtonClicked();
    });

    // Hex color edit
    colorEditBox = new ColorEditBox(this);
    colorEditBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    colorEditBox->setMenu(colorMenu);
    connect(colorEditBox, &ColorEditBox::editTextChanged, this, [this](const QString &text) {
        QColor color(text);
        if (color.isValid())
            this->setColor(color);
        else
            this->colorEditBox->setEditText(this->currentColor.name());
    });

    layout->addWidget(colorButton);
    layout->addWidget(colorEditBox);
    this->setLayout(layout);
}

const QColor &ColorPicker::color() const {
    return currentColor;
}

void ColorPicker::setColor(const QColor &color) {
    if (color == currentColor || !color.isValid())
        return;
    currentColor = color;
    QIcon icon = colorButton->icon();
    IconHelper::setIconColor(icon, color);
    colorButton->setIcon(icon);
    colorEditBox->setEditText(color.name());
    emit colorChanged(color);
}

void ColorPicker::resizeEvent(QResizeEvent *event) {
    const QSize &size = event->size();
    int height = size.height();
    colorButton->setFixedSize(height, height);
}
