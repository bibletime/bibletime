/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTFINDIDGET_H
#define BTFINDIDGET_H

#include <QWidget>
#include <QWebPage>
class QCheckBox;
class QLineEdit;
class QHBoxLayout;
class QString;

class BtFindWidget : public QWidget {
        Q_OBJECT

    public:
        BtFindWidget(QWidget* parent = 0);
        ~BtFindWidget();
        void showAndSelect();

    private slots:
        void findNext();
        void findPrevious();
        void returnPressed();
        void textChanged(const QString& text);

    private:
        void createCaseCheckBox();
        void createLayout();
        void createSpacer();
        void createTextEditor();
        void createToolButton(const QString& iconName, const QString& text, const char* slot);
        void highlightText(const QString& searchText);

        QHBoxLayout* m_layout;
        QLineEdit* m_textEditor;
        QCheckBox* m_caseCheckBox;

    signals:
        void findPrevious(const QString & text, bool caseSensitive);
        void findNext(const QString & text, bool caseSensitive);
        void highlightText(const QString & text, bool caseSensitive);
};

#endif


