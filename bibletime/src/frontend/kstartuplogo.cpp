/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//Own includes
#include "kstartuplogo.h"
#include "util/directoryutil.h"

//Qt includes
#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QDesktopWidget>

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
: QWidget(0, /*WStyle_Customize | WStyle_NoBorder*/
			Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint )
{

	QPixmap pm;
	if ( !pm.load( util::filesystem::DirectoryUtil::getPicsDir().canonicalPath().append("/startuplogo.png")) ) {
		qWarning("Can't load startuplogo! Check your installation.");
	}

	//setBackgroundPixmap(pm); //see qt3 support members of qwidget
	QPalette bgPalette;
 	bgPalette.setBrush(this->backgroundRole(), QBrush(pm));
 	this->setPalette(bgPalette);

	//Please not change the margin
	textLabel = new QLabel(this);
	textLabel->setGeometry(0,pm.height(),pm.width(),textLabel->sizeHint().height()+10);
	//textLabel->setBackgroundColor( QColor("#0d6de9") );
	QPalette labelPalette;
	labelPalette.setColor(textLabel->backgroundRole(), QColor("#0d6de9"));
	textLabel->setPalette(labelPalette);


	textLabel->setFrameStyle(QFrame::Panel | QFrame::Plain);
	textLabel->setLineWidth(1);
	textLabel->setScaledContents( true );

	QFont f;
	f.setBold( true );
	textLabel->setFont( f );

	QPalette p = palette();
	p.setColor( QPalette::Inactive, QPalette::Text, Qt::white );
	p.setColor( QPalette::Inactive, QPalette::Foreground, Qt::white );
	p.setColor( QPalette::Normal, QPalette::Text, Qt::white );
	p.setColor( QPalette::Normal, QPalette::Foreground, Qt::white );
	setPalette( p );

	QWidget* primaryScreen = QApplication::desktop()->screen( QApplication::desktop()->primaryScreen() );
	setGeometry (
		(primaryScreen->width()-pm.width())/2,
		(primaryScreen->height()-pm.height()-textLabel->height())/2,
		pm.width(),
		pm.height()+textLabel->height()
	);
}

void KStartupLogo::setText(const QString text) {
	//Please not make the text bold & let the first character be blank
	textLabel->setText( QString::fromLatin1(" %1").arg(text) );
	qApp->processEvents();
}

///** Makes the splashscreen the toplevel window. */
//void KStartupLogo::raiseSplash() {
//	if (startupLogo) {
//		startupLogo->raise();
//		startupLogo->activateWindow();
//		startupLogo->textLabel->show();
//	}
//}
//
///** Lowers the splash screen one window down, so it's possible to make it not to hide tip windows or other startup stuff. */
//void KStartupLogo::lowerSplash() {
//	if (startupLogo) {
//		startupLogo->lower();
//	}
//}
