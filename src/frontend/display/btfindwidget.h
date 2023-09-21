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

#include <QObject>
#include <QString>


class QCheckBox;
class QLineEdit;
class QToolButton;

class BtFindWidget final: public QWidget {

    Q_OBJECT

private: /* Types: */

    struct HighlightState {
        QString text;
        bool caseSensitive;

        friend bool operator==(HighlightState const & lhs,
                               HighlightState const & rhs) noexcept
        {
            return (lhs.text == rhs.text)
                   && (lhs.caseSensitive == rhs.caseSensitive);
        }

        friend bool operator!=(HighlightState const & lhs,
                               HighlightState const & rhs) noexcept
        {
            return (lhs.text != rhs.text)
                   || (lhs.caseSensitive != rhs.caseSensitive);
        }
    };

public: // methods:

    BtFindWidget(QWidget * parent = nullptr);

    void showAndSelect();

protected: // Methods:

    void timerEvent(QTimerEvent * const event) final override;

private: // methods:

    void queueHighlight();
    void highlightImmediately();
    void retranslateUi();

Q_SIGNALS:

    void findPrevious();
    void findNext();
    void highlightText(QString const & text, bool caseSensitive);

private: // fields:

    QLineEdit * m_textEditor;
    QToolButton * m_nextButton;
    QToolButton * m_previousButton;
    QCheckBox * m_caseCheckBox;
    HighlightState m_lastHighlightState{{}, false};
    int m_throttleTimerId = 0;

};
