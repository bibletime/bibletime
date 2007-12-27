//
// C++ Interface: cacceleratorsettings
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CACCELERATORSETTINGS_H
#define CACCELERATORSETTINGS_H

#include <QWidget>
#include <QPointer>

#include <kshortcutseditor.h>

class QComboBox;
class QStackedWidget;
class KActionCollection;

/**
	@author The BibleTime team <info@bibletime.info>
*/
class CAcceleratorSettingsPage : public QWidget
{
	Q_OBJECT

public:
    CAcceleratorSettingsPage(QWidget *parent);

    ~CAcceleratorSettingsPage();

	void save();

protected slots:

	void slotKeyChooserTypeChanged(const QString& title);

private:

	struct WindowType {
		QPointer<KShortcutsEditor> keyChooser;
		KActionCollection* actionCollection;
		QString title;

		WindowType() {
			keyChooser = 0;
			actionCollection = 0;
		};
		WindowType(const QString& newTitle) {
			title = newTitle;
			keyChooser = 0;
			actionCollection = 0;
		}
	};

	WindowType m_application;
	WindowType m_general;
	WindowType m_bible;
	WindowType m_commentary;
	WindowType m_lexicon;
	WindowType m_book;

	QComboBox* m_typeChooser;
	QStackedWidget* m_keyChooserStack;

};

#endif
