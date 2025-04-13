#include "CalqlatrWidget.h"
#include "ui_CalqlatrWidget.h"
#include "CalqlatrHistoryWidget.h"

CalqlatrWidget::CalqlatrWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::CalqlatrWidget) {
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    pageSwitchButton = new MultiStatePushButton(this);
    pageSwitchButton->setStates({ tr("History"), tr("Calculator") });
    connect(pageSwitchButton, &MultiStatePushButton::clicked, this, &CalqlatrWidget::switchPage);
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, pageSwitchButton,
        &MultiStatePushButton::switchState);
    ui->headLineLO->addWidget(pageSwitchButton);

    connect(ui->calqlatrHistoryWidget, &CalqlatrHistoryWidget::transferToCalqlatr,
        ui->sciCalqlatrWidget->calqlatr, &SciCalqlatr::loadRecord);
    connect(ui->calqlatrHistoryWidget, &CalqlatrHistoryWidget::transferToCalqlatr, this,
        &CalqlatrWidget::switchPage);

    ui->transferButton->setDisabled(true);
    connect(ui->calqlatrHistoryWidget, &CalqlatrHistoryWidget::transferStateChanged, this,
        &CalqlatrWidget::handleTranferStateChange);
    connect(ui->sciCalqlatrWidget, &SciCalqlatrWidget::transferStateChanged, this, &CalqlatrWidget::handleTranferStateChange);

    connect(ui->transferButton, &QPushButton::clicked, this, &CalqlatrWidget::transferToNotebook);

    connect(ui->sciCalqlatrWidget->calqlatr, &SciCalqlatr::recordProduced, ui->calqlatrHistoryWidget,
        &CalqlatrHistoryWidget::insertRecord);
}

CalqlatrWidget::~CalqlatrWidget() {
    delete ui;
}

void CalqlatrWidget::setCalHistoryBaseFontSize(int size) {
    this->ui->calqlatrHistoryWidget->setFontSize(size);
}

void CalqlatrWidget::setSciCalBaseFontSize(int size) {
    this->ui->sciCalqlatrWidget->setFontSize(size);
}


void CalqlatrWidget::readFrom(QDataStream &in) {
    this->ui->sciCalqlatrWidget->calqlatr->readFrom(in);
    this->ui->calqlatrHistoryWidget->history.readFrom(in);
    this->ui->sciCalqlatrWidget->reloadDisplay();
    this->ui->calqlatrHistoryWidget->reloadDisplayedRecords();
}

void CalqlatrWidget::writeTo(QDataStream &out) {
    this->ui->sciCalqlatrWidget->calqlatr->writeTo(out);
    this->ui->calqlatrHistoryWidget->history.writeTo(out);
}

void CalqlatrWidget::switchPage() {
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() == 1 ? 0 : 1);
    RecordTransferSrcInterface *src =
        dynamic_cast<RecordTransferSrcInterface *>(ui->stackedWidget->currentWidget());
    if (src != nullptr) {
        this->ui->transferButton->setDisabled(!src->canTransfer());
    } else {
        this->ui->transferButton->setDisabled(true);
    }
}

void CalqlatrWidget::gotoHistoryRecord(qint32 id) {
    if (this->ui->stackedWidget->currentIndex() != 1)
        switchPage();

    this->ui->calqlatrHistoryWidget->gotoRecord(id);
}

void CalqlatrWidget::loadRecord(const CalqlatrRecord &rec) {
    ui->sciCalqlatrWidget->calqlatr->loadRecord(rec);
}

void CalqlatrWidget::handleTranferStateChange(bool canTransfer) {
    QObject *src = sender();
    if (src == this->ui->stackedWidget->currentWidget()) {
        this->ui->transferButton->setDisabled(!canTransfer);
    }
}

void CalqlatrWidget::transferToNotebook() {
    RecordTransferSrcInterface *src =
        dynamic_cast<RecordTransferSrcInterface *>(ui->stackedWidget->currentWidget());
    if (src != nullptr) {
        emit transferToNoteRequested(src->getRecordToTransfer());
    }
}

bool CalqlatrWidget::getRecordsBeforeId(QList<CalqlatrRecord> &res, qint32 id, int count) {
    const CalqlatrHistory &history = this->ui->calqlatrHistoryWidget->history;
    const QList<CalqlatrRecord> &records = history.records();
    qint32 to = history.getIndex(id);
    if (to < 0 || count < 1)
        return false;

    qint32 from = to - count + 1;
    from = from < 0 ? 0 : from;
    res = records.sliced(from, to - from + 1);
    return true;
}
