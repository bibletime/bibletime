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

#include <QWizard>

#include <QObject>
#include <QString>
#include <Qt>


class BtPlainOrHtmlPage;
class BtEditTextPage;
class QFont;
class QWidget;

/** \brief The Edit Text wizard for editing the personal commentary. */
class BtEditTextWizard final: public QWizard {

    Q_OBJECT

public: // methods:

    BtEditTextWizard(QWidget * parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    bool htmlMode() const;
    void setFont(const QFont& font);
    void setText(const QString& text);
    void setTitle(const QString& text);
    QString text() const;

public Q_SLOTS:

    void accept() final override;

private:

    void retranslateUi();

    BtPlainOrHtmlPage * const m_plainOrHtmlPage;
    BtEditTextPage * const m_editTextPage;

}; /* class BtEditTextWizard */
