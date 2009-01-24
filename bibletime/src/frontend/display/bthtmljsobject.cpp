/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/
#include "bthtmljsobject.h"
#include "bthtmlreaddisplay.h"
#include "frontend/cinfodisplay.h"
#include "frontend/cdragdrop.h"
#include "backend/managers/creferencemanager.h"
#include "backend/managers/cswordbackend.h"
#include "backend/keys/cswordkey.h"
#include "backend/config/cbtconfig.h"
#include <boost/scoped_ptr.hpp>
#include <QObject>

using namespace InfoDisplay;

// This class works along with the BtHtmlReadDisplay class. The BtHtmlReadDisplay class loads
// Javascript (bthtml.js) along with the html it displays. This class is added to the Javascript model
// so that Javascript can call this class, or this class can call Javascript.
// Access to DOM objects is implemented in Javascript and is communicated back to c++ through this class

BtHtmlJsObject::BtHtmlJsObject(BtHtmlReadDisplay* display) 
	: m_display(display) 
{
}

void BtHtmlJsObject::moveToAnchor(const QString& anchor)
{
	// Call gotoAnchor in Javascript
	emit gotoAnchor(anchor);
}

void BtHtmlJsObject::mouseDownLeft(const QString& url, const int& x, const int& y)
{
	m_dndData.mousePressed = true;
	m_dndData.isDragging = false;
	m_dndData.startPos = QPoint(x,y);
	m_dndData.url = url;
}
 
void BtHtmlJsObject::mouseClick(const QString& url)
{
	m_dndData.mousePressed = false;
	if (!url.isEmpty() && CReferenceManager::isHyperlink(url)) 
	{
		QString module;
		QString key;
		CReferenceManager::Type type;

		CReferenceManager::decodeHyperlink(url, module, key, type);
		if (module.isEmpty()) 
		{
			module = CReferenceManager::preferredModule( type );
		}
		m_display->connectionsProxy()->emitReferenceClicked(module,key);
	}
}

void BtHtmlJsObject::mouseDownRight(const QString& url)
{
	m_display->setActiveAnchor(url);
}

void BtHtmlJsObject::mouseMoveEvent(const QString& /*attributes*/, const int& x, const int& y, const bool& shiftKey)
{
	if(!m_dndData.isDragging && m_dndData.mousePressed) 
	{
		QPoint current(x,y);
		if ((current - m_dndData.startPos).manhattanLength() > 4 /*qApp->startDragDistance()*/ ) 
		{
			QDrag* d = 0;
			if (m_dndData.url.size() != 0 ) 
			{
				// create a new bookmark drag!
				QString moduleName = QString::null;
				QString keyName = QString::null;
				CReferenceManager::Type type;
				if ( !CReferenceManager::decodeHyperlink(m_dndData.url, moduleName, keyName, type) )
					return;
				d = new QDrag(m_display->view());
				BTMimeData* mimedata = new BTMimeData(moduleName, keyName, QString::null);
				d->setMimeData(mimedata);
				//add real Bible text from module/key
				if (CSwordModuleInfo* module = CPointers::backend()->findModuleByName(moduleName)) 
				{
					boost::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );
					key->key( keyName );
					mimedata->setText(key->strippedText()); // This works across applications!
				}
			}
#if 0
			else if ((m_dndData.dragType == DNDData::Text) && !m_dndData.selection.isEmpty()) {
				d = new QDrag(KHTMLPart::view()->viewport());
				BTMimeData* mimedata = new BTMimeData(m_dndData.selection);
				d->setMimeData(mimedata);
			}
#endif
			if (d) {
				m_dndData.isDragging = true;
				m_dndData.mousePressed = false;

				//first make a virtual mouse click to end the selection, if it's in progress
//				QMouseEvent e(QEvent::MouseButtonRelease, QPoint(0,0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
//				QApplication::sendEvent(view()->viewport(), &e);
				d->exec(Qt::CopyAction, Qt::CopyAction);

			}
		}
	}
	else if (m_display->getMouseTracking() &&  !shiftKey) 
	{
		//no mouse button pressed and tracking enabled
		// After some millisecs the new timer activates the Mag window update, see timerEvent()
		// QObject has simple timer
		emit startTimer(CBTConfig::get(CBTConfig::magDelay));
	}
}

void BtHtmlJsObject::timeOutEvent(const QString& attributes)
{
	CInfoDisplay::ListInfoData infoList;
	QStringList attrList = attributes.split("|");
	for (int i=0; i<attrList.count(); i++)
	{
		QString attrPair = attrList[i];
		QStringList attr = attrPair.split("=");
		if (attr.count() == 2)
		{
			QString attrName = attr[0];
			QString attrValue= attr[1];
			if (attrName == "note") 
			{
				infoList.append( qMakePair(CInfoDisplay::Footnote, attrValue));
			}
			if (attrName == "lemma") 
			{
				infoList.append( qMakePair(CInfoDisplay::Lemma, attrValue));
			}
			if (attrName == "morph") 
			{
				infoList.append( qMakePair(CInfoDisplay::Morph, attrValue));
			}
			if (attrName == "expansion") 
			{
				infoList.append( qMakePair(CInfoDisplay::Abbreviation, attrValue));
			}
			if (attrName == "crossrefs") 
			{
				infoList.append( qMakePair(CInfoDisplay::CrossReference, attrValue));
			}
		}
	}
	// Update the mag if there is new content
	if (!(infoList.isEmpty())) 
	{
		CPointers::infoDisplay()->setInfo(infoList);
	}
}
