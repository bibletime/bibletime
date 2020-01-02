/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "tool.h"

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QRegExp>
#include <QTextStream>
#include <QtGlobal>
#include <QWidget>
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../backend/managers/cswordbackend.h"
#include "../frontend/messagedialog.h"
#include "btassert.h"
#include "cresmgr.h"
#include "directory.h"


namespace util {
namespace tool {


/** Creates the file filename and put text into the file.
 */
bool savePlainFile(const QString & filename,
                   const QString & text,
                   QTextCodec * const fileCodec)
{
    BT_ASSERT(fileCodec);
    BT_ASSERT(!filename.isEmpty());

    QFile saveFile(filename);

    if (saveFile.exists())
        saveFile.remove();

    if (saveFile.open(QIODevice::ReadWrite)) {
        QTextStream textstream(&saveFile);
        textstream.setCodec(fileCodec);
        textstream << text;
        textstream.flush();
        saveFile.close();
        if (saveFile.error() == QFile::NoError)
            return true;

        QMessageBox::critical(nullptr, QObject::tr("Error"),
                              QString::fromLatin1("<qt>%1<br/><b>%2</b></qt>")
                                  .arg(QObject::tr("Error while writing to file."))
                                  .arg(QObject::tr("Please check that enough disk space is available.")));
    }
    else {
        QMessageBox::critical(nullptr, QObject::tr("Error"),
                              QString::fromLatin1("<qt>%1<br/><b>%2</b></qt>")
                                  .arg(QObject::tr("The file couldn't be opened for saving."))
                                  .arg(QObject::tr("Please check permissions etc.")));
    }

    return false;
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

QLabel * explanationLabel(QWidget * const parent,
                          const QString & heading,
                          const QString & text)
{
    QLabel * const label = new QLabel(parent);
    initExplanationLabel(label, heading, text);
    return label;
}

void initExplanationLabel(QLabel * const label,
                          const QString & heading,
                          const QString & text)
{
    QString labelText;
    if (!heading.isEmpty()) {
        labelText += "<b>";
        labelText += heading;
        labelText += "</b>";
    }
    if (!heading.isEmpty() && !text.isEmpty()) {
        labelText += "<span style=\"white-space:pre\">  -  </span>";
    }
    if (!text.isEmpty()) {
        labelText += "<small>";
        labelText += text;
        labelText += "</small>";
    }
    label->setText(labelText);
    label->setWordWrap(true);
    label->setMargin(1);
    label->setFrameStyle(QFrame::Box | QFrame::Sunken);
}

bool inHTMLTag(const int pos, const QString & text) {
    int i1 = text.lastIndexOf("<", pos);
    int i2 = text.lastIndexOf(">", pos);
    int i3 = text.indexOf(">", pos);
    int i4 = text.indexOf("<", pos);


    // if ((i1>0) && (i2==-1))  //we're in th first html tag
    //  i2=i1; // not ncessary, just for explanation

    if ((i3 > 0) && (i4 == -1))  //we're in the last html tag
        i4 = i3 + 1;

    //  qWarning("%d > %d && %d < %d",i1,i2,i3,i4);

    return (i1 > i2) && (i3 < i4);
}

QString remoteModuleToolTip(const CSwordModuleInfo & module,
                            const QString & localVer)
{
    QString text = "<p style='white-space:pre'><b>";
    text += module.name();
    text += "</b> ";

    if (module.category() == CSwordModuleInfo::Cult) {
        text += "<small><b>";
        text += QObject::tr("Take care, this work contains cult / questionable "
                            "material!");
        text += "</b></small><br/>";
    }

    text += "<small>(";
    text += module.config(CSwordModuleInfo::Description);
    text += ")</small><hr/>";

    if (module.isEncrypted()) {
        text += QObject::tr("Encrypted - needs unlock key");
        text += "<br/>";
    }

    if (!localVer.isEmpty()) {
        text += "<b>";
        text += QObject::tr("Updated version available!");
        text += "</b><br/>";
    }

    if (module.hasVersion()) {
        text += QObject::tr("Version");
        text += ": ";
        text += module.config(CSwordModuleInfo::ModuleVersion);
    }

    // if installed already
    if (!localVer.isEmpty()) {
        text += "  ";
        text += QObject::tr("Installed version");
        text += ": ";
        text += localVer;
    }
    text += "<br/><small>(";
    text += QObject::tr("Double click for more information");
    text += ")</small></p>";

    return text;
}


int mWidth(const QWidget * const widget, const int mCount) {
    const QString mString(mCount, 'M');
    #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    if (widget)
        return widget->fontMetrics().horizontalAdvance(mString);
    return QApplication::fontMetrics().horizontalAdvance(mString);
    #else
    if (widget)
        return widget->fontMetrics().width(mString);
    return QApplication::fontMetrics().width(mString);
    #endif
}


} // namespace tool {
} // namespace util {
