#ifndef SCICALQLATRWIDGET_H
#define SCICALQLATRWIDGET_H

#include <QWidget>
#include <QQueue>
#include <QLineEdit>

#include "SciCalqlatr.h"
#include "SciCalqlatrButton.h"
#include "CalqlatrElement.h"
#include "CalqlatrRecord.h"
#include "CalculationObject.h"
#include "MultiStateToolButton.h"
#include "RecordTransferSrcInterface.h"

typedef CalqlatrElement::Element Element;

QT_BEGIN_NAMESPACE
namespace Ui {
    class SciCalqlatrWidget;
}
QT_END_NAMESPACE

class SciCalqlatrWidget : public QWidget, public RecordTransferSrcInterface {
    Q_OBJECT

public:
    SciCalqlatr *calqlatr;

    SciCalqlatrWidget(QWidget *parent = nullptr);
    ~SciCalqlatrWidget();

    void setFontSize(int baseSize);

private:
    Ui::SciCalqlatrWidget *ui;

    SciCalqlatrButton *sinButton;
    SciCalqlatrButton *cosButton;
    SciCalqlatrButton *tanButton;
    SciCalqlatrButton *secButton;
    SciCalqlatrButton *lgButton;
    SciCalqlatrButton *lnButton;
    SciCalqlatrButton *absButton;
    SciCalqlatrButton *sqrtButton;

    SciCalqlatrButton *powerButton;
    SciCalqlatrButton *factorialButton;
    SciCalqlatrButton *addButton;
    SciCalqlatrButton *subtractButton;
    SciCalqlatrButton *multiplyButton;
    SciCalqlatrButton *divideButton;
    SciCalqlatrButton *modButton;
    SciCalqlatrButton *dotButton;

    SciCalqlatrButton *LBracketButton;
    SciCalqlatrButton *RBracketButton;
    SciCalqlatrButton *eulerNumButton;
    SciCalqlatrButton *piButton;
    QList<SciCalqlatrButton *> digitButtons;

    MultiStateToolButton *degRadSwitchButton;
    QToolButton *trigoSwitchButton;
    QToolButton *backspaceButton;
    QToolButton *clearButton;
    QToolButton *calculateButton;

    QList<QLineEdit *> recordLineEdits;

    bool transfer = false;

    // UI
    template <typename pMemberFunction>
    SciCalqlatrButton *CreateButton(const ButtonState &state, const Element &content,
        const pMemberFunction &member);
    template <typename pMemberFunction>
    SciCalqlatrButton *CreateButton(const QList<ButtonState> &states, const QList<Element> &contents,
        const pMemberFunction &member);

signals:
    void transferStateChanged(bool canTransfer);

private slots:
    void buttonClicked();
    void backspaceClicked();
    void clearButtonClicked();

    void handleElementInsert(int index, Element e);

public slots:
    void setError();
    void reloadDisplay();
    void updateText();
    void updateRecordsText();
    void calqlatr_calculate();

    // RecordTransferSrcInterface interface
public:
    virtual bool canTransfer() override;
    virtual const CalqlatrRecord &getRecordToTransfer() override;
};
#endif // SCICALQLATRWIDGET_H
