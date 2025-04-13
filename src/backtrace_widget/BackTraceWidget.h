#ifndef BACKTRACEWIDGET_H
#define BACKTRACEWIDGET_H

#include <QListWidget>
#include <QObject>
#include <QWidget>

class BackTraceWidget : public QListWidget {
    Q_OBJECT
private:
    int item_point_size;

public:
    BackTraceWidget(QWidget *parent = nullptr);

    void addItem(const QString &text, qint32 id);
    void insertItem(int row, const QString &text, qint32 id);
    qint32 getId(int row) const;
};

#endif // BACKTRACEWIDGET_H
