/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "util/tool.h"

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QRegExp>
#include <QTextStream>
#include <QWidget>
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/dialogutil.h"


/** Creates the file filename and put text into the file.
 */
bool util::tool::savePlainFile( const QString& filename, const QString& text, const bool& forceOverwrite, QTextCodec* fileCodec) {
    QFile saveFile(filename);
    bool ret;

    if (saveFile.exists()) {
        if (!forceOverwrite && util::showQuestion(0, QObject::tr("Overwrite File?"),
                QString::fromLatin1("<qt><b>%1</b><br/>%2</qt>")
                .arg( QObject::tr("The file already exists.") )
                .arg( QObject::tr("Do you want to overwrite it?")),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No) == QMessageBox::No
           ) {
            return false;
        }
        else { //either the user chose yes or forceOverwrite is set
            saveFile.remove();
        }
    }

    if ( saveFile.open(QIODevice::ReadWrite) ) {
        QTextStream textstream( &saveFile );
        textstream.setCodec(fileCodec);
        textstream << text;
        saveFile.close();
        ret = true;
    }
    else {
        QMessageBox::critical(0, QObject::tr("Error"),
                              QString::fromLatin1("<qt>%1<br/><b>%2</b></qt>")
                              .arg( QObject::tr("The file couldn't be saved.") )
                              .arg( QObject::tr("Please check permissions etc.")));
        saveFile.close();
        ret = false;
    }
    return ret;
}


/** Returns the icon used for the module given as aparameter. */
QIcon util::tool::getIconForModule( CSwordModuleInfo* module_info ) {
    namespace DU = util::directory;
    return DU::getIcon(getIconNameForModule(module_info));
}

/** Returns the name for the icon used for the module given as aparameter. */
QString util::tool::getIconNameForModule( CSwordModuleInfo* module_info ) {
    //qDebug() << "util::tool::getIconNameForModule";
    if (!module_info) return CResMgr::modules::book::icon_locked;

    if (module_info->category() == CSwordModuleInfo::Cult) {
        return "stop.svg";
    }

    switch (module_info->type()) {
        case CSwordModuleInfo::Bible:
            if (module_info->isLocked())
                return CResMgr::modules::bible::icon_locked;
            else
                return CResMgr::modules::bible::icon_unlocked;
            break;

        case CSwordModuleInfo::Lexicon:
            if (module_info->isLocked())
                return CResMgr::modules::lexicon::icon_locked;
            else
                return CResMgr::modules::lexicon::icon_unlocked;
            break;

        case CSwordModuleInfo::Commentary:
            if (module_info->isLocked())
                return CResMgr::modules::commentary::icon_locked;
            else
                return CResMgr::modules::commentary::icon_unlocked;
            break;

        case CSwordModuleInfo::GenericBook:
            if (module_info->isLocked())
                return CResMgr::modules::book::icon_locked;
            else
                return CResMgr::modules::book::icon_unlocked;
            break;

        case CSwordModuleInfo::Unknown: //fallback
        default:
            if (module_info->isLocked())
                return CResMgr::modules::book::icon_locked;
            else
                return CResMgr::modules::book::icon_unlocked;
            break;
    }
    return CResMgr::modules::book::icon_unlocked;
}

QLabel* util::tool::explanationLabel(QWidget *parent, const QString &heading, const QString &text) {
    QLabel *label = new QLabel(parent);
    initExplanationLabel(label, heading, text);
    return label;
}

void util::tool::initExplanationLabel(QLabel *label, const QString &heading, const QString &text) {
    QString br;
    if (!heading.isEmpty() && !text.isEmpty()) {
        br = QString::fromLatin1("<span style='white-space:pre'>  -  </span>");
    }
    label->setText(QString::fromLatin1("<b>%1</b>%2<small>%3</small>").arg(heading).arg(br).arg(text));
    label->setWordWrap(true);
    label->setMargin(1);
    label->setFrameStyle(QFrame::Box | QFrame::Sunken);
}

/** No descriptions */
bool util::tool::inHTMLTag(int pos, QString & text) {
    int i1 = text.lastIndexOf("<", pos);
    int i2 = text.lastIndexOf(">", pos);
    int i3 = text.indexOf(">", pos);
    int i4 = text.indexOf("<", pos);


    // if ((i1>0) && (i2==-1))  //we're in th first html tag
    //  i2=i1; // not ncessary, just for explanation

    if ((i3 > 0) && (i4 == -1))  //we're in the last html tag
        i4 = i3 + 1;

    //  qWarning("%d > %d && %d < %d",i1,i2,i3,i4);

    if ( (i1 > i2) && (i3 < i4) )
        return true; //yes, we're in a tag

    return false;
}

QString util::tool::remoteModuleToolTip(CSwordModuleInfo* module, QString localVer) {
    Q_ASSERT(module);
    if (!module) {
        return QString::null;
    }

    QString text;

    text = QString("<p style='white-space:pre'><b>%1</b> ").arg( module->name() )
           + ((module->category() == CSwordModuleInfo::Cult) ? QString::fromLatin1("<small><b>%1</b></small><br/>").arg(QObject::tr("Take care, this work contains cult / questionable material!")) : QString::null);

    text += QString("<small>(") + module->config(CSwordModuleInfo::Description) + QString(")</small><hr/>");

    if (module->isEncrypted()) {
        text += QObject::tr("Encrypted - needs unlock key") + QString("<br/>");
    }

    if (!localVer.isEmpty()) {
        text += QString("<b>") + QObject::tr("Updated version available!") + QString("</b><br/>");
    }

    if (module->hasVersion()) {
        text += QObject::tr("Version") + QString(": %1").arg( module->config(CSwordModuleInfo::ModuleVersion) );
    }
    // if installed already
    if (!localVer.isEmpty()) {
        text += QString("  ") + QObject::tr("Installed version") + QString(": %1").arg(localVer);
    }
    text += QString("<br/>");

    text += QString("<small>(") + QObject::tr("Double click for more information") + QString(")</small></p>");


    if (text.right(4) == QString::fromLatin1("<br/>")) {
        text = text.left(text.length() - 4);
    }

    return text;
}


int util::tool::mWidth(const QWidget* widget, int m) {
    if (widget) {
        return widget->fontMetrics().width(QString().fill('M', m));
    }
    return QApplication::fontMetrics().width(QString().fill('M', m));
}
