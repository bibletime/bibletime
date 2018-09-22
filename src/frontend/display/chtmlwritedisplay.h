/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CHTMLWRITEDISPLAY_H
#define CHTMLWRITEDISPLAY_H

#include "frontend/display/cplainwritedisplay.h"


class BtActionCollection;
class BtColorWidget;
class BtFontSizeWidget;
class CHTMLWriteWindow;
class QAction;
class QFontComboBox;
class QMenu;
class QToolBar;
class QWidget;

/** The WYSIWYG implementation of the write display interface.
  * @author The BibleTime team
  */
class CHTMLWriteDisplay : public CPlainWriteDisplay {
        Q_OBJECT
    public:

        CHTMLWriteDisplay(CHTMLWriteWindow * parentWindow, QWidget * parent = nullptr);

        void setText( const QString& newText ) override;

        const QString plainText() override;

        void setupToolbar(QToolBar * bar, BtActionCollection * actionCollection) override;

    protected:

        void alignmentChanged(int);

    protected slots:
        void toggleBold(bool checked);
        void toggleItalic(bool checked);
        void toggleUnderline(bool checked);

        void alignLeft(bool);
        void alignCenter(bool);
        void alignRight(bool);

        void slotFontFamilyChosen(const QFont&);
        void slotFontSizeChosen(int);

        void slotCurrentCharFormatChanged(const QTextCharFormat &);

    signals:

        void signalFontChanged(const QFont &);
        void signalFontSizeChanged(int);
        void signalFontColorChanged(const QColor &);

    private:
        struct {
            QAction* bold;
            QAction* italic;
            QAction* underline;

            QAction* alignLeft;
            QAction* alignCenter;
            QAction* alignRight;
        }
        m_actions;

        bool m_handingFormatChangeFromEditor;
};

#endif
