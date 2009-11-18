/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/profile/cprofile.h"

#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QTextStream>
#include "util/directory.h"


#define CURRENT_SYNTAX_VERSION 3

namespace Profile {

CProfile::CProfile( const QString& file, const QString& name )
        : m_name(name.isEmpty() ? QObject::tr("unknown") : name),
        m_filename(file),
        m_mdiArrangementMode((CMDIArea::MDIArrangementMode)0) { //0 is not a valid enum entry, means "unknown"
    namespace DU = util::directory;

    if (!m_filename.isEmpty() && name.isEmpty()) {
        loadBasics();
    }
    else if (m_filename.isEmpty() && !name.isEmpty()) {
        m_filename = name;
        m_filename.replace(QRegExp("\\s=#."), "_");
        m_filename = DU::getUserSessionsDir().absolutePath() + "/"  + m_filename + ".xml";
        init(m_filename);
    }
    else {
        qWarning("CProfile: empty file name!");
    }
}

CProfile::~CProfile() {
    qDeleteAll(m_profileWindows); //there's no autodelete feature in qt4
    m_profileWindows.clear();  //delete all CProfileWindows objects
}

/** Loads the profile from the file given in the constructor. */
QList<CProfileWindow*> CProfile::load() {
    QFile file(m_filename);
    if (!file.exists()) {
        //qWarning() << "Standard profile not found at filename " << m_filename;
        return QList<CProfileWindow*>();
    }

    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream t( &file );
        t.setCodec("UTF-8");
        doc.setContent(t.readAll());
        file.close();
    }

    QDomElement document = doc.documentElement();
    if ( document.tagName() != "BibleTimeProfile" && document.tagName() != "BibleTime" ) { //BibleTime was used in syntax version 1.0
        qWarning("CProfile::load: Missing BibleTime doc");
        return m_profileWindows;
    }
    if (document.hasAttribute("name")) {
        m_name = document.attribute("name");
    }

    //load settings of the main window
    {
        // see if there's a section with the name MAINWINDOW
        QDomElement elem = document.firstChild().toElement();
        QDomElement mainWindow;
        while (!elem.isNull()) {
            if (elem.tagName() == "MAINWINDOW") {
                mainWindow = elem;
                break; //found the element
            }
            elem = elem.nextSibling().toElement();
        }
        if (!mainWindow.isNull()) { //was found

            QByteArray bgeometry;
            bgeometry += mainWindow.attribute("geometry");
            setMainwindowGeometry(QByteArray::fromHex(bgeometry));

            QByteArray bstate;
            bstate += mainWindow.attribute("state");
            setMainwindowState(QByteArray::fromHex(bstate));

            QDomElement mdi_element = mainWindow.namedItem("MDI").toElement();
            if (!mdi_element.isNull()) {
                if (mdi_element.hasAttribute("ArrangementMode")) {
                    this->setMDIArrangementMode((CMDIArea::MDIArrangementMode)mdi_element.attribute("ArrangementMode").toInt());
                }
			}
		}
	}

    m_profileWindows.clear();
    QDomElement elem = document.firstChild().toElement();
    while (!elem.isNull()) {
        CProfileWindow* p = 0;
        if (elem.tagName() == "BIBLE") {
            p = new CProfileWindow(CSwordModuleInfo::Bible);
        }
        else if (elem.tagName() == "COMMENTARY") {
            p = new CProfileWindow(CSwordModuleInfo::Commentary);
        }
        else if (elem.tagName() == "LEXICON") {
            p = new CProfileWindow(CSwordModuleInfo::Lexicon);
        }
        else if (elem.tagName() == "BOOK") {
            p = new CProfileWindow(CSwordModuleInfo::GenericBook);
        }

        if (p) {
            m_profileWindows.append(p);

            if (elem.hasAttribute("windowSettings")) {
                p->setWindowSettings( elem.attribute("windowSettings").toInt() );
            }
            if (elem.hasAttribute("writeWindowType")) {
                p->setWriteWindowType( elem.attribute("writeWindowType").toInt() );
            }
            if (elem.hasAttribute("hasFocus")) {
                p->setFocus( static_cast<bool>(elem.attribute("hasFocus").toInt()) );
            }

            QRect rect;

            QDomElement object = elem.namedItem("GEOMETRY").toElement();
            if (!object.isNull()) {
                if (object.hasAttribute("x")) {
                    rect.setX(object.attribute("x").toInt());
                }
                if (object.hasAttribute("y")) {
                    rect.setY(object.attribute("y").toInt());
                }
                if (object.hasAttribute("width")) {
                    rect.setWidth(object.attribute("width").toInt());
                }
                if (object.hasAttribute("height")) {
                    rect.setHeight(object.attribute("height").toInt());
                }
                if (object.hasAttribute("isMaximized")) {
                    p->setMaximized( static_cast<bool>(object.attribute("isMaximized").toInt()) );
                }
            }
            p->setGeometry(rect);

            object = elem.namedItem("MODULES").toElement();
            if (!object.isNull()) {
                if (object.hasAttribute("list")) {
                    const QString sep = object.hasAttribute("separator") ? object.attribute("separator") : "|";
                    QStringList modules = object.attribute("list").split(sep);
                    p->setModules(modules);
                }
            }

            object = elem.namedItem("KEY").toElement();
            if (!object.isNull()) {
                if (object.hasAttribute("name"))
                    p->setKey(object.attribute("name"));
            }

            object = elem.namedItem("SCROLLBARS").toElement();
            if (!object.isNull()) {
                int horizontal = 0, vertical = 0;
                if (object.hasAttribute("horizontal"))
                    horizontal = object.attribute("horizontal").toInt();
                if (object.hasAttribute("vertical"))
                    vertical = object.attribute("vertical").toInt();

                p->setScrollbarPositions(horizontal, vertical);
            }
        }
        elem = elem.nextSibling().toElement();
    }
    return m_profileWindows;
}

/** Saves the profile to the file given in the constructor. */
bool CProfile::save(QList<CProfileWindow*> windows) {
    /** Save the settings using a XML file
    * Save the CProfileWindow objects using a XML file which name is in m_filename
    */
    bool ret = false;
    QDomDocument doc("DOC");
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement content = doc.createElement("BibleTimeProfile");
    content.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
    content.setAttribute("name", m_name);
    doc.appendChild(content);

    //save mainwindow settings
    {
        QDomElement mainWindow = doc.createElement("MAINWINDOW");

        QString sgeometry = QString(getMainwindowGeometry().toHex());
        mainWindow.setAttribute("geometry", sgeometry);

        QString sstate = QString(getMainwindowState().toHex());
        mainWindow.setAttribute("state", sstate);

        QDomElement mdi = doc.createElement("MDI");
        mainWindow.appendChild(mdi);
        mdi.setAttribute("ArrangementMode", static_cast<int>(this->getMDIArrangementMode()));

        content.appendChild(mainWindow);
    }

    //for (CProfileWindow* p = windows.first(); p; p = windows.next()) {
    foreach(CProfileWindow* p, windows) {
        QDomElement window;
        switch (p->type()) {
            case CSwordModuleInfo::Bible:
                window = doc.createElement("BIBLE");
                break;
            case CSwordModuleInfo::Commentary:
                window = doc.createElement("COMMENTARY");
                break;
            case CSwordModuleInfo::Lexicon:
                window = doc.createElement("LEXICON");
                break;
            case CSwordModuleInfo::GenericBook:
                window = doc.createElement("BOOK");
                break;
            default:
                break;
        }
        if (window.isNull())
            break;
        window.setAttribute("windowSettings", p->windowSettings());
        window.setAttribute("writeWindowType", p->writeWindowType());
        window.setAttribute("hasFocus", p->hasFocus());

        //save geomtery
        const QRect r = p->geometry();
        QDomElement geometry = doc.createElement("GEOMETRY");
        geometry.setAttribute("x", r.x());
        geometry.setAttribute("y", r.y());
        geometry.setAttribute("width", r.width());
        geometry.setAttribute("height", r.height());
        geometry.setAttribute("isMaximized", static_cast<int>(p->maximized()));
        window.appendChild( geometry );

        QDomElement modules = doc.createElement("MODULES");
        modules.setAttribute("separator", "|");
        modules.setAttribute("list", p->modules().join("|"));
        window.appendChild( modules );

        QDomElement key = doc.createElement("KEY");
        key.setAttribute("name", p->key());
        window.appendChild( key );

        QDomElement scrollbars = doc.createElement("SCROLLBARS");
        scrollbars.setAttribute("horizontal", p->scrollbarPositions().horizontal);
        scrollbars.setAttribute("vertical", p->scrollbarPositions().vertical);
        window.appendChild( scrollbars );

        content.appendChild( window );
    }

    QFile file(m_filename);
    if ( file.open(QIODevice::WriteOnly) ) {
        ret = true;
        QTextStream t( &file );
        t.setCodec("UTF-8");
        t << doc.toString();
        file.close();
    }
    else
        ret = false;

    return ret;
}

/** Saves the profile to the file given in the constructor. */
bool CProfile::save() {
    return save(m_profileWindows);
}

/** Returns the filename used for this profile. */
const QString& CProfile::filename() {
    return m_filename;
}

/** Returns the name of this profile. */
const QString& CProfile::name() {
    return m_name;
}

/** Initializes the XML for the first time (use to create a new profile) */
void CProfile::init(const QString file) {
    const QString oldFile = m_filename;
    m_filename = file;
    save(QList<CProfileWindow*>());
    m_filename = oldFile;
}

/** Changes the name of this profile. */
void CProfile::setName( const QString& newName ) {
    m_name = newName;
    saveBasics(); //save chanegd name
}

/** Loads the basic settings requires for proper operation. */
void CProfile::loadBasics() {
    QFile file(m_filename);
    if (!file.exists())
        return;

    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream t( &file );
        t.setCodec("UTF-8");
        doc.setContent(t.readAll());
        file.close();
    }
    QDomElement document = doc.documentElement();
    if (document.hasAttribute("name"))
        m_name = document.attribute("name");
}

void CProfile::saveBasics() {
    QFile file(m_filename);
    if (!file.exists())
        return;

    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream t(&file);
        t.setCodec("UTF-8");
        doc.setContent(t.readAll());
        file.close();
    }

    QDomElement document = doc.documentElement();
    document.setAttribute("name", m_name);

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream t( &file );
        t.setCodec("UTF-8");
        t << doc.toString();
        file.close();
    }
}

void CProfile::setMDIArrangementMode(const CMDIArea::MDIArrangementMode newArrangementMode) {
    m_mdiArrangementMode = newArrangementMode;
}

CMDIArea::MDIArrangementMode CProfile::getMDIArrangementMode(void) {
    return m_mdiArrangementMode;
}

void CProfile::setMainwindowGeometry(const QByteArray& geometry) {
    m_mainwindowGeometry = geometry;
}

QByteArray CProfile::getMainwindowGeometry() {
    return m_mainwindowGeometry;
}

void CProfile::setMainwindowState(const QByteArray& state) {
    m_mainwindowState = state;
}

QByteArray CProfile::getMainwindowState() {
    return m_mainwindowState;
}


} //end of namespace Profile
