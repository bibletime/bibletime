/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BT_WINDOW_INTERFACE_H
#define BT_WINDOW_INTERFACE_H

#include "backend/managers/cswordbackend.h"
#include "backend/models/btmoduletextmodel.h"
#include "mobile/models/roleitemmodel.h"
#include "mobile/ui/btmmoduletextfilter.h"
#include <QDomNodeList>
#include <QList>
#include <QObject>
#include <QString>

class CSwordKey;
class CSwordVerseKey;
class CSwordModuleInfo;

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

    Q_PROPERTY(QStringList  comboBoxEntries         READ getComboBoxEntries NOTIFY comboBoxEntriesChanged)
    Q_PROPERTY(QString      currentModelText        READ getCurrentModelText NOTIFY currentModelIndexChanged)
    Q_PROPERTY(int          currentModelIndex       READ getCurrentModelIndex NOTIFY currentModelIndexChanged)
    Q_PROPERTY(int          fontSize                READ getFontSize WRITE setFontSize NOTIFY textChanged)
    Q_PROPERTY(QString      fontName                READ getFontName NOTIFY textChanged)
    Q_PROPERTY(QString      footnoteText            READ getFootnoteText NOTIFY footnoteTextChanged)
    Q_PROPERTY(bool         footnoteVisible         READ getFootnoteVisible NOTIFY footnoteVisibleChanged)
    Q_PROPERTY(QString      highlightWords          READ getHighlightWords WRITE setHighlightWords)
    Q_PROPERTY(bool         historyBackwardVisible  READ getHistoryBackwardVisible NOTIFY historyChanged)
    Q_PROPERTY(bool         historyForwardVisible   READ getHistoryForwardVisible NOTIFY historyChanged)
    Q_PROPERTY(bool         isMagView               READ isMagView WRITE setMagView)
    Q_PROPERTY(QString      moduleLanguage          READ getModuleLanguage)
    Q_PROPERTY(QString      moduleName              READ getModuleName WRITE setModuleName NOTIFY moduleChanged)
    Q_PROPERTY(QString      module2Name             READ getModule2Name WRITE setModule2Name NOTIFY module2Changed)
    Q_PROPERTY(QString      module3Name             READ getModule3Name WRITE setModule3Name NOTIFY module3Changed)
    Q_PROPERTY(QString      module4Name             READ getModule4Name WRITE setModule4Name NOTIFY module4Changed)
    Q_PROPERTY(int          numModules              READ getNumModules NOTIFY numModulesChanged)
    Q_PROPERTY(QString      prompt                  READ getPrompt NOTIFY promptChanged)
    Q_PROPERTY(QString      reference               READ getReference WRITE setReference NOTIFY referenceChange)
    Q_PROPERTY(QStringList  references              READ getReferences NOTIFY referencesChanged)
    Q_PROPERTY(QString      referencesViewTitle     READ getReferencesViewTitle NOTIFY referencesViewTitleChanged)
    Q_PROPERTY(QVariant     textModel               READ getTextModel NOTIFY textModelChanged)

public:
    Q_INVOKABLE void changeReference();
    Q_INVOKABLE int  getComboIndexFromUrl(const QString& url);
    Q_INVOKABLE QString getDefaultSwordModuleByType(const QString& type);
    Q_INVOKABLE QStringList getModuleNames() const;
    Q_INVOKABLE bool firstModuleIsBibleOrCommentary();
    Q_INVOKABLE void moveHistoryBackward();
    Q_INVOKABLE void moveHistoryForward();
    Q_INVOKABLE void saveWindowStateToConfig(int windowIndex);
    Q_INVOKABLE void setHistoryPoint();
    Q_INVOKABLE void setModuleToBeginning();
    Q_INVOKABLE void setReferenceByUrl(const QString& url);
    Q_INVOKABLE void updateCurrentModelIndex();
    Q_INVOKABLE void updateDefaultModules();
    Q_INVOKABLE void updateTextFonts();
    Q_INVOKABLE void updateKeyText(int modelIndex);
    Q_INVOKABLE void updateReferences();
    Q_INVOKABLE QString getModelTextByIndex(int index) const;


    BtWindowInterface(QObject *parent = nullptr);
    ~BtWindowInterface();

    QStringList getComboBoxEntries() const;
    QString getCurrentModelText() const;
    int getCurrentModelIndex() const;
    int getFontSize() const;
    QString getFontName() const;
    QString getFootnoteText() const;
    bool getFootnoteVisible() const;
    QString getHighlightWords() const;
    bool getHistoryBackwardVisible() const;
    bool getHistoryForwardVisible() const;
    CSwordKey* getKey() const;
    QString getModuleLanguage() const;
    QString getModuleName() const;
    QString getModule2Name() const;
    QString getModule3Name() const;
    QString getModule4Name() const;
    int getNumModules() const;
    QString getPrompt() const;
    QString getReference() const;
    QStringList getReferences() const;
    QString getReferencesViewTitle() const;
    QVariant getTextModel();
    bool isMagView() const;
    void moduleNameChanged(const QString& moduleName);

    void setFontSize(int size);
    void setHighlightWords(const QString& words);
    void setMagView(bool magView);
    void setModuleName(const QString& moduleName);
    void setModule2Name(const QString& moduleName);
    void setModule3Name(const QString& moduleName);
    void setModule4Name(const QString& moduleName);
    void setReference(const QString& key);


signals:
    void comboBoxEntriesChanged();
    void currentModelIndexChanged();
    void footnoteTextChanged();
    void footnoteVisibleChanged();
    void historyChanged();
    void moduleChanged();
    void module2Changed();
    void module3Changed();
    void module4Changed();
    void numModulesChanged();
    void promptChanged();
    void referenceChange();
    void referencesChanged();
    void referencesViewTitleChanged();
    void textChanged();
    void textModelChanged();

private slots:
    void referenceChanged();
    void referenceChosen();
    void referenceChosen(int index);
    void reloadModules(CSwordBackend::SetupChangedReason reason);

private:
    void configModuleByType(const QString& type, const QStringList& availableModuleNames);
    void decodeFootnote(const QString& keyName, const QString& footnote);
    void decodeLemmaMorph(const QString& keyName);
    void displayText(const QString& text, const QString& lang);
    QString decodeLemma(const QString& value);
    QString decodeMorph(const QString& value);
    QString getReferenceFromUrl(const QString& url);
    VerseChooser* getVerseKeyChooser();
    BookKeyChooser* getBookKeyChooser();
    KeyNameChooser* getKeyNameChooser();
    void lookupAvailableModules();
    const CSwordModuleInfo* module() const;
    void updateModel();
    void parseVerseForReferences(const QString& verse);
    void processNodes(const QDomNodeList& nodes);
    void setFootnoteVisible(bool visible);
    void setPrompt(const QString& title);
    void setReferencesViewTitle(const QString& title);

    BookKeyChooser* m_bookKeyChooser;
    bool m_firstHref;
    bool m_footnoteVisible;
    int m_historyIndex;
    CSwordKey* m_key;
    KeyNameChooser* m_keyNameChooser;
    bool m_magView;
    BtModuleTextModel* m_moduleTextModel;
    RoleItemModel* m_textModel;
    VerseChooser* m_verseKeyChooser;

    QStringList m_bibles;
    QStringList m_comboBoxEntries;
    QString m_footnoteNum;
    QString m_footnoteText;
    QStringList m_greekStrongsLexicons;
    QStringList m_hebrewStrongsLexicons;
    QString m_highlightWords;
    QList<History> m_history;
    QStringList m_moduleNames;
    QString m_prompt;
    QStringList m_references;
    QString m_referencesViewTitle;
    BtmModuleTextFilter m_textFilter;
};

} // end namespace

#endif
