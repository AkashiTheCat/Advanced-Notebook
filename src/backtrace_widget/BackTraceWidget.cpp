#include "BackTraceWidget.h"
#include "ProgramSettings.h"
#include "CalqlatrStaticProperties.h"

BackTraceWidget::BackTraceWidget(QWidget *parent) : QListWidget(parent) {
    this->setWindowFlags(
        Qt::Window |
        Qt::CustomizeWindowHint |
        Qt::WindowCloseButtonHint |
        Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint);
    this->setWindowIcon(QIcon(":/assets/icon/trace.svg"));
    this->setWindowTitle(tr("Back Trace"));
    this->setMinimumHeight(100);

    QFont font = this->font();
    font.setPointSize(ProgramSettings::TRACEWINDOW_FONT_SIZE.value());
    this->setFont(font);
    this->setFrameStyle(QFrame::NoFrame);
    this->setUniformItemSizes(true);
    this->setAlternatingRowColors(true);

    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    // this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

void BackTraceWidget::addItem(const QString &text, qint32 id) {
    QListWidgetItem *item = new QListWidgetItem(text);
    item->setData(CalqlatrStaticProperties::ROLE_RECORD_ID_DATA, id);
    QListWidget::addItem(item);
}

void BackTraceWidget::insertItem(int row, const QString &text, qint32 id) {
    QListWidgetItem *item = new QListWidgetItem(text);
    item->setData(CalqlatrStaticProperties::ROLE_RECORD_ID_DATA, id);
    QListWidget::insertItem(row, item);
}

qint32 BackTraceWidget::getId(int row) const {
    return this->item(row)->data(CalqlatrStaticProperties::ROLE_RECORD_ID_DATA).toInt();
}
