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

#pragma once

#include <QWidget>

#include <QTimer>
#include "../../../backend/drivers/cswordbiblemoduleinfo.h"


class CSwordVerseKey;
class QLineEdit;
class QMenu;

class BtBibleKeyWidget : public QWidget  {
        Q_OBJECT

    public:
        BtBibleKeyWidget(CSwordBibleModuleInfo const * module,
                         CSwordVerseKey * key,
                         QWidget * parent = nullptr);

        ~BtBibleKeyWidget() override;
        bool setKey(CSwordVerseKey* key);
        void setModule(const CSwordBibleModuleInfo *m = nullptr);
        bool eventFilter(QObject *o, QEvent *e) override;

    Q_SIGNALS:
        void changed(CSwordVerseKey* key);

    protected:
        #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        void enterEvent(QEvent * event) override;
        #else
        void enterEvent(QEnterEvent * event) override;
        #endif
        void leaveEvent(QEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void resetDropDownButtons();

    public Q_SLOTS:
        void updateText();

    private: // methods:

        void populateBookMenu(QMenu & menu);
        void populateChapterMenu(QMenu & menu);
        void populateVerseMenu(QMenu & menu);

    private:

        CSwordVerseKey *m_key;

        QLineEdit* m_textbox;

        QWidget *m_dropDownButtons;
        QTimer m_dropDownHoverTimer;

        bool updatelock;
        QString oldKey;
        const CSwordBibleModuleInfo *m_module;
};
