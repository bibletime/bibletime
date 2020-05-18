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

#ifndef BTBIBLEKEYWIDGET_H
#define BTBIBLEKEYWIDGET_H

#include <QWidget>

#include <QTimer>
#include "../../../backend/drivers/cswordbiblemoduleinfo.h"
#include "../cscrollerwidgetset.h"


class BtDropdownChooserButton;
class CLexiconKeyChooser;
class CSwordVerseKey;
class QLineEdit;

class BtBibleKeyWidget : public QWidget  {
        Q_OBJECT

    public:
        BtBibleKeyWidget(const CSwordBibleModuleInfo *module,
                         CSwordVerseKey *key, QWidget *parent = nullptr,
                         const char *name = nullptr);

        ~BtBibleKeyWidget() override;
        bool setKey(CSwordVerseKey* key);
        void setModule(const CSwordBibleModuleInfo *m = nullptr);
        bool eventFilter(QObject *o, QEvent *e) override;

    signals:
        void beforeChange(CSwordVerseKey* key);
        void changed(CSwordVerseKey* key);

    protected:
        void enterEvent(QEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void resetDropDownButtons();

    protected slots: // Protected slots
        /**
        * Is called when the return key was presed in the textbox.
        */
        void slotReturnPressed();

        void slotClearRef();

        void slotUpdateLock();
        void slotUpdateUnlock();

        /**
         * \brief Change the book by the given offset.
         * \param offset The offset to move to.
         */
        void slotStepBook(int offset);

        /**
         * \brief Change the chapter by the given offset.
         * \param offset The offset to move to.
         */
        void slotStepChapter(int offset);

        /**
         * \brief Change the verse by the given offset.
         * \param offset The offset to move to.
         */
        void slotStepVerse(int offset);

        /**
         * \brief Jump to the specified book.
         * \param bookname name of the book to change to
         */
        void slotChangeBook(QString bookname);

        /**
         * \brief Jump to the specified chapter.
         * \param chapter number of the chapter to change to
         */
        void slotChangeChapter(int chapter);

        /**
         * \brief Jump to the specified verse.
         * \param bookname number of the verse to change to
         */
        void slotChangeVerse(int verse);

    public slots:
        void updateText();

    private:
        friend class CLexiconKeyChooser;
        friend class BtDropdownChooserButton;
        friend class BtBookDropdownChooserButton;
        friend class BtChapterDropdownChooserButton;
        friend class BtVerseDropdownChooserButton;

        CSwordVerseKey *m_key;

        QLineEdit* m_textbox;

        CScrollerWidgetSet *m_bookScroller;
        CScrollerWidgetSet *m_chapterScroller;
        CScrollerWidgetSet *m_verseScroller;

        QWidget *m_dropDownButtons;
        QTimer m_dropDownHoverTimer;
        BtDropdownChooserButton* m_bookDropdownButton;
        BtDropdownChooserButton* m_chapterDropdownButton;
        BtDropdownChooserButton* m_verseDropdownButton;

        bool updatelock;
        QString oldKey;
        const CSwordBibleModuleInfo *m_module;
};

#endif
