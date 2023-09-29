/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "tool.h"

#include <QApplication>
#include <QChar>
#include <QFile>
#include <QFileDevice>
#include <QFontMetrics>
#include <QFrame>
#include <QIODevice>
#include <QLabel>
#include <QMessageBox>
#include <QObject>
#include <QTextStream>
#include <QWidget>
#include "../backend/drivers/cswordmoduleinfo.h"
#include "btassert.h"
#include "cresmgr.h"


namespace util {
namespace tool {

bool savePlainFile(const QString & filename,
                   void (&writer)(QTextStream &, void *),
                   void * userPtr)
{
    BT_ASSERT(!filename.isEmpty());

    QFile saveFile(filename);

    if (saveFile.exists())
        saveFile.remove();

    if (saveFile.open(QIODevice::ReadWrite)) {
        QTextStream textstream(&saveFile);
        writer(textstream, userPtr);
        textstream.flush();
        saveFile.close();
        if (saveFile.error() == QFile::NoError)
            return true;

        QMessageBox::critical(nullptr, QObject::tr("Error"),
                              QStringLiteral("<qt>%1<br/><b>%2</b></qt>")
                                  .arg(QObject::tr("Error while writing to file."))
                                  .arg(QObject::tr("Please check that enough disk space is available.")));
    }
    else {
        QMessageBox::critical(nullptr, QObject::tr("Error"),
                              QStringLiteral("<qt>%1<br/><b>%2</b></qt>")
                                  .arg(QObject::tr("The file couldn't be opened for saving."))
                                  .arg(QObject::tr("Please check permissions etc.")));
    }

    return false;
}

/** Creates the file filename and put text into the file.
 */
bool savePlainFile(QString const & filename, QString const & text)
{
    struct UserData { QString const & text; } userData{text};
    static auto const writer =
            +[](QTextStream & out, void * textPtr)
            { out << static_cast<const UserData *>(textPtr)->text; };
    return savePlainFile(filename, *writer, &userData);
}

QIcon const & getIconForModule(const CSwordModuleInfo * const module) {
    if (!module)
        return CResMgr::modules::book::icon_locked();

    if (module->category() == CSwordModuleInfo::Cult)
        return CResMgr::modules::icon_cult();

    switch (module->type()) {
        case CSwordModuleInfo::Bible:
            if (module->isLocked())
                return CResMgr::modules::bible::icon_locked();
            return CResMgr::modules::bible::icon_unlocked();

        case CSwordModuleInfo::Lexicon:
            if (module->isLocked())
                return CResMgr::modules::lexicon::icon_locked();
            return CResMgr::modules::lexicon::icon_unlocked();

        case CSwordModuleInfo::Commentary:
            if (module->isLocked())
                return CResMgr::modules::commentary::icon_locked();
            return CResMgr::modules::commentary::icon_unlocked();

        case CSwordModuleInfo::GenericBook:
            if (module->isLocked())
                return CResMgr::modules::book::icon_locked();
            return CResMgr::modules::book::icon_unlocked();

        case CSwordModuleInfo::Unknown: //fallback
        default:
            if (module->isLocked())
                return CResMgr::modules::book::icon_locked();
            return CResMgr::modules::book::icon_unlocked();
    }
}

void initExplanationLabel(QLabel * const label,
                          const QString & heading,
                          const QString & text)
{
    QString labelText;
    if (!heading.isEmpty())
        labelText += QStringLiteral("<b>%1</b>").arg(heading);

    if (!heading.isEmpty() && !text.isEmpty())
        labelText +=
                QStringLiteral("<span style=\"white-space:pre\">  -  </span>");

    if (!text.isEmpty())
        labelText += QStringLiteral("<small>%1</small>").arg(text);

    label->setText(labelText);
    label->setWordWrap(true);
    label->setMargin(1);
    label->setFrameStyle(QFrame::Box | QFrame::Sunken);
}

bool inHTMLTag(const int pos, const QString & text) {
    int i1 = text.lastIndexOf('<', pos);
    int i2 = text.lastIndexOf('>', pos);
    int i3 = text.indexOf('>', pos);
    int i4 = text.indexOf('<', pos);


    // if ((i1>0) && (i2==-1))  //we're in th first html tag
    //  i2=i1; // not ncessary, just for explanation

    if ((i3 > 0) && (i4 == -1))  //we're in the last html tag
        i4 = i3 + 1;

    //  qWarning("%d > %d && %d < %d",i1,i2,i3,i4);

    return (i1 > i2) && (i3 < i4);
}

int mWidth(QWidget const & widget, int const mCount)
{ return widget.fontMetrics().horizontalAdvance(QString(mCount, 'M')); }

QString fixSwordBcp47(QString input) {
    input.replace('_', '-');
    return input;
}

} // namespace tool {
} // namespace util {
