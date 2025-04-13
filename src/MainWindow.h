#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "NoteBookWidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    quint32 newNoteCounter = 0;

    void handleNewWidgetConnection(NoteBookWidget *widget);
    // Return true if cancelled, otherwise return false
    bool hintSave(AbstractFileEditor *tab);

public slots:
    void updateActionState();
    void updateCurrentFile(int index);

    void loadOpeningFiles();
    void newFile();
    void openFile();
    void closeTab(int index);
    void saveCurrent();
    void saveCurrentAs();
    void saveCurrentAsTxt();
    void saveAll();
    void renameCurrent();

    void openSettingsWindow();

    void handleFileNameChange(const QString &to);
    void handleFileSaveStateChange();

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
