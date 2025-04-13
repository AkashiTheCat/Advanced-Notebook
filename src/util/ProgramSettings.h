#ifndef PROGRAMSETTINGS_H
#define PROGRAMSETTINGS_H

#include <QFile>
#include <QDir>
#include <QJsonObject>

#include "ConfigFile.h"
#include "ConfigObject.h"

class ProgramSettings {
public:
    // Program Info
    inline static ConfigFile PROGRAM_INFO_FILE = ConfigFile("program_info.json");

    inline static ConfigObject LAST_USED_DIRECTORY = ConfigObject<QString>(PROGRAM_INFO_FILE, "lastDir", QDir::homePath());
    inline static ConfigObject OPENING_FILES = ConfigObject<QStringList>(PROGRAM_INFO_FILE, "openingFiles", {});
    inline static ConfigObject CURRENT_FILE_NAME = ConfigObject<QString>(PROGRAM_INFO_FILE, "currentFile", "");

    // User settings
    inline static ConfigFile USER_SETTINGS_FILE = ConfigFile("user_settings.json");

    inline static ConfigObject FORMAT_EDIT_LINE_HEIGHT = ConfigObject<int>(USER_SETTINGS_FILE, "formatEditLineHeight", 24);
    inline static ConfigObject TEXTEDIT_FONT_SIZE = ConfigObject<int>(USER_SETTINGS_FILE, "textEditFontSize", 12);
    inline static ConfigObject TRACEWINDOW_FONT_SIZE = ConfigObject<int>(USER_SETTINGS_FILE, "traceWindowFontSize", 12);
    inline static ConfigObject CALHISTORY_FONT_SIZE = ConfigObject<int>(USER_SETTINGS_FILE, "calHistoryFontSize", 16);
    inline static ConfigObject SCI_CALCULATOR_FONT_SIZE = ConfigObject<int>(USER_SETTINGS_FILE, "sciCalqlatrFontSize", 16);
};

#endif // PROGRAMSETTINGS_H
