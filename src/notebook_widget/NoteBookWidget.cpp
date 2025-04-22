#include <QScreen>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextBlock>
#include <QListWidget>

#include "NoteBookWidget.h"
#include "./ui_NoteBookWidget.h"
#include "RecordObject.h"
#include "ProgramSettings.h"
#include "BackTraceWidget.h"
#include "CalqlatrStaticProperties.h"

void NoteBookWidget::initUI() {
    ui->setupUi(this);
    ui->calqlatrToggleButton->setStates({ QIcon(":/assets/icon/doublearrow_right.svg"),
        QIcon(":/assets/icon/doublearrow_left.svg") });

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    this->resize(screenWidth * 0.5, screenHeight * 0.5);
    ui->calqlatrWidget->setFixedWidth(screenWidth * 0.15);

    updateSettings();
}

NoteBookWidget::NoteBookWidget(QWidget *parent) : AbstractFileEditor(parent), ui(new Ui::NoteBookWidget) {
    initUI();

    this->setOpeningTitle(tr("Open"));
    this->setOpeningFilter(tr("Advanced Notes (*.advcn);;Text files (*.txt)"));
    this->setSavingTitle(tr("Save As"));
    this->setSavingFilter(tr("Advanced Notes (*.advcn)"));
    this->setRenamingTitle(tr("Rename"));
    this->setRenamingFilter(tr("Advanced Notes (*.advcn);;Text files (*.txt)"));

    connect(ui->mainTextEdit, &AdvTextEdit::textChanged, this, [this] {
        this->setEdited(true);
    });

    /*
     *      Record Embedding/Tracing Logics
     */

    QTextDocument *doc = ui->mainTextEdit->document();
    doc->documentLayout()->registerHandler(RecordObject::TYPE, new RecordObject(ui->mainTextEdit));

    connect(ui->mainTextEdit, &AdvTextEdit::recordLoadingRequested, ui->calqlatrWidget,
        &CalqlatrWidget::loadRecord);

    // connect(ui->calqlatrWidget, &CalqlatrWidget::transferToNoteRequested, this,
    //         &NoteBookWidget::insertCalqlatrRecord);
    // This causes qt.core.qobject.connect: QObject::connect: signal not found in CalqlatrWidget
    // ↓ But this doesn't
    connect(ui->calqlatrWidget, SIGNAL(transferToNoteRequested(CalqlatrRecord)), this,
        SLOT(insertCalqlatrRecord(CalqlatrRecord))); // WHY??????????

    // Handle Trace Window
    connect(ui->mainTextEdit, &AdvTextEdit::traceWindowRequested, this,
        &NoteBookWidget::createTraceWindow);

    /*
     *      Calculator Toggle Button
     */

    connect(ui->calqlatrToggleButton, &MultiStatePushButton::clicked, this, [this]() {
        if (this->ui->calqlatrWidget->isHidden()) {
            this->ui->calqlatrWidget->show();
        } else {
            this->ui->calqlatrWidget->hide();
        }
        this->ui->calqlatrToggleButton->switchState();
    });

    /*
     *      Text Format Logics
     */

    ui->fontSizeComboBox->setValidator(new QIntValidator(6, 255, ui->fontSizeComboBox));

    connect(ui->mainTextEdit, &AdvTextEdit::currentCharFormatChanged, this,
        [this](const QTextCharFormat &fmt) {
            if (!this->ui->mainTextEdit->textCursor().hasSelection()) {
                this->updateFormatDisplay(fmt);
            }
        });

    connect(ui->fontComboBox, &QFontComboBox::currentFontChanged, this, [this](const QFont &font) {
        this->ui->mainTextEdit->setFontFamily(font.family());
    });

    connect(ui->fontSizeComboBox, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        bool ok = false;
        int size = text.toInt(&ok);
        if (ok) {
            QFont font = this->ui->mainTextEdit->currentFont();
            font.setPointSize(size);
            this->ui->mainTextEdit->setCurrentFont(font);
        }
    });

    connect(ui->colorPickerWidget, &ColorPicker::colorButtonClicked, this, [this]() {
        ColorPicker *picker = static_cast<ColorPicker *>(sender());
        QTextCharFormat fmt;
        fmt.setForeground(picker->color());
        this->ui->mainTextEdit->mergeCurrentCharFormat(fmt);
    });

    connect(ui->boldButton, &QToolButton::clicked, this, [this]() {
        QToolButton *btn = static_cast<QToolButton *>(sender());
        QFont font = this->ui->mainTextEdit->currentFont();
        font.setBold(btn->isChecked());
        this->ui->mainTextEdit->setCurrentFont(font);
    });

    connect(ui->italicButton, &QToolButton::clicked, this, [this]() {
        QToolButton *btn = static_cast<QToolButton *>(sender());
        QFont font = this->ui->mainTextEdit->currentFont();
        font.setItalic(btn->isChecked());
        this->ui->mainTextEdit->setCurrentFont(font);
    });

    connect(ui->underlineButton, &QToolButton::clicked, this, [this]() {
        QToolButton *btn = static_cast<QToolButton *>(sender());
        QFont font = this->ui->mainTextEdit->currentFont();
        font.setUnderline(btn->isChecked());
        this->ui->mainTextEdit->setCurrentFont(font);
    });

    connect(ui->strikeOutButton, &QToolButton::clicked, this, [this]() {
        QToolButton *btn = static_cast<QToolButton *>(sender());
        QFont font = this->ui->mainTextEdit->currentFont();
        font.setStrikeOut(btn->isChecked());
        this->ui->mainTextEdit->setCurrentFont(font);
    });

    // Format brush
    connect(ui->fmtBrushButton, &QToolButton::toggled, this, [this](bool checked) {
        if (checked) {
            ui->mainTextEdit->viewport()->installEventFilter(this);
            ui->mainTextEdit->setFmtBrushMode(true);
            this->fmtBrushFmt = this->ui->mainTextEdit->currentCharFormat();
            QPixmap pixmap(":/assets/icon/fmtbrush_cursor.png");
            this->ui->mainTextEdit->viewport()->setCursor(QCursor(pixmap));
        } else {
            ui->mainTextEdit->viewport()->removeEventFilter(this);
            ui->mainTextEdit->setFmtBrushMode(false);
        }
    });
}

NoteBookWidget::~NoteBookWidget() {
    delete ui;
}

void NoteBookWidget::insertCalqlatrRecord(const CalqlatrRecord &rec) {
    QTextCharFormat fmt = ui->mainTextEdit->currentCharFormat();
    QTextCharFormat recFmt = fmt;
    recFmt.merge(RecordObject::getTextCharFormat(rec));
    ui->mainTextEdit->textCursor().insertText(QString(QChar::ObjectReplacementCharacter), recFmt);
    ui->mainTextEdit->setCurrentCharFormat(fmt);
}

void NoteBookWidget::createTraceWindow(const QPoint &pos, qint32 id) {
    QList<CalqlatrRecord> records;
    if (!this->ui->calqlatrWidget->getRecordsBeforeId(records, id)) {
        QMessageBox::warning(this, "ERROR", "Record not found");
        return;
    }
    BackTraceWidget *widget = new BackTraceWidget(this);
    for (CalqlatrRecord &rec : records) {
        widget->addItem(rec.getText(), rec.id());
    }
    widget->move(pos);
    widget->show();

    connect(widget, &BackTraceWidget::itemDoubleClicked, this->ui->calqlatrWidget,
        [this](QListWidgetItem *item) -> void {
            qint32 id = item->data(CalqlatrStaticProperties::ROLE_RECORD_ID_DATA).toInt();
            this->ui->calqlatrWidget->gotoHistoryRecord(id);
        });
}

void NoteBookWidget::updateFormatLineHeight() {
    int height = ProgramSettings::FORMAT_EDIT_LINE_HEIGHT.value();
    ui->fmtBrushButton->setFixedSize(height, height);
    ui->fontComboBox->setFixedHeight(height - 1);
    ui->fontSizeComboBox->setFixedHeight(height - 1);
    ui->colorPickerWidget->setFixedHeight(height - 1);
    ui->boldButton->setFixedSize(height, height);
    ui->italicButton->setFixedSize(height, height);
    ui->underlineButton->setFixedSize(height, height);
    ui->strikeOutButton->setFixedSize(height, height);
    ui->calqlatrToggleButton->setFixedHeight(height);
}

void NoteBookWidget::updateSettings() {
    QFont font = this->ui->mainTextEdit->font();
    font.setPointSize(ProgramSettings::TEXTEDIT_FONT_SIZE.value());
    this->ui->mainTextEdit->setFont(font);
    this->ui->calqlatrWidget->setCalHistoryBaseFontSize(ProgramSettings::CALHISTORY_FONT_SIZE.value());
    this->ui->calqlatrWidget->setSciCalBaseFontSize(ProgramSettings::SCI_CALCULATOR_FONT_SIZE.value());
    updateFormatLineHeight();
}

/*
 *      Format Editing
 */

void NoteBookWidget::updateFormatDisplay(const QTextCharFormat &fmt) {
    QFont font = fmt.font();
    ui->fontComboBox->setCurrentFont(font);
    ui->fontSizeComboBox->setCurrentText(QString::number(font.pointSize()));
    ui->boldButton->setChecked(font.bold());
    ui->italicButton->setChecked(font.italic());
    ui->underlineButton->setChecked(font.underline());
    ui->strikeOutButton->setChecked(font.strikeOut());
}

bool NoteBookWidget::eventFilter(QObject *watched, QEvent *event) {
    if (ui->fmtBrushButton->isChecked() && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            ui->mainTextEdit->mergeCurrentCharFormat(fmtBrushFmt);
        }
        ui->fmtBrushButton->setChecked(false);
    }
    return QWidget::eventFilter(watched, event);
}


/*
 *      Saving & Loading
 */

QString NoteBookWidget::getDefaultDir() const {
    return ProgramSettings::LAST_USED_DIRECTORY.value();
}

bool NoteBookWidget::readFromFile(QFile &file) {
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QFileInfo info(file);
    ProgramSettings::LAST_USED_DIRECTORY.setValue(info.absoluteDir().path());

    if (info.completeSuffix() == "txt") {
        QTextStream stream(&file);
        QString str;
        stream >> str;
        this->ui->mainTextEdit->setPlainText(str);
        return stream.status() == QTextStream::Ok;
    } else {
        QDataStream stream(&file);
        this->ui->mainTextEdit->readFrom(stream);
        this->ui->calqlatrWidget->readFrom(stream);
        return stream.status() == QDataStream::Ok;
    }
}

bool NoteBookWidget::writeToFile(QFile &file) {
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream stream(&file);
    this->ui->mainTextEdit->writeTo(stream);
    this->ui->calqlatrWidget->writeTo(stream);

    QFileInfo info(file);
    ProgramSettings::LAST_USED_DIRECTORY.setValue(info.absoluteDir().path());

    return stream.status() == QDataStream::Ok;
}

bool NoteBookWidget::saveAsTxt(bool *discarded) {
    if (discarded != nullptr)
        *discarded = false;

    QFileInfo info(this->fileName());
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save As"),
        info.absoluteDir().path(),
        tr("Text files (*.txt);;All types(*.*)"));

    if (fileName.isEmpty()) {
        if (discarded != nullptr)
            *discarded = true;
        return false;
    }

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        qWarning() << "Failed to open" << fileName;
        return false;
    }

    QTextStream stream(&file);
    stream << this->ui->mainTextEdit->toPlainText();
    return stream.status() == QTextStream::Ok;
}

bool NoteBookWidget::saveFile(bool *discarded) {
    QFileInfo info(this->fileName());
    if (info.suffix() != "advcn")
        return this->saveFileAs(discarded);
    return AbstractFileEditor::saveFile(discarded);
}
