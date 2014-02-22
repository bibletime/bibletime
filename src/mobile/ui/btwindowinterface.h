/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BT_WINDOW_INTERFACE_H
#define BT_WINDOW_INTERFACE_H

#include <QObject>
#include <QString>
#include "mobile/models/roleitemmodel.h"

class CSwordKey;
class CSwordVerseKey;
class CSwordModuleInfo;
class BtModuleTextModel;

namespace btm {

class BookKeyChooser;
class KeyNameChooser;
class VerseChooser;

class BtWindowInterface : public QObject {

    Q_OBJECT

    Q_PROPERTY(QString moduleName READ getModuleName WRITE setModuleName NOTIFY moduleChanged)
    Q_PROPERTY(QString reference READ getReference WRITE setReference NOTIFY referenceChange)
    Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize NOTIFY textChanged)
    Q_PROPERTY(QVariant textModel READ getTextModel NOTIFY textModelChanged)
    Q_PROPERTY(int currentModelIndex READ getCurrentModelIndex NOTIFY currentModelIndexChanged)

public:
    Q_INVOKABLE void changeModule();
    Q_INVOKABLE void changeReference();
    Q_INVOKABLE void saveWindowStateToConfig(int windowIndex);
    Q_INVOKABLE void updateKeyText(int modelIndex);
    Q_INVOKABLE void updateCurrentModelIndex();

    BtWindowInterface(QObject *parent = 0);

    CSwordKey* getKey() const;

    QString getModuleName() const;
    void setModuleName(const QString& moduleName);
    void setReference(const QString& key);

    QString getReference() const;

    int getCurrentModelIndex() const;

    int getFontSize() const;
    void setFontSize(int size);

    QVariant getTextModel();

signals:
    void referenceChange();
    void moduleChanged();
    void textChanged();
    void textModelChanged();
    void currentModelIndexChanged();

private slots:
    void referenceChanged();
    void referenceChosen();
    void referenceChosen(int index);

private:
    const CSwordModuleInfo* module() const;
    void updateModel();

    CSwordKey* m_key;
    RoleItemModel* m_textModel;
    BtModuleTextModel* m_moduleTextModel;
    BookKeyChooser* m_bookKeyChooser;
    KeyNameChooser* m_keyNameChooser;
    VerseChooser* m_verseKeyChooser;
};

} // end namespace

#endif
