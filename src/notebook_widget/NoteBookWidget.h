#ifndef NOTEBOOKWIDGET_H
#define NOTEBOOKWIDGET_H

#include <QWidget>
#include <QScreen>
#include <QTextCharFormat>

#include "CalqlatrRecord.h"
#include "AbstractFileEditor.h"
#include "./ui_NoteBookWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class NoteBookWidget;
}
QT_END_NAMESPACE

class NoteBookWidget : public AbstractFileEditor {
    Q_OBJECT

public:
    NoteBookWidget(QWidget *parent = nullptr);
    NoteBookWidget(QFile &file, QWidget *parent = nullptr);
    ~NoteBookWidget();

    // Save note as plain text. WARNING: LOSES ALL FORMATS AND CALCULATOR STATE
    bool saveAsTxt(bool *discarded = nullptr);

private:
    Ui::NoteBookWidget *ui;
    void initUI();

    QTextCharFormat fmtBrushFmt;

public slots:
    void insertCalqlatrRecord(const CalqlatrRecord &rec);
    void createTraceWindow(const QPoint &pos, qint32 id);

    void updateFormatDisplay(const QTextCharFormat &fmt);
    void updateFormatLineHeight();

    void updateSettings();

    // AbstractFileEditor interface
protected:
    virtual QString getDefaultDir() const override;
    virtual bool readFromFile(QFile &file) override;
    virtual bool writeToFile(QFile &file) override;

    // AbstractFileEditor interface
public:
    virtual bool saveFile(bool *discarded) override;

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;

};
#endif // NOTEBOOKWIDGET_H
