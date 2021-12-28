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

#include <QDialog>

#include <QObject>
#include <QString>
#include <Qt>


class QLineEdit;
class QWidget;

/**
 * @brief The BtMessageInputDialog class provides a editable
 * field for user input. Optionally it displays a larger
 * message.
 */

class BtMessageInputDialog : public QDialog
{
    Q_OBJECT

public: // types:

    enum InputType { Normal, Password };

public: // methods:

    /**
     * @brief The BtMessageInputDialog class provides a editable
     * field for user input. Optionally it displays a larger message.
     * @param title  dialog title
     * @param inputLabel  short text displayed above input field
     * @param inputType The type of user input field to display
     * @param inputText  user input field which may be preloaded with text
     * @param infoMessage optional larger message displayed above the inputLabel
     * @param parent parent window for dialog
     * @param f  optional window flags
     */
    BtMessageInputDialog(QString const & title,
                         QString const & inputLabel,
                         InputType inputType,
                         QString const & inputText,
                         QString const & infoMessage = QString(),
                         QWidget * parent = nullptr,
                         Qt::WindowFlags f = Qt::Dialog);

    /**
     * @brief getUserInput
     * @returns edited text from user
     */
    QString getUserInput() const;

private:
    QLineEdit * m_inputTextEdit;
};
