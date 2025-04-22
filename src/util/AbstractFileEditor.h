#ifndef ABSTRACTFILEEDITOR_H
#define ABSTRACTFILEEDITOR_H

#include <QWidget>
#include <QFile>

// Abstract file editor widget class, provides file saving & loading logics
class AbstractFileEditor : public QWidget {
    Q_OBJECT
private:
    QString OpeningTitle = "Open";
    QString OpeningFilter = "All types (*.*)";

    QString SavingTitle = "Save As";
    QString SavingFilter = "All types (*.*)";

    QString RenamingTitle = "Rename";
    QString RenamingFilter = "All types (*.*)";

    QString FileName = "";
    bool Edited = true;

public:
    explicit AbstractFileEditor(QWidget *parent = nullptr);

    // Returns whether the file is a temporary file
    bool isTempFile() const;
    // Returns whether file is edited without saving.
    bool isFileEdited() const;
    // Returns the current file name (including path)
    const QString &fileName() const;
    // Set dialog title when opening file
    void setOpeningTitle(const QString &title);
    // Set file filter when opening file (see QFileDialog::getOpenFileName for example)
    void setOpeningFilter(const QString &filter);
    // Set dialog title when saving file
    void setSavingTitle(const QString &title);
    // Set file filter when saving file (see QFileDialog::getSaveFileName for example)
    void setSavingFilter(const QString &filter);
    // Set dialog title when renaming file
    void setRenamingTitle(const QString &title);
    // Set file filter when renaming file
    void setRenamingFilter(const QString &filter);

    // Create a file dialog and load the chosen file
    // if blacklist provided, cancel file loading when file name matched
    // matchedIndex will be -1 if not matched
    virtual bool loadFile(bool *discarded = nullptr, const QStringList &blackList = {},
        int *matchedIndex = nullptr);
    // Load from a designated file
    virtual bool loadFile(const QString &fileName);
    // Save current file or call saveFileAs() if current file is a new file
    virtual bool saveFile(bool *discarded = nullptr);
    // Create a file dialog and save to the chosen file
    virtual bool saveFileAs(bool *discarded = nullptr);
    // Create a file dialog and rename the current file
    virtual bool renameFile(bool *discarded = nullptr);

public slots:
    // Set file state to [edited]
    void setEdited(bool edited);

signals:
    // Emitted when current file name is going to change(not yet), new file name provided in parameter
    void fileNameChange(const QString &to);
    // Emitted when file state changes from saved to unsaved(new changes made) or unsaved to saved(file saved)
    void editStateChanged();

protected:
    // Returns the default file directory for the file dialog
    virtual QString getDefaultDir() const;
    // Pure virtual function. Called in loadFile()
    virtual bool readFromFile(QFile &file) = 0;
    // Pure virtual function. Called in saveFile() and saveFileAs()
    virtual bool writeToFile(QFile &file) = 0;
};

#endif // ABSTRACTFILEEDITOR_H
