#include <QMouseEvent>
#include <QTextBlock>
#include <QMenu>
#include <QTextDocumentWriter>
#include <QListWidget>

#include "AdvTextEdit.h"
#include "RecordObject.h"

#define CALREC_PROP  NoteBookStaticProperties::PROP_REC_OBJ_CALRECORD_INDEX
#define SHOWREC_PROP NoteBookStaticProperties::PROP_REC_OBJ_SHOWRES_INDEX

AdvTextEdit::AdvTextEdit(QWidget *parent)
    : QTextEdit(parent) {
    QFont font = this->currentCharFormat().font();
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setStrikeOut(false);
    this->setCurrentFont(font);
}

QString AdvTextEdit::toPlainText() const {
    QTextDocument *doc = this->document();
    QString res;
    res.reserve(doc->characterCount());

    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        QTextBlock::Iterator iterator = block.begin();
        while (!iterator.atEnd()) {
            QTextFragment fragment = iterator.fragment();
            iterator++;
            QTextCharFormat fmt = fragment.charFormat();
            QString tmp = " ";
            switch (fmt.objectType()) {
            case RecordObject::TYPE:
                tmp.append(fmt.property(NoteBookStaticProperties::CALREC_PROP)
                        .value<CalqlatrRecord>()
                        .getText());
                tmp.append(' ');
                res.append(tmp.repeated(fragment.length()));
                break;
            default:
                res.append(fragment.text());
                break;
            }
        }
        res.append('\n');
    }

    return res;
}

void AdvTextEdit::setFmtBrushMode(bool enabled) {
    this->fmtBrush = enabled;
}

void AdvTextEdit::writeTo(QDataStream &out) const {
    QTextDocument *doc = this->document();
    // write formats
    QList<QTextFormat> fmts = doc->allFormats();
    out << fmts;

    // write text blocks
    QTextBlock block = doc->begin();
    while (block.isValid()) {
        out << block.blockFormatIndex();
        QTextBlock::Iterator iterator = block.begin();
        while (!iterator.atEnd()) {
            QTextFragment fragment = iterator.fragment();
            out << fragment.charFormatIndex() << fragment.text();
            iterator++;
        }
        out << BLOCK_END_FLAG;
        block = block.next();
    }
    out << DOCUMENT_END_FLAG;
}

void AdvTextEdit::readFrom(QDataStream &in) {
    QTextDocument *doc = this->document();
    doc->clear();
    QTextCursor docCursor(doc);

    // read formats
    QList<QTextFormat> fmts;
    in >> fmts;

    // read document
    int index;
    in >> index;
    docCursor.setBlockFormat(fmts.at(index).toBlockFormat());

    while (!in.atEnd()) {
        in >> index;
        switch (index) {
        case BLOCK_END_FLAG:
            in >> index;
            if (index == DOCUMENT_END_FLAG) {
                qDebug() << "Document Reading completed";
                return;
            }
            docCursor.insertBlock(fmts.at(index).toBlockFormat());
            break;
        case DOCUMENT_END_FLAG:
            qWarning() << "ERROR: Document reading end without text block end flag";
            doc->clear();
            return;
        default:
            QString text;
            in >> text;
            docCursor.insertText(text, fmts.at(index).toCharFormat());
            break;
        }
    }

    doc->clear();
    qWarning() << "ERROR: Data stream end reached without document end flag";
}

QRect AdvTextEdit::getObjectRectForPos(const QPoint &pos) {
    int x = pos.x();
    int y = pos.y();

    QTextCursor c0 = this->cursorForPosition(pos);
    QRect rect0 = cursorRect(c0);
    if (y < rect0.top() || y > rect0.bottom())
        return QRect();

    QTextCursor c1 = c0;
    QTextCursor c2 = c0;
    if (!c1.atBlockStart())
        c1.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
    if (!c2.atBlockEnd())
        c2.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);

    QRect rect1 = cursorRect(c1).united(rect0);
    if (rect1.width() > 1 && rect1.left() <= x && x <= rect1.right())
        return rect1;

    rect0 = cursorRect(c2).united(rect0);
    if (rect0.width() > 1 && rect0.left() <= x && x <= rect0.right())
        return rect0;

    return QRect();
}

QRect AdvTextEdit::getObjectRectForPos(const QPoint &pos, QTextCursor &CursorAtRectR) {
    CursorAtRectR.setPosition(-1);

    int x = pos.x();
    int y = pos.y();

    QTextCursor c0 = this->cursorForPosition(pos);
    QRect rect0 = cursorRect(c0);
    if (y < rect0.top() || y > rect0.bottom())
        return QRect();

    QTextCursor c1 = c0;
    QTextCursor c2 = c0;
    if (!c1.atBlockStart()) {
        c1.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
    }
    if (!c2.atBlockEnd()) {
        c2.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    }

    QRect rect1 = cursorRect(c1).united(rect0);
    if (rect1.width() > 1 && rect1.left() <= x && x <= rect1.right()) {
        CursorAtRectR = c0;
        CursorAtRectR.clearSelection();
        return rect1;
    }

    rect0 = cursorRect(c2).united(rect0);
    if (rect0.width() > 1 && rect0.left() <= x && x <= rect0.right()) {
        CursorAtRectR = c2;
        CursorAtRectR.clearSelection();
        return rect0;
    }

    return QRect();
}

void AdvTextEdit::mousePressEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    QTextCursor cursor;
    QRect rect = getObjectRectForPos(pos, cursor);
    QTextCharFormat fmt = cursor.charFormat();

    if (!rect.isNull() && fmt.objectType() == RecordObject::TYPE) {
        if (event->button() == Qt::LeftButton) {
            cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            fmt.setProperty(SHOWREC_PROP, !fmt.property(SHOWREC_PROP).toBool());
            cursor.setCharFormat(fmt);
            document()->markContentsDirty(cursor.position(), 1);
            return;
        }
        if (event->button() == Qt::RightButton) {
            QMenu *contextMenu = new QMenu(this);
            contextMenu->setAttribute(Qt::WA_DeleteOnClose);

            QAction *loadAction = contextMenu->addAction(tr("Load"));
            connect(loadAction, &QAction::triggered, this, [=] {
                CalqlatrRecord rec = fmt.property(CALREC_PROP).value<CalqlatrRecord>();
                emit recordLoadingRequested(rec);
            });

            QAction *traceAction = contextMenu->addAction(tr("Trace"));
            connect(traceAction, &QAction::triggered, this, [=] {
                CalqlatrRecord rec = fmt.property(CALREC_PROP).value<CalqlatrRecord>();
                emit traceWindowRequested(event->globalPosition().toPoint(), rec.id());
            });

            contextMenu->popup(event->globalPosition().toPoint());
            event->accept();
            return;
        }
    }

    QTextEdit::mousePressEvent(event);
}

void AdvTextEdit::mouseMoveEvent(QMouseEvent *event) {
    if (fmtBrush) {
        QTextEdit::mouseMoveEvent(event);
        return;
    }

    QPoint mousePos = event->pos();
    QTextCursor cursor;
    QRect rect = getObjectRectForPos(mousePos, cursor);

    if (!rect.isNull() && cursor.charFormat().objectType() == RecordObject::TYPE) {
        if (viewport()->cursor().shape() != Qt::PointingHandCursor)
            viewport()->setCursor(Qt::PointingHandCursor);
    } else if (viewport()->cursor().shape() != Qt::IBeamCursor) {
        viewport()->setCursor(Qt::IBeamCursor);
    }
    QTextEdit::mouseMoveEvent(event);
}
