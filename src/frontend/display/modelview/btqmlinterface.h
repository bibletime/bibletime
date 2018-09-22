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

#ifndef BT_DISPLAY_VIEW_INTERFACE_H
#define BT_DISPLAY_VIEW_INTERFACE_H

#include <memory>
#include "bttextfilter.h"
#include <QFont>
#include <QList>
#include <QObject>
#include <QString>

class CSwordKey;
class CSwordModuleInfo;
class QTimer;
class RoleItemModel;

struct RefIndexes {
    QString r1;
    QString r2;
    int index1;
    int index2;
};

/**
 * /brief This class provides communications between QML and c++.
 *
 * It is instantiated by usage within the QML files. It provides
 * properties and functions written in c++ and usable by QML.
 */

class BtQmlInterface : public QObject {

    Q_OBJECT

    enum Format {
        HTML,
        Text
    };
    Q_PROPERTY(QString      activeLink              READ getActiveLink  NOTIFY activeLinkChanged    WRITE setActiveLink)
    Q_PROPERTY(int          contextMenuIndex        READ getContextMenuIndex NOTIFY contextMenuIndexChanged WRITE setContextMenuIndex)
    Q_PROPERTY(int          currentModelIndex       READ getCurrentModelIndex NOTIFY currentModelIndexChanged)
    Q_PROPERTY(QString      highlightWords          READ getHighlightWords NOTIFY highlightWordsChanged)
    Q_PROPERTY(int          fontSize0               READ getFontSize0   NOTIFY fontChanged)
    Q_PROPERTY(int          fontSize1               READ getFontSize1   NOTIFY fontChanged)
    Q_PROPERTY(int          fontSize2               READ getFontSize2   NOTIFY fontChanged)
    Q_PROPERTY(int          fontSize3               READ getFontSize3   NOTIFY fontChanged)
    Q_PROPERTY(QString      fontName0               READ getFontName0   NOTIFY fontChanged)
    Q_PROPERTY(QString      fontName1               READ getFontName1   NOTIFY fontChanged)
    Q_PROPERTY(QString      fontName2               READ getFontName2   NOTIFY fontChanged)
    Q_PROPERTY(QString      fontName3               READ getFontName3   NOTIFY fontChanged)
    Q_PROPERTY(QString      highlightWords          READ getHighlightWords NOTIFY highlightWordsChanged)
    Q_PROPERTY(int          numModules              READ getNumModules NOTIFY numModulesChanged)
    Q_PROPERTY(bool         pageDown                READ getPageDown   NOTIFY pageDownChanged)
    Q_PROPERTY(bool         pageUp                  READ getPageUp     NOTIFY pageUpChanged)
    Q_PROPERTY(double       pixelsPerMM             READ getPixelsPerMM NOTIFY pixelsPerMMChanged)
    Q_PROPERTY(QVariant     textModel               READ getTextModel NOTIFY textModelChanged)

public:
    Q_INVOKABLE void changeReference(int i);
    Q_INVOKABLE void dragHandler(int index,const QString& link);
    Q_INVOKABLE QString getRawText(int row, int column);
    Q_INVOKABLE QStringList getModuleNames() const;
    Q_INVOKABLE bool moduleIsWritable(int column);
    Q_INVOKABLE void openContextMenu(int x, int y,int width);
    Q_INVOKABLE void referenceChosen();
    Q_INVOKABLE void setMagReferenceByUrl(const QString& url);
    Q_INVOKABLE void setRawText(int row, int column, const QString& text);

    BtQmlInterface(QObject *parent = nullptr);
    ~BtQmlInterface();

    QString getActiveLink() const;
    QString getBibleUrlFromLink(const QString& url);
    int getContextMenuIndex() const;
    int getCurrentModelIndex() const;
    QString getFontName0() const;
    QString getFontName1() const;
    QString getFontName2() const;
    QString getFontName3() const;
    int getFontSize0() const;
    int getFontSize1() const;
    int getFontSize2() const;
    int getFontSize3() const;
    QString getHighlightWords() const;
    CSwordKey* getKey() const;
    QString getLemmaFromLink(const QString& url);
    int getNumModules() const;
    bool getPageDown() const;
    bool getPageUp() const;
    double getPixelsPerMM() const;
    QVariant getTextModel();
    void pageDown();
    void pageUp();
    void referenceChoosen();
    void scrollToSwordKey(CSwordKey * key);
    void setActiveLink(const QString& link);
    void setContextMenuIndex(int index);
    void setFilterOptions(FilterOptions filterOptions);
    void setHighlightWords(const QString& words, bool caseSensitivy);
    void setKey(CSwordKey* key);
    void setModules(const QStringList &modules);
    void settingsChanged();

signals:
    void activeLinkChanged();
    void contextMenuIndexChanged();
    void contextMenu(int x, int y, int moduleNum);
    void currentModelIndexChanged();
    void fontChanged();
    void highlightWordsChanged();
    void numModulesChanged();
    void pageDownChanged();
    void pageUpChanged();
    void pixelsPerMMChanged();
    void referenceChange();
    void textChanged();
    void textModelChanged();
    void updateReference(const QString& reference);
    void dragOccuring(const QString& moduleName, const QString& keyName);

private slots:
    void timeoutEvent();

private:
    void configModuleByType(const QString& type, const QStringList& availableModuleNames);
    void displayText(const QString& text, const QString& lang);
    QString decodeLemma(const QString& value);
    QString decodeMorph(const QString& value);
    QString fontName(int column) const;
    int fontSize(int column) const;
    void getFontsFromSettings();
    QString getReferenceFromUrl(const QString& url);
    const CSwordModuleInfo* module() const;
    RefIndexes normalizeReferences(const QString& ref1, const QString& ref2);
    QString stripHtml(const QString& html);

    bool m_firstHref;
    QTimer* m_linkTimer;
    BtModuleTextModel* m_moduleTextModel;
    CSwordKey* m_swordKey;

    QList<QFont> m_fonts;
    QString m_highlightWords;
    QStringList m_moduleNames;
    BtTextFilter m_textFilter;
    QString m_timeoutUrl;
    int m_contextMenuIndex;
    QString m_activeLink;
};

#endif
