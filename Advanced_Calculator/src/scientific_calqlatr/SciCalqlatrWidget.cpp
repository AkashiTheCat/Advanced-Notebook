#include <QtMath>
#include <QStack>

#include "SciCalqlatrWidget.h"
#include "./ui_SciCalqlatrWidget.h"
#include "SciCalqlatrButton.h"
#include "MultiStateToolButton.h"
#include "CalqlatrRecord.h"

void printCalObjList(QList<CalculationObject> &list);

SciCalqlatrWidget::SciCalqlatrWidget(QWidget *parent) : QWidget(parent), ui(new Ui::SciCalqlatrWidget) {
    ui->setupUi(this);
    this->calqlatr = new SciCalqlatr(this);

    recordLineEdits.append(this->ui->recentLine1);
    recordLineEdits.append(this->ui->recentLine2);
    recordLineEdits.append(this->ui->recentLine3);

    connect(this->calqlatr, &SciCalqlatr::currentExpChanged, this, &SciCalqlatrWidget::updateText);
    connect(this->calqlatr, &SciCalqlatr::recentRecordsChanged, this, &SciCalqlatrWidget::updateRecordsText);
    connect(this->calqlatr, &SciCalqlatr::inserted, this, &SciCalqlatrWidget::handleElementInsert);
    connect(this->calqlatr, &SciCalqlatr::recordProduced, this, [this](const CalqlatrRecord &) -> void {
        this->transfer = true;
        emit transferStateChanged(true);
    });

    // Function Buttons
    this->sinButton = CreateButton({ tr("sin"), tr("arcsin") }, { Element::F_SIN, Element::F_ARCSIN },
        &SciCalqlatrWidget::buttonClicked);
    sinButton->setObjectName("test");
    this->cosButton = CreateButton({ tr("cos"), tr("arccos") }, { Element::F_COS, Element::F_ARCCOS },
        &SciCalqlatrWidget::buttonClicked);
    this->tanButton = CreateButton({ tr("tan"), tr("arctan") }, { Element::F_TAN, Element::F_ARCTAN },
        &SciCalqlatrWidget::buttonClicked);
    this->secButton = CreateButton({ tr("sec"), tr("arcsec") }, { Element::F_SEC, Element::F_ARCSEC },
        &SciCalqlatrWidget::buttonClicked);
    this->lgButton = CreateButton(tr("lg"), Element::F_LG, &SciCalqlatrWidget::buttonClicked);
    this->lnButton = CreateButton(tr("ln"), Element::F_LN, &SciCalqlatrWidget::buttonClicked);
    this->absButton = CreateButton(tr("abs"), Element::F_ABS, &SciCalqlatrWidget::buttonClicked);
    this->sqrtButton =
        CreateButton(QIcon(":/icons/sqrt_icon.png"), Element::F_SQRT, &SciCalqlatrWidget::buttonClicked);

    // Operator Buttons
    this->powerButton =
        CreateButton(QIcon(":/icons/power_icon.png"), Element::O_POWER, &SciCalqlatrWidget::buttonClicked);
    this->factorialButton = CreateButton(tr("x!"), Element::O_FACTORIAL, &SciCalqlatrWidget::buttonClicked);
    this->addButton = CreateButton(tr("+"), Element::O_ADD, &SciCalqlatrWidget::buttonClicked);
    this->subtractButton = CreateButton(tr("-"), Element::O_SUBTRACT, &SciCalqlatrWidget::buttonClicked);
    this->multiplyButton = CreateButton(tr("×"), Element::O_MULTIPLY, &SciCalqlatrWidget::buttonClicked);
    this->divideButton = CreateButton(tr("÷"), Element::O_DIVIDE, &SciCalqlatrWidget::buttonClicked);
    this->modButton = CreateButton(tr("mod"), Element::O_MOD, &SciCalqlatrWidget::buttonClicked);
    this->dotButton = CreateButton(tr("."), Element::O_DOT, &SciCalqlatrWidget::buttonClicked);

    // Digits and Brackets
    this->LBracketButton = CreateButton(tr("("), Element::BRACKET_L, &SciCalqlatrWidget::buttonClicked);
    this->RBracketButton = CreateButton(tr(")"), Element::BRACKET_R, &SciCalqlatrWidget::buttonClicked);
    this->eulerNumButton = CreateButton(tr("e"), Element::D_E, &SciCalqlatrWidget::buttonClicked);
    this->piButton = CreateButton(tr("π"), Element::D_PI, &SciCalqlatrWidget::buttonClicked);
    for (int i = 0; i < 10; i++) {
        digitButtons.append(CreateButton(QString::number(i), CalqlatrElement::getDigitElement(i),
            &SciCalqlatrWidget::buttonClicked));
    }

    // Util Buttons
    this->trigoSwitchButton = new QToolButton(this);
    this->trigoSwitchButton->setText(tr("2nd"));
    trigoSwitchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(trigoSwitchButton, &MultiStateToolButton::clicked, sinButton, &SciCalqlatrButton::switchNextState);
    connect(trigoSwitchButton, &MultiStateToolButton::clicked, cosButton, &SciCalqlatrButton::switchNextState);
    connect(trigoSwitchButton, &MultiStateToolButton::clicked, tanButton, &SciCalqlatrButton::switchNextState);
    connect(trigoSwitchButton, &MultiStateToolButton::clicked, secButton, &SciCalqlatrButton::switchNextState);

    this->degRadSwitchButton = new MultiStateToolButton(this);
    degRadSwitchButton->setStates({ tr("rad"), tr("deg") });
    degRadSwitchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(degRadSwitchButton, &MultiStateToolButton::clicked, degRadSwitchButton,
        &SciCalqlatrButton::switchNextState);
    connect(degRadSwitchButton, &MultiStateToolButton::clicked, this->calqlatr, &SciCalqlatr::switchAngleUnit);

    this->backspaceButton = new QToolButton(this);
    backspaceButton->setIcon(QIcon(":/icons/backspace_icon.png"));
    backspaceButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(backspaceButton, &MultiStateToolButton::clicked, this, &SciCalqlatrWidget::backspaceClicked);

    this->clearButton = new QToolButton(this);
    clearButton->setText(tr("AC"));
    clearButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(clearButton, &MultiStateToolButton::clicked, this, &SciCalqlatrWidget::clearButtonClicked);

    this->calculateButton = new QToolButton(this);
    calculateButton->setText(tr("="));
    calculateButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(calculateButton, &MultiStateToolButton::clicked, this, &SciCalqlatrWidget::calqlatr_calculate);

    // Add buttons to layout
    QGridLayout *bottomLayout = ui->bottomLayout;
    // Line 0
    bottomLayout->addWidget(trigoSwitchButton, 0, 0);
    bottomLayout->addWidget(sinButton, 0, 1);
    bottomLayout->addWidget(cosButton, 0, 2);
    bottomLayout->addWidget(clearButton, 0, 3);
    bottomLayout->addWidget(backspaceButton, 0, 4);

    // Line 1
    bottomLayout->addWidget(degRadSwitchButton, 1, 0);
    bottomLayout->addWidget(tanButton, 1, 1);
    bottomLayout->addWidget(secButton, 1, 2);
    bottomLayout->addWidget(absButton, 1, 3);
    bottomLayout->addWidget(modButton, 1, 4);

    // Line 2
    bottomLayout->addWidget(powerButton, 2, 0);
    bottomLayout->addWidget(sqrtButton, 2, 1);
    bottomLayout->addWidget(LBracketButton, 2, 2);
    bottomLayout->addWidget(RBracketButton, 2, 3);
    bottomLayout->addWidget(divideButton, 2, 4);

    // Line 3
    bottomLayout->addWidget(factorialButton, 3, 0);
    bottomLayout->addWidget(multiplyButton, 3, 4);

    // Line 4
    bottomLayout->addWidget(lgButton, 4, 0);
    bottomLayout->addWidget(subtractButton, 4, 4);

    // Line 5
    bottomLayout->addWidget(lnButton, 5, 0);
    bottomLayout->addWidget(addButton, 5, 4);

    // Line 6
    bottomLayout->addWidget(dotButton, 6, 3);
    bottomLayout->addWidget(calculateButton, 6, 4);

    // Digits
    // 1~9
    int index = 1;
    for (int row = 5; row >= 3; row--) {
        for (int col = 1; col <= 3; col++) {
            bottomLayout->addWidget(digitButtons.at(index++), row, col);
        }
    }
    // 0
    bottomLayout->addWidget(digitButtons.at(0), 6, 2);
    // Pi, e
    bottomLayout->addWidget(piButton, 6, 0);
    bottomLayout->addWidget(eulerNumButton, 6, 1);
}

SciCalqlatrWidget::~SciCalqlatrWidget() {
    delete ui;
}

void SciCalqlatrWidget::setFontSize(int baseSize) {
    QFont font = this->ui->currentLine->font();
    font.setPointSize(baseSize);
    this->ui->currentLine->setFont(font);

    font.setPointSize(qRound((float)baseSize * 0.6));
    qInfo() << qRound((float)baseSize * 0.6);

    for (QLineEdit *line : std::as_const(this->recordLineEdits))
        line->setFont(font);
}

void SciCalqlatrWidget::setError() {
    this->calqlatr->clearCurrentExp();
    this->ui->currentLine->setText("ERROR");
}

void SciCalqlatrWidget::reloadDisplay() {
    this->updateText();
    this->updateRecordsText();
}

/*
 *      Inputs
 */

// Update text of currentExpression according to currentExp
void SciCalqlatrWidget::updateText() {
    qInfo() << "CurrentLine text updating";
    QString text = CalqlatrElement::convertElementListToString(calqlatr->currentExp());
    this->ui->currentLine->setText(text);
}

// Handle operator/function/digit buttons' click event
void SciCalqlatrWidget::buttonClicked() {
    this->transfer = false;
    emit transferStateChanged(false);

    SciCalqlatrButton *clickedButton = qobject_cast<SciCalqlatrButton *>(sender());

    const int cursorIndex = ui->currentLine->cursorPosition();
    int elementIndex = CalqlatrElement::getElementIndexFromStringIndex(calqlatr->currentExp(), cursorIndex);
    this->calqlatr->insertToExp(elementIndex, clickedButton->getContent());
}

void SciCalqlatrWidget::backspaceClicked() {
    const int cursorPos = ui->currentLine->cursorPosition();
    int elementIndex = CalqlatrElement::getElementIndexFromStringIndex(calqlatr->currentExp(), cursorPos);

    if (elementIndex == 0)
        return;

    const Element &e = this->calqlatr->currentExp().at(elementIndex - 1);
    int newCursorPos = cursorPos - CalqlatrElement::getStringForElement(e).length();

    this->calqlatr->removeFromExp(elementIndex - 1);
    ui->currentLine->setCursorPosition(newCursorPos);
}

void SciCalqlatrWidget::clearButtonClicked() {
    if (!calqlatr->isCurrentExpCleared())
        calqlatr->clearCurrentExp();
    else
        calqlatr->clearRecentRecords();
}

void SciCalqlatrWidget::handleElementInsert(int index, Element e) {
    const QList<Element> &exp = this->calqlatr->currentExp();
    int pos = 0;
    for (; index >= 0; index--) {
        pos += CalqlatrElement::getStringForElement(exp.at(index)).length();
    }
    this->ui->currentLine->setCursorPosition(pos);
}

/*
 *      Record Management
 */

void SciCalqlatrWidget::updateRecordsText() {
    qInfo() << "Records text updating";
    const QQueue<CalqlatrRecord>& records = calqlatr->recentRecords();
    for (int i = 0; i < recordLineEdits.length(); i++) {
        recordLineEdits.at(i)->setText(i < records.length() ? records.at(i).getText() : "");
    }
}

void SciCalqlatrWidget::calqlatr_calculate() {
    if (!calqlatr->calculate())
        setError();
}

bool SciCalqlatrWidget::canTransfer() {
    return this->transfer;
}

const CalqlatrRecord &SciCalqlatrWidget::getRecordToTransfer() {
    return this->calqlatr->recentRecords().last();
}


/*
 *      CreateButton Overloads
 */

template <typename pMemberFunction>
SciCalqlatrButton *SciCalqlatrWidget::CreateButton(const ButtonState &state, const Element &content,
    const pMemberFunction &member) {
    SciCalqlatrButton *button = new SciCalqlatrButton(state, content, this);
    connect(button, &SciCalqlatrButton::clicked, this, member);
    return button;
}

template <typename pMemberFunction>
SciCalqlatrButton *SciCalqlatrWidget::CreateButton(const QList<ButtonState> &states, const QList<Element> &contents,
    const pMemberFunction &member) {
    SciCalqlatrButton *button = new SciCalqlatrButton(states, contents, this);
    connect(button, &SciCalqlatrButton::clicked, this, member);
    return button;
}
