#include <QIntValidator>

#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"
#include "ProgramSettings.h"

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent), ui(new Ui::SettingsWidget) {
    ui->setupUi(this);
    QValidator *validator = new QIntValidator(5, 255, this);
    ui->textEditorFontSizeEdit->setValidator(validator);
    ui->fmtEditorHeightEdit->setValidator(validator);
    ui->sciCalFontSizeEdit->setValidator(validator);
    ui->calHistoryFontSizeEdit->setValidator(validator);

    ui->textEditorFontSizeEdit->setText(ProgramSettings::TEXTEDIT_FONT_SIZE.toString());
    ui->fmtEditorHeightEdit->setText(ProgramSettings::FORMAT_EDIT_LINE_HEIGHT.toString());
    ui->sciCalFontSizeEdit->setText(ProgramSettings::SCI_CALCULATOR_FONT_SIZE.toString());
    ui->calHistoryFontSizeEdit->setText(ProgramSettings::CALHISTORY_FONT_SIZE.toString());

    connect(ui->saveButton, &QPushButton::clicked, this, [this]() {
        ProgramSettings::TEXTEDIT_FONT_SIZE.setValue(ui->textEditorFontSizeEdit->text().toInt());
        ProgramSettings::FORMAT_EDIT_LINE_HEIGHT.setValue(ui->fmtEditorHeightEdit->text().toInt());
        ProgramSettings::SCI_CALCULATOR_FONT_SIZE.setValue(ui->sciCalFontSizeEdit->text().toInt());
        ProgramSettings::CALHISTORY_FONT_SIZE.setValue(ui->calHistoryFontSizeEdit->text().toInt());
        emit settingsSaved();
        this->close();
    });
}

SettingsWidget::~SettingsWidget() {
    delete ui;
}
