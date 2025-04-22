#include <QMessageBox>
#include <QFileInfo>
#include <QCloseEvent>
#include <QShortcut>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "NoteBookWidget.h"
#include "ProgramSettings.h"
#include "SettingsWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(this->ui->actionNew_File, &QAction::triggered, this, &MainWindow::newFile);
    connect(this->ui->actionOpen_File, &QAction::triggered, this, &MainWindow::openFile);
    connect(this->ui->actionRename, &QAction::triggered, this, &MainWindow::renameCurrent);
    connect(this->ui->actionSave_File, &QAction::triggered, this, &MainWindow::saveCurrent);
    connect(this->ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveCurrentAs);
    connect(this->ui->actionSave_As_txt, &QAction::triggered, this, &MainWindow::saveCurrentAsTxt);
    connect(this->ui->actionSave_All, &QAction::triggered, this, &MainWindow::saveAll);

    connect(this->ui->actionGeneral_Settings, &QAction::triggered, this, &MainWindow::openSettingsWindow);

    loadOpeningFiles();

    connect(this->ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(this->ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateCurrentFile);

    updateActionState();

    QShortcut *shortcutOpen = new QShortcut(QKeySequence("Ctrl+O"), this);
    QShortcut *shortcutSave = new QShortcut(QKeySequence("Ctrl+S"), this);
    QShortcut *shortcutSaveAs = new QShortcut(QKeySequence("Ctrl+Alt+S"), this);
    QShortcut *shortcutSaveAll = new QShortcut(QKeySequence("Ctrl+Shift+S"), this);

    connect(shortcutOpen, &QShortcut::activated, this, &MainWindow::openFile);
    connect(shortcutSave, &QShortcut::activated, this, &MainWindow::saveCurrent);
    connect(shortcutSaveAs, &QShortcut::activated, this, &MainWindow::saveCurrentAs);
    connect(shortcutSaveAll, &QShortcut::activated, this, &MainWindow::saveAll);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::handleNewWidgetConnection(NoteBookWidget *widget) {
    connect(widget, &NoteBookWidget::fileNameChange, this, &MainWindow::handleFileNameChange);
    connect(widget, &NoteBookWidget::editStateChanged, this, &MainWindow::handleFileEditStateChange);
}

bool MainWindow::hintSave(AbstractFileEditor *tab) {
    const QMessageBox::StandardButton &pressed = QMessageBox::question(this,
        tr("File Unsaved"),
        tr("Discard changes?"),
        QMessageBox::StandardButtons(
            QMessageBox::Save |
            QMessageBox::SaveAll |
            QMessageBox::Discard |
            QMessageBox::Cancel),
        QMessageBox::Save);

    switch (pressed) {
    case QMessageBox::Escape:
    case QMessageBox::Cancel:
        return true;
    case QMessageBox::SaveAll:
        this->saveAll();
        return true;
    case QMessageBox::Save:
        bool discarded;
        if (tab->saveFile(&discarded)) {
            break;
        }

        if (!discarded)
            QMessageBox::warning(this, tr("ERROR:"), tr("Failed to save file"));
        return false;
    default:
        break;
    }

    return false;
}

void MainWindow::updateActionState() {
    bool enabled = this->ui->tabWidget->count() > 0;
    this->ui->actionRename->setEnabled(enabled);
    this->ui->actionSave_All->setEnabled(enabled);
    this->ui->actionSave_File->setEnabled(enabled);
    this->ui->actionSave_As->setEnabled(enabled);
    this->ui->actionSave_As_txt->setEnabled(enabled);
}

void MainWindow::updateCurrentFile(int index) {
    if (index == -1)
        return;
    AbstractFileEditor *wTab = static_cast<AbstractFileEditor *>(this->ui->tabWidget->widget(index));
    if (wTab->fileName() != ProgramSettings::CURRENT_FILE_NAME.value())
        ProgramSettings::CURRENT_FILE_NAME.setValue(wTab->fileName());
}

void MainWindow::loadOpeningFiles() {
    QStringList list = ProgramSettings::OPENING_FILES.value();
    QStringList newList;
    for (int i = 0; i < list.length(); i++) {
        const QString &str = list.at(i);
        NoteBookWidget *widget = new NoteBookWidget(this);
        if (widget->loadFile(str)) {
            QFileInfo info(widget->fileName());
            handleNewWidgetConnection(widget);
            int index = this->ui->tabWidget->addTab(widget, info.fileName() + " ");
            if (ProgramSettings::CURRENT_FILE_NAME.value() == widget->fileName()) {
                this->ui->tabWidget->setCurrentIndex(index);
            }
            newList.append(list.at(i));
        } else {
            delete widget;
        }
    }
    ProgramSettings::OPENING_FILES.setValue(newList);
}

void MainWindow::newFile() {
    NoteBookWidget *widget = new NoteBookWidget(this);
    widget->setEdited(true);
    handleNewWidgetConnection(widget);
    int index = this->ui->tabWidget->addTab(widget, "NewNote" + QString::number(newNoteCounter++) + '*');
    this->ui->tabWidget->setCurrentIndex(index);
    updateActionState();
}

void MainWindow::openFile() {
    QStringList list = ProgramSettings::OPENING_FILES.value();
    NoteBookWidget *widget = new NoteBookWidget(this);
    bool discarded;
    int blackListIndex;
    if (!widget->loadFile(&discarded, list, &blackListIndex)) {
        if (discarded) {
            delete widget;
            return;
        }

        if (blackListIndex != -1) {
            const QString &name = list.at(blackListIndex);
            for (int i = 0; i < this->ui->tabWidget->count(); i++) {
                AbstractFileEditor *wTab = static_cast<AbstractFileEditor *>(
                    this->ui->tabWidget->widget(i));
                if (wTab->fileName() == name) {
                    this->ui->tabWidget->setCurrentIndex(i);
                }
            }
        } else {
            QMessageBox::warning(this, tr("ERROR"), tr("Failed to open file"));
        }

        delete widget;
        return;
    }


    if (list.contains(widget->fileName())) {
        delete widget;
        return;
    }

    QFileInfo info(widget->fileName());
    int index = this->ui->tabWidget->addTab(widget, info.fileName() + " ");
    this->ui->tabWidget->setCurrentIndex(index);
    handleNewWidgetConnection(widget);

    list.append(widget->fileName());
    ProgramSettings::OPENING_FILES.setValue(list);

    updateActionState();
}

void MainWindow::closeTab(int index) {
    AbstractFileEditor *wTab = static_cast<AbstractFileEditor *>(this->ui->tabWidget->widget(index));
    if (wTab->isFileEdited() && hintSave(wTab))
        return;

    QStringList list = ProgramSettings::OPENING_FILES.value();
    list.removeOne(wTab->fileName());
    ProgramSettings::OPENING_FILES.setValue(list);

    this->ui->tabWidget->removeTab(index);

    updateActionState();
}

void MainWindow::saveCurrent() {
    if (this->ui->tabWidget->count() == 0)
        return;
    AbstractFileEditor *wTab = static_cast<AbstractFileEditor *>(this->ui->tabWidget->currentWidget());
    bool discarded;
    if (wTab->isFileEdited() && !wTab->saveFile(&discarded) && !discarded) {
        QFileInfo info(wTab->fileName());
        QMessageBox::warning(this, tr("ERROR"), tr("Failed to save ") + info.fileName());
    }
}

void MainWindow::saveCurrentAs() {
    if (this->ui->tabWidget->count() == 0)
        return;
    AbstractFileEditor *wTab = static_cast<AbstractFileEditor *>(this->ui->tabWidget->currentWidget());
    bool discarded;
    if (!wTab->saveFileAs(&discarded) && !discarded) {
        QFileInfo info(wTab->fileName());
        QMessageBox::warning(this, tr("ERROR"), tr("Failed to save ") + info.fileName());
    }
}

void MainWindow::saveCurrentAsTxt() {
    if (this->ui->tabWidget->count() == 0)
        return;
    NoteBookWidget *wTab = static_cast<NoteBookWidget *>(this->ui->tabWidget->currentWidget());
    bool discarded;
    if (!wTab->saveAsTxt(&discarded) && !discarded) {
        QFileInfo info(wTab->fileName());
        QMessageBox::warning(this, tr("ERROR"), tr("Failed to save ") + info.fileName());
    }
}

void MainWindow::saveAll() {
    if (this->ui->tabWidget->count() == 0)
        return;
    QTabWidget *tabWidget = this->ui->tabWidget;
    AbstractFileEditor *wTab;
    for (int i = 0; i < tabWidget->count(); i++) {
        wTab = static_cast<AbstractFileEditor *>(tabWidget->widget(i));
        bool discarded;
        if (wTab->isFileEdited() && !wTab->saveFile(&discarded) && !discarded) {
            QFileInfo info(wTab->fileName());
            QMessageBox::warning(this, tr("ERROR"), tr("Failed to save ") + info.fileName());
            tabWidget->setCurrentIndex(i);
            return;
        }
    }
}

void MainWindow::renameCurrent() {
    if (this->ui->tabWidget->count() == 0)
        return;
    AbstractFileEditor *wTab = static_cast<AbstractFileEditor *>(this->ui->tabWidget->currentWidget());
    bool discarded;
    if (!wTab->renameFile(&discarded) && !discarded) {
        QFileInfo info(wTab->fileName());
        QMessageBox::warning(this, tr("ERROR"), tr("Failed to rename ") + info.fileName());
    }
}

void MainWindow::openSettingsWindow() {
    SettingsWidget *settingsWidget = new SettingsWidget(this);
    settingsWidget->setWindowFlags(Qt::Window);
    settingsWidget->setWindowTitle(tr("Settings"));
    settingsWidget->setAttribute(Qt::WA_DeleteOnClose);

    connect(settingsWidget, &SettingsWidget::settingsSaved, this, [this]() {
        QTabWidget *tabWidget = this->ui->tabWidget;
        for (int i = 0; i < tabWidget->count(); i++) {
            NoteBookWidget *widget = static_cast<NoteBookWidget *>(tabWidget->widget(i));
            widget->updateSettings();
        }
    });
    settingsWidget->show();
}

void MainWindow::handleFileNameChange(const QString &to) {
    AbstractFileEditor *wTab = static_cast<AbstractFileEditor *>(sender());
    QFileInfo info(to);

    QStringList list = ProgramSettings::OPENING_FILES.value();
    int index = list.indexOf(wTab->fileName());
    if (index != -1)
        list.replace(index, to);
    else
        list.append(to);
    ProgramSettings::OPENING_FILES.setValue(list);

    this->ui->tabWidget->setTabText(this->ui->tabWidget->indexOf(wTab),
        info.fileName() + (!wTab->isFileEdited() ? " " : "*"));
}

void MainWindow::handleFileEditStateChange() {
    AbstractFileEditor *wTab = static_cast<AbstractFileEditor *>(sender());
    QTabWidget *tabWidget = this->ui->tabWidget;
    int index = tabWidget->indexOf(wTab);
    tabWidget->setTabText(index, tabWidget->tabText(index).removeLast() + (wTab->isFileEdited() ? "*" : " "));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QTabWidget *tabWidget = this->ui->tabWidget;
    AbstractFileEditor *wTab;
    for (int i = 0; i < tabWidget->count(); i++) {
        wTab = static_cast<AbstractFileEditor *>(tabWidget->widget(i));
        if (wTab->isFileEdited()) {
            tabWidget->setCurrentIndex(i);
            if (hintSave(wTab)) {
                event->ignore();
                return;
            }
        }
    }
}
