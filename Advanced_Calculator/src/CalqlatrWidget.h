#ifndef CALQLATRWIDGET_H
#define CALQLATRWIDGET_H

#include <QWidget>

#include "MultiStatePushButton.h"
#include "CalqlatrRecord.h"

#ifdef ADVANCED_CALCULATOR_BUILD
#define ADVANCED_CALCULATOR_API Q_DECL_EXPORT
#else
#define ADVANCED_CALCULATOR_API Q_DECL_IMPORT
#endif

namespace Ui {
    class CalqlatrWidget;
}

class CalqlatrWidget : public QWidget {
    Q_OBJECT

public:
    explicit CalqlatrWidget(QWidget *parent = nullptr);
    ~CalqlatrWidget();

    void setCalHistoryBaseFontSize(int size);
    void setSciCalBaseFontSize(int size);

    void readFrom(QDataStream &in);
    void writeTo(QDataStream &out);

    bool getRecordsBeforeId(QList<CalqlatrRecord> &res, qint32 id, int count = 5);

public slots:
    void switchPage();
    void gotoHistoryRecord(qint32 id);
    void loadRecord(const CalqlatrRecord &rec);

    void handleTranferStateChange(bool canTransfer);
    void transferToNotebook();

private:
    Ui::CalqlatrWidget *ui;

    MultiStatePushButton *pageSwitchButton;

signals:
    void transferToNoteRequested(const CalqlatrRecord &rec);
};

#endif // CALQLATRWIDGET_H
