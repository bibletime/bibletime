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

#ifndef BT_SEARCH_INTERFACE_H
#define BT_SEARCH_INTERFACE_H

#include <QObject>
#include <QString>
#include "backend/cswordmodulesearch.h"
#include "mobile/models/roleitemmodel.h"
#include <listkey.h>

class CSwordKey;
class CSwordVerseKey;
class CSwordModuleInfo;
class BtModuleTextModel;
class QQuickItem;
class IndexThread;

namespace btm {

class BtSearchInterface : public QObject {

    Q_OBJECT

    enum findType {
        AndType,
        OrType,
        FullType
    };

    Q_PROPERTY(QString searchText READ getSearchText WRITE setSearchText)
    Q_PROPERTY(QString findChoice READ getFindChoice WRITE setFindChoice)
    Q_PROPERTY(QString moduleList READ getModuleList WRITE setModuleList)
    Q_PROPERTY(QVariant modulesModel READ getModulesModel NOTIFY modulesModelChanged)
    Q_PROPERTY(QVariant referencesModel READ getReferencesModel NOTIFY referencesModelChanged)
    Q_PROPERTY(int AndType READ getAndType)
    Q_PROPERTY(int OrType READ getAndType)
    Q_PROPERTY(int FullType READ getAndType)
    Q_PROPERTY(int searchType READ getSearchType WRITE setSearchType)
    Q_PROPERTY(bool indexingFinished READ getIndexingFinished NOTIFY indexingFinished)
    Q_PROPERTY(qreal progressValue  READ progressValue   NOTIFY progressValueChanged)
    Q_PROPERTY(QString progressText READ progressText    NOTIFY progressTextChanged)
    Q_PROPERTY(bool haveReferences READ haveReferences NOTIFY haveReferencesChanged)

public:
    Q_INVOKABLE bool performSearch();
    Q_INVOKABLE bool modulesAreIndexed();
    Q_INVOKABLE bool indexModules();
    Q_INVOKABLE void selectReferences(int moduleIndex);
    Q_INVOKABLE QString getModuleName(int index);
    Q_INVOKABLE QString getReference(int index);
    Q_INVOKABLE void cancel();

    BtSearchInterface(QObject *parent = nullptr);
    ~BtSearchInterface();

    QString getSearchText() const;
    QString getFindChoice() const;
    QString getModuleList() const;

    void setSearchText(const QString& searchText);
    void setFindChoice(const QString& findChoice);
    void setModuleList(const QString& moduleList);

    QVariant getModulesModel();
    QVariant getReferencesModel();

    qreal progressValue() const;
    QString progressText() const;

    int getAndType() { return AndType; }
    int getOrType() { return OrType; }
    int getFullType() { return FullType; }
    int getSearchType() { return m_searchType; }
    bool getIndexingFinished() {
        return true;
    };
    bool haveReferences();
    void setSearchType(int searchType);

signals:
    void haveReferencesChanged();
    void modulesModelChanged();
    void referencesModelChanged();
    void indexingFinished();

    void progressValueChanged();
    void progressTextChanged();

private slots:
    void slotModuleProgress(int value);
    void slotBeginModuleIndexing(const QString& moduleName);
    void slotIndexingFinished();

private:
    QString prepareSearchText(const QString& orig);
    void setupModuleModel(const CSwordModuleSearch::Results& results);
    void setupReferenceModel(const CSwordModuleInfo *m,
                             const swordxx::ListKey & results);
    void setupSearchType();
    bool wasCanceled();

    int m_searchType;
    qreal m_progressValue;
    QString m_progressText;

    IndexThread* m_thread;
    bool m_wasCancelled;
    QString m_searchText;
    QString m_findChoice;
    QString m_moduleList;
    RoleItemModel m_modulesModel;
    RoleItemModel m_referencesModel;
    CSwordModuleSearch::Results m_results;
};

} // end namespace

#endif
