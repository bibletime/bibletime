/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//Own includes
#include "kstartuplogo.h"

//Qt includes
#include <qapplication.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>

//KDE includes
#include <kapplication.h>
#include <kstandarddirs.h>
//#include <kimageio.h>

//static objects
KStartupLogo* KStartupLogo::startupLogo = 0;

void KStartupLogo::createSplash() {
	deleteSplash();
	startupLogo = new KStartupLogo();
}

void KStartupLogo::showSplash() {
	if (KStartupLogo::startupLogo) {
		KStartupLogo::startupLogo->show();
	}
}

void KStartupLogo::hideSplash() {
	if (startupLogo) {
		startupLogo->hide();
	}
}

void KStartupLogo::deleteSplash() {
	delete startupLogo;
	startupLogo = 0;
}


void KStartupLogo::setStatusMessage(const QString& message) {
	if (startupLogo) {
		startupLogo->setText(message);
	}
}

KStartupLogo::KStartupLogo()
: QWidget(0, "startuplogo", /*WStyle_Customize | WStyle_NoBorder*/ WStyle_NoBorder|WStyle_StaysOnTop|WX11BypassWM ) {

	QPixmap pm;
	if ( !pm.load(locate("BT_pic","startuplogo.png")) ) {
		qWarning("Can't load startuplogo! Check your installation.");
	}

	setBackgroundPixmap(pm);

	//Please not change the margin
	textLabel = new QLabel(this);
	textLabel->setGeometry(0,pm.height(),pm.width(),textLabel->sizeHint().height()+10);
	textLabel->setBackgroundColor( QColor("#0d6de9") );
	textLabel->setFrameStyle(QFrame::Panel | QFrame::Plain);
	textLabel->setLineWidth(1);
	textLabel->setScaledContents( true );

	QFont f;
	f.setBold( true );
	textLabel->setFont( f );

	QPalette p = palette();
	p.setColor( QPalette::Inactive, QColorGroup::Text, Qt::white );
	p.setColor( QPalette::Inactive, QColorGroup::Foreground, Qt::white );
	p.setColor( QPalette::Normal, QColorGroup::Text, Qt::white );
	p.setColor( QPalette::Normal, QColorGroup::Foreground, Qt::white );
	setPalette( p );

	setGeometry (
		(KApplication::desktop()->width()-pm.width())/2,
		(KApplication::desktop()->height()-pm.height()-textLabel->height())/2,
		pm.width(),
		pm.height()+textLabel->height()
	);
}

void KStartupLogo::setText(const QString text) {
	//Please not make the text bold & let the first character be blank
	textLabel->setText( QString::fromLatin1(" %1").arg(text) );
	KApplication::kApplication()->processEvents();
}

/** Makes the splashscreen the toplevel window. */
void KStartupLogo::raiseSplash() {
	if (startupLogo) {
		startupLogo->raise();
		startupLogo->setActiveWindow();
		startupLogo->textLabel->show();
	}
}

/** Lowers the splash screen one window down, so it's possible to make it not to hide tip windows or other startup stuff. */
void KStartupLogo::lowerSplash() {
	if (startupLogo) {
		startupLogo->lower();
	}
}
