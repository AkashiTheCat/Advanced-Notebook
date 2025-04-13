#ifndef CALQLATRHISTORYWIDGET_H
#define CALQLATRHISTORYWIDGET_H

#include <QWidget>
#include <QListWidgetItem>

#include "CalqlatrRecord.h"
#include "CalqlatrHistory.h"
#include "RecordTransferSrcInterface.h"

namespace Ui {
    class CalqlatrHistoryWidget;
}

class CalqlatrHistoryWidget : public QWidget, public RecordTransferSrcInterface {
    Q_OBJECT
private:
    Ui::CalqlatrHistoryWidget *ui;

    static qint32 getRecordId(QListWidgetItem *item);
    QListWidgetItem *createListWidgetItem(const CalqlatrRecord &rec);

public:
    explicit CalqlatrHistoryWidget(QWidget *parent = nullptr);
    ~CalqlatrHistoryWidget();

    void setFontSize(int baseSize);

    CalqlatrHistory history;

    const qint32 getRecordItemRow(const qint32 id) const;
    QListWidgetItem *getRecordItemAt(const qint32 row);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

    bool transfer = false;

signals:
    void transferToCalqlatr(const CalqlatrRecord &record);
    void requestDeleteFromBuffer(const quint16 id);
    void transferStateChanged(bool canTransfer);

private slots:
    void handleItemSelection();

public slots:
    void gotoRecord(qint32 id);

    void insertRecord(const CalqlatrRecord &rec);
    void reloadDisplayedRecords();

    // RecordTransferSrcInterface interface
public:
    virtual bool canTransfer() override;
    virtual const CalqlatrRecord &getRecordToTransfer() override;
};

#endif // CALQLATRHISTORYWIDGET_H
