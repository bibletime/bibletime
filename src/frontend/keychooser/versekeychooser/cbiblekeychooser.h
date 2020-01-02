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

#ifndef CBIBLEKEYCHOOSER_H
#define CBIBLEKEYCHOOSER_H

#include "../ckeychooser.h"

#include <QList>
#include "../../../backend/drivers/cswordbiblemoduleinfo.h"


class QWidget;

class BtBibleKeyWidget;
class CSwordVerseKey;
class CSwordBibleModuleInfo;

/** This class implements the KeyChooser for bibles and commentaries
 *
 * it inhertits @ref CKeyChooser
 *
 * it uses a BtBibleKeyWidget to represent the bible keys
 *
  * @author The BibleTime team
  */

class CBibleKeyChooser : public CKeyChooser  {
        Q_OBJECT

    public:
        CBibleKeyChooser(const BtConstModuleList &modules,
                         BTHistory *history, CSwordKey *key = nullptr,
                         QWidget *parent = nullptr);

    public slots:

        CSwordKey* key() override;

        void setKey(CSwordKey *key) override;

        void setModules(const BtConstModuleList &modules,
                        bool refresh = true) override;

        /**
        * used to do actions before key changes
        */
        void beforeRefChange(CSwordVerseKey *key);
        /**
        * used to do actions after key changes
        */
        void refChanged(CSwordVerseKey *key);

        void updateKey(CSwordKey* key) override;
        void adjustFont() override;
        void refreshContent() override;

    protected slots:

        void setKey(const QString & newKey) override;

    private:
        BtBibleKeyWidget* w_ref;
        QList<const CSwordBibleModuleInfo*> m_modules;
        CSwordVerseKey *m_key;
};

#endif
