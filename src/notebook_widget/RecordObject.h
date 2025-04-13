#ifndef RECORDOBJECT_H
#define RECORDOBJECT_H

#include <QObject>
#include <QTextObjectInterface>

#include "CalqlatrRecord.h"
#include "NoteBookStaticProperties.h"

class RecordObject : public QObject, public QTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    static const int TYPE = NoteBookStaticProperties::RECORD_OBJECT_TYPE;
    explicit RecordObject(QObject *parent = nullptr);

    static QTextCharFormat getTextCharFormat(const CalqlatrRecord &rec);

    static QString toHtml(const QTextCharFormat &format);
    static CalqlatrRecord fromHtml(const QTextCharFormat &format);

signals:

    // QTextObjectInterface interface
public:
    virtual QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override;
    virtual void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument,
        const QTextFormat &format) override;
};

#endif // RECORDOBJECT_H
