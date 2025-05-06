#include <QFontMetrics>
#include <QPainter>

#include "RecordObject.h"
#include "CalqlatrRecord.h"
#include "NoteBookStaticProperties.h"

#define CALREC_PROP NoteBookStaticProperties::PROP_REC_OBJ_CALRECORD_INDEX
#define SHOWRES_PROP NoteBookStaticProperties::PROP_REC_OBJ_SHOWRES_INDEX

RecordObject::RecordObject(QObject *parent)
    : QObject(parent) {
}

QTextCharFormat RecordObject::getTextCharFormat(const CalqlatrRecord &rec) {
    QTextCharFormat format;
    format.setObjectType(RecordObject::TYPE);
    format.setVerticalAlignment(QTextCharFormat::AlignBottom);
    format.setProperty(CALREC_PROP, QVariant::fromValue(rec));
    format.setProperty(SHOWRES_PROP, true);
    return format;
}

QSizeF RecordObject::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) {
    Q_UNUSED(doc);
    Q_UNUSED(posInDocument);

    const CalqlatrRecord& rec = format.property(CALREC_PROP).value<CalqlatrRecord>();
    bool showResult = format.property(SHOWRES_PROP).toBool();
    QString text = showResult ? QString::number(rec.getResult()) : rec.getText();

    QFontMetrics fm(format.toCharFormat().font());
    int textWidth = fm.horizontalAdvance(text);
    int textHeight = fm.height();
    int iconSize = textHeight;

    return QSizeF(textWidth + iconSize, textHeight);
}

void RecordObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument,
    const QTextFormat &format) {
    Q_UNUSED(doc);
    Q_UNUSED(posInDocument);

    const CalqlatrRecord& rec = format.property(CALREC_PROP).value<CalqlatrRecord>();
    bool showResult = format.property(SHOWRES_PROP).toBool();
    QString text = showResult ? QString::number(rec.getResult()) : rec.getText();

    QFont font = format.toCharFormat().font();
    font.setUnderline(true);
    painter->setFont(font);
    painter->setPen(QColor(Qt::darkCyan));
    QFontMetrics fm(font);

    int textHeight = fm.height();

    painter->drawText(rect, Qt::AlignBaseline, text);

    QPixmap pixmap(":/assets/icon/calqlatr.svg");
    if (!pixmap.isNull()) {
        QRectF imgRect(rect.right() - textHeight, rect.top(), textHeight, textHeight);

        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->drawPixmap(imgRect, pixmap, pixmap.rect());
    }
}
