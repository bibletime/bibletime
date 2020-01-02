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

#ifndef CLEXICONKEYCHOOSER_H
#define CLEXICONKEYCHOOSER_H

#include "ckeychooser.h"


class CKeyChooserWidget;
class CSwordLDKey;
class CSwordLexiconModuleInfo;
class CSwordModuleInfo;
class QHBoxLayout;
class QWidget;

/**
 * This class implements the KeyChooser for lexicons
 *
 * it inhertits @ref CKeyChooser
 * it uses 1 @ref CKeyChooserWidget to represent the lexicon keys
 *
  * @author The BibleTime team
  */
class CLexiconKeyChooser : public CKeyChooser {
        Q_OBJECT

    public:
        CLexiconKeyChooser(const BtConstModuleList &modules,
                           BTHistory *history, CSwordKey *key = nullptr,
                           QWidget *parent = nullptr);

    public slots:

        CSwordKey *key() override;

        void setKey(CSwordKey* key) override;

        /**
        * used to react to changes in the @ref CKeyChooserWidget
        *
        * @param index not used
        **/
        void activated(int index);

        void refreshContent() override;

        void setModules(const BtConstModuleList &modules,
                        bool refresh = true) override;

    protected:
        CKeyChooserWidget *m_widget;
        CSwordLDKey* m_key;
        QList<const CSwordLexiconModuleInfo*> m_modules;
        QHBoxLayout *m_layout;

        inline void adjustFont() override {}

    public slots: // Public slots
        void updateKey(CSwordKey* key) override;

    protected slots:
        void setKey(const QString & newKey) override;

};

#endif
