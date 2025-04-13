#include "AbstractFileEditor.h"

#include <QFileDialog>
#include <QObject>
#include <QDir>

AbstractFileEditor::AbstractFileEditor(QWidget *parent) : QWidget(parent) {}

bool AbstractFileEditor::isTempFile() const { return this->FileName.isEmpty(); }

bool AbstractFileEditor::loadFile(bool *discarded, const QStringList &blackList, int *matchedIndex) {
    if (discarded != nullptr)
        *discarded = false;
    if (matchedIndex != nullptr)
        *matchedIndex = -1;

    QString fileName = QFileDialog::getOpenFileName(this,
        this->OpeningTitle,
        getDefaultDir(),
        this->OpeningFilter);

    if (fileName.isEmpty()) {
        if (discarded != nullptr)
            *discarded = true;
        return false;
    }

    int index = blackList.indexOf(fileName);
    if (index != -1) {
        if (matchedIndex != nullptr)
            *matchedIndex = index;
        return false;
    }

    return loadFile(fileName);
}

bool AbstractFileEditor::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!readFromFile(file)) {
        qWarning() << "Failed to open" << fileName;
        return false;
    }

    emit fileNameChange(fileName);
    this->FileName = fileName;

    this->Saved = true;
    emit saveStateChanged();

    return true;
}

bool AbstractFileEditor::saveFileAs(bool *discarded) {
    if (discarded != nullptr)
        *discarded = false;

    QString fileName = QFileDialog::getSaveFileName(this,
        this->SavingTitle,
        getDefaultDir(),
        this->SavingFilter);

    if (fileName.isEmpty()) {
        if (discarded != nullptr)
            *discarded = true;
        return false;
    }

    QFile file(fileName);
    if (!writeToFile(file)) {
        qWarning() << "Failed to save to" << fileName;
        return false;
    }

    emit fileNameChange(fileName);
    this->FileName = fileName;


    this->Saved = true;
    emit saveStateChanged();

    return true;
}

bool AbstractFileEditor::saveFile(bool *discarded) {
    if (discarded != nullptr)
        *discarded = false;

    if (this->FileName.isEmpty()) {
        return saveFileAs(discarded);
    }

    QFile file(this->FileName);
    if (!writeToFile(file)) {
        qWarning() << "Failed to save to" << this->FileName;
        return false;
    }

    this->Saved = true;
    emit saveStateChanged();

    return true;
}

bool AbstractFileEditor::renameFile(bool *discarded) {
    if (discarded != nullptr)
        *discarded = false;

    QFile file(this->fileName());
    QFileInfo info(file);

    QString fileName = QFileDialog::getSaveFileName(this,
        this->RenamingTitle,
        info.filePath(),
        this->RenamingFilter);

    if (fileName.isEmpty()) {
        if (discarded != nullptr)
            *discarded = true;
        return false;
    }

    if (!file.rename(fileName)) {
        return false;
    }

    emit fileNameChange(fileName);
    this->FileName = fileName;

    return true;
}

bool AbstractFileEditor::isFileSaved() const { return this->Saved; }

void AbstractFileEditor::setEdited() {
    this->Saved = false;
    emit saveStateChanged();
}

const QString &AbstractFileEditor::fileName() const { return this->FileName; }

void AbstractFileEditor::setOpeningTitle(const QString &title) { this->OpeningTitle = title; }

void AbstractFileEditor::setOpeningFilter(const QString &filter) { this->OpeningFilter = filter; }

void AbstractFileEditor::setSavingTitle(const QString &title) { this->SavingTitle = title; }

void AbstractFileEditor::setSavingFilter(const QString &filter) { this->SavingFilter = filter; }

void AbstractFileEditor::setRenamingTitle(const QString &title) { this->RenamingTitle = title; }

void AbstractFileEditor::setRenamingFilter(const QString &filter) { this->RenamingFilter = filter; }

QString AbstractFileEditor::getDefaultDir() const { return QDir::homePath(); }
