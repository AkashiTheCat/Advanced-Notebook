#ifndef ADVTEXTEDIT_H
#define ADVTEXTEDIT_H

#include <QObject>
#include <QTextEdit>

#include "CalqlatrRecord.h"

class AdvTextEdit : public QTextEdit {
    Q_OBJECT
public:
    AdvTextEdit(QWidget *parent = nullptr);

    void writeTo(QDataStream &in) const;
    void readFrom(QDataStream &out);

    QString toPlainText() const;

    void setFmtBrushMode(bool enabled);

private:
    bool fmtBrush = false;

    QFont fontAtHead;

    static const int BLOCK_END_FLAG = -127;
    static const int DOCUMENT_END_FLAG = -255;

    QRect getObjectRectForPos(const QPoint &pos);
    QRect getObjectRectForPos(const QPoint &pos, QTextCursor &CursorAtRectR);

signals:
    void recordLoadingRequested(const CalqlatrRecord &rec);
    void traceWindowRequested(const QPoint &pos, qint32 id);

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // ADVTEXTEDIT_H
