#ifndef NOTEBOOKSTATICPROPERTIES_H
#define NOTEBOOKSTATICPROPERTIES_H

#include <QObject>
#include <QTextFormat>

class NoteBookStaticProperties {
public:
    inline static const qint32 RECORD_OBJECT_TYPE = QTextFormat::UserObject;
    inline static const qint32 PROP_REC_OBJ_CALRECORD_INDEX = QTextFormat::UserProperty;
    inline static const qint32 PROP_REC_OBJ_SHOWRES_INDEX = QTextFormat::UserProperty + 1;
};

#endif // NOTEBOOKSTATICPROPERTIES_H
