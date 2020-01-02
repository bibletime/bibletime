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

#ifndef BTFINDIDGET_H
#define BTFINDIDGET_H

#include <QWidget>


class QCheckBox;
class QHBoxLayout;
class QLineEdit;
class QString;
class QToolButton;

class BtFindWidget: public QWidget {

    Q_OBJECT

public: /* Methods: */

    BtFindWidget(QWidget * parent = nullptr);

    void showAndSelect();

private slots:

    void findNext() { emit findNext(text(), caseSensitive()); }
    void findPrevious() { emit findPrevious(text(), caseSensitive()); }
    void returnPressed() { emitChange(text(), caseSensitive()); }
    void textChanged(QString const & txt) { emitChange(txt, caseSensitive()); }
    void caseStateChanged(int st) { emitChange(text(), st == Qt::Checked); }

private: /* Methods: */

    void retranslateUi();

    void highlightText(QString const & text)
    { emit highlightText(text, caseSensitive()); }

    bool caseSensitive() const;

    QString text() const;

    void emitChange(QString const & text, bool const caseSensitive) {
        emit highlightText(text, caseSensitive);
    }

signals:

    void findPrevious(QString const & text, bool caseSensitive);
    void findNext(QString const & text, bool caseSensitive);
    void highlightText(QString const & text, bool caseSensitive);

private: /* Fields: */

    QHBoxLayout * m_layout;
    QLineEdit * m_textEditor;
    QToolButton * m_nextButton;
    QToolButton * m_previousButton;
    QCheckBox * m_caseCheckBox;

};

#endif
