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

#include <QList>
#include <QObject>
#include <QString>
#include "backend/managers/cswordbackend.h"
#include "mobile/models/roleitemmodel.h"

class CSwordKey;
class CSwordVerseKey;
class CSwordModuleInfo;
class BtModuleTextModel;

namespace btm {

class BookKeyChooser;
class KeyNameChooser;
class VerseChooser;

struct History
{
    QString moduleName;
    QString reference;
};

class BtWindowInterface : public QObject {

    Q_OBJECT

    Q_PROPERTY(int      currentModelIndex   READ getCurrentModelIndex NOTIFY currentModelIndexChanged)
    Q_PROPERTY(int      fontSize   READ getFontSize WRITE setFontSize NOTIFY textChanged)
    Q_PROPERTY(QString  highlightWords   READ getHighlightWords WRITE setHighlightWords)
    Q_PROPERTY(QString  moduleName   READ getModuleName WRITE setModuleName NOTIFY moduleChanged)
    Q_PROPERTY(QString  reference   READ getReference WRITE setReference NOTIFY referenceChange)
    Q_PROPERTY(QVariant textModel   READ getTextModel NOTIFY textModelChanged)
    Q_PROPERTY(bool     historyBackwardVisible READ getHistoryBackwardVisible NOTIFY historyChanged)
    Q_PROPERTY(bool     historyForwardVisible READ getHistoryForwardVisible NOTIFY historyChanged)

public:
    Q_INVOKABLE void changeModule();
    Q_INVOKABLE void changeReference();
    Q_INVOKABLE void saveWindowStateToConfig(int windowIndex);
    Q_INVOKABLE void updateCurrentModelIndex();
    Q_INVOKABLE void updateKeyText(int modelIndex);
    Q_INVOKABLE void moveHistoryBackward();
    Q_INVOKABLE void moveHistoryForward();
    Q_INVOKABLE void setHistoryPoint();
    Q_INVOKABLE void setModuleToBeginning();

    BtWindowInterface(QObject *parent = 0);

    int getCurrentModelIndex() const;
    int getFontSize() const;
    QString getHighlightWords() const;
    CSwordKey* getKey() const;
    QString getModuleName() const;
    QString getReference() const;
    QVariant getTextModel();

    void moduleNameChanged(const QString& moduleName);
    void setFontSize(int size);
    void setHighlightWords(const QString& words);
    void setModuleName(const QString& moduleName);
    void setReference(const QString& key);

    bool getHistoryBackwardVisible() const;
    bool getHistoryForwardVisible() const;

signals:
    void currentModelIndexChanged();
    void historyChanged();
    void moduleChanged();
    void referenceChange();
    void textChanged();
    void textModelChanged();

private slots:
    void referenceChanged();
    void referenceChosen();
    void referenceChosen(int index);
    void reloadModules(CSwordBackend::SetupChangedReason reason);

private:
    const CSwordModuleInfo* module() const;
    void updateModel();

    CSwordKey* m_key;
    RoleItemModel* m_textModel;
    BtModuleTextModel* m_moduleTextModel;
    BookKeyChooser* m_bookKeyChooser;
    KeyNameChooser* m_keyNameChooser;
    VerseChooser* m_verseKeyChooser;
    QString m_highlightWords;
    QString m_moduleName;
    QList<History> m_history;
    int m_historyIndex;

};

} // end namespace

#endif
