#ifndef COLOREDITBOX_H
#define COLOREDITBOX_H

#include <QComboBox>
#include <QObject>

class ColorEditBox : public QComboBox {
    Q_OBJECT
private:
    QMenu *popUpMenu;

public:
    ColorEditBox(QWidget *parent = nullptr);

    void setMenu(QMenu *menu);

private slots:
    void hidePopupMenu();

    // QComboBox interface
public:
    virtual void showPopup() override;
    virtual void hidePopup() override;
};

#endif // COLOREDITBOX_H
