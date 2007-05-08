/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CKEYREFERENCEWIDGET_H
#define CKEYREFERENCEWIDGET_H

#include "cscrollerwidgetset.h"
#include "backend/cswordbiblemoduleinfo.h"

#include <qwidget.h>
#include <qlayout.h>

#include <klineedit.h>
#include <kcompletion.h>

class CLexiconKeyChooser;
class CSwordVerseKey;

class CKeyReferenceCompletion : public KCompletion {
    Q_OBJECT
public:
    CKeyReferenceCompletion(CSwordBibleModuleInfo*);
    QString makeCompletion(const QString &);
    QStringList allMatches();
private:
    CSwordVerseKey *m_key;
    CSwordBibleModuleInfo *m_module;
};

class CKeyReferenceLineEdit : public KLineEdit {
    Q_OBJECT
public:
    CKeyReferenceLineEdit(QWidget *parent=0, const char *name=0);
    void makeCompletion(const QString &);
};

class CKeyReferenceWidget : public QWidget  {
    Q_OBJECT
public:
    /**
    * the constructor
    */
    CKeyReferenceWidget(CSwordBibleModuleInfo *, CSwordVerseKey*, QWidget *parent=0, const char *name=0);
    bool setKey(CSwordVerseKey* key);
    KLineEdit* textbox();
    void setModule(CSwordBibleModuleInfo *m = 0);

signals:
    void changed(CSwordVerseKey* key);

protected:
    void updateText();

protected slots: // Protected slots
    /**
    * Is called when the return key was presed in the textbox.
    */
    void slotReturnPressed();

    void slotClearRef();

    void slotUpdateLock();
    void slotUpdateUnlock();
    void slotBookChange(int);
    void slotChapterChange(int);
    void slotVerseChange(int);

private:
    friend class CLexiconKeyChooser;
    QHBoxLayout *m_mainLayout;
    CKeyReferenceLineEdit *m_textbox;
    CSwordVerseKey *m_key;
    CScrollerWidgetSet *m_bookScroller;
    CScrollerWidgetSet *m_chapterScroller;
    CScrollerWidgetSet *m_verseScroller;
    bool updatelock;
    QString oldKey;
    CSwordBibleModuleInfo *m_module;
};

#endif
