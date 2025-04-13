#include <QScrollBar>
#include <QMenu>
#include <QContextMenuEvent>
#include <QListWidget>
#include <QMessageBox>

#include "CalqlatrHistoryWidget.h"
#include "ui_CalqlatrHistoryWidget.h"
#include "CalqlatrStaticProperties.h"

CalqlatrHistoryWidget::CalqlatrHistoryWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::CalqlatrHistoryWidget) {
    ui->setupUi(this);

    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &CalqlatrHistoryWidget::handleItemSelection);
}

CalqlatrHistoryWidget::~CalqlatrHistoryWidget() {
    delete ui;
}

void CalqlatrHistoryWidget::setFontSize(int baseSize) {
    QFont font = this->ui->listWidget->font();
    font.setPointSize(baseSize);
    this->ui->listWidget->setFont(font);
}

const qint32 CalqlatrHistoryWidget::getRecordItemRow(const qint32 id) const {
    if (id < 0)
        return -1;

    QListWidget *listWidget = this->ui->listWidget;
    int left = 0;
    int right = listWidget->count() - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        qint32 midId = getRecordId(listWidget->item(mid));

        if (midId == id) {
            return mid;
        } else if (midId > id) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

QListWidgetItem *CalqlatrHistoryWidget::getRecordItemAt(const qint32 row) {
    return this->ui->listWidget->item(row);
}

void CalqlatrHistoryWidget::reloadDisplayedRecords() {
    ui->listWidget->clear();
    const QList<CalqlatrRecord> &records = this->history.records();
    for (int i = records.length() - 1; i >= 0; i--) {
        ui->listWidget->addItem(createListWidgetItem(records.at(i)));
    }
}

void CalqlatrHistoryWidget::handleItemSelection() {
    if (!transfer != ui->listWidget->selectedItems().empty()) {
        transfer = !transfer;
        emit transferStateChanged(transfer);
    }
}

void CalqlatrHistoryWidget::gotoRecord(qint32 id) {
    qint32 row = this->getRecordItemRow(id);
    qInfo() << row;
    if (row < 0)
        return;

    QListWidget *listWidget = this->ui->listWidget;
    listWidget->scrollToItem(listWidget->item(row));
    listWidget->clearSelection();
    listWidget->setCurrentRow(row);
}

void CalqlatrHistoryWidget::insertRecord(const CalqlatrRecord &rec) {
    this->history.appendRecord(rec);
    ui->listWidget->insertItem(0, createListWidgetItem(rec));
}

QListWidgetItem *CalqlatrHistoryWidget::createListWidgetItem(const CalqlatrRecord &rec) {
    QListWidgetItem *item = new QListWidgetItem(rec.getText());
    item->setData(CalqlatrStaticProperties::ROLE_RECORD_ID_DATA, rec.id());
    item->setTextAlignment(Qt::AlignRight);
    return item;
}

void CalqlatrHistoryWidget::contextMenuEvent(QContextMenuEvent *event) {
    QListWidgetItem *item = ui->listWidget->itemAt(event->pos());
    if (!item)
        return;

    int row = ui->listWidget->row(item);
    QMenu menu;
    QAction *deleteAction = menu.addAction("Delete");
    QAction *transferAction = menu.addAction("Load");
    QAction *selected = menu.exec(event->globalPos());

    if (selected == deleteAction) {
        if (row >= 0 && row < this->history.records().length()) {
            this->history.deleteRecord(getRecordId(item));
            ui->listWidget->takeItem(row);
        }
    } else if (selected == transferAction) {
        emit transferToCalqlatr(this->history.getRecord(getRecordId(item)));
    }
}

qint32 CalqlatrHistoryWidget::getRecordId(QListWidgetItem *item) {
    return item->data(CalqlatrStaticProperties::ROLE_RECORD_ID_DATA).toInt();
}

bool CalqlatrHistoryWidget::canTransfer() {
    return this->transfer;
}

const CalqlatrRecord &CalqlatrHistoryWidget::getRecordToTransfer() {
    return this->history.getRecord(getRecordId(this->ui->listWidget->currentItem()));
}
