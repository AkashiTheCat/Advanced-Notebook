#ifndef READONLYLINEEDIT_H
#define READONLYLINEEDIT_H

#include <QLineEdit>
#include <QObject>
#include <QKeyEvent>

class ReadOnlyLineEdit : public QLineEdit {
    Q_OBJECT
public:
    ReadOnlyLineEdit(QWidget *parent = nullptr);

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void contextMenuEvent(QContextMenuEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
};

#endif // READONLYLINEEDIT_H
