#ifndef BT_WINDOW_INTERFACE_H
#define BT_WINDOW_INTERFACE_H

#include <QObject>
#include <QString>
#include "mobile/models/roleitemmodel.h"
#include "mobile/models/bibletextmodelbuilder.h"
#include "mobile/models/booktextmodelbuilder.h"

class CSwordKey;
class CSwordVerseKey;
class CSwordModuleInfo;

namespace btm {

class BookKeyChooser;
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

private:
    const CSwordModuleInfo* module() const;
    void updateModel();

    CSwordKey* m_key;
    RoleItemModel* m_textModel;
    BookKeyChooser* m_bookKeyChooser;
    VerseChooser* m_verseKeyChooser;
    BibleTextModelBuilder m_bibleTextModelBuilder;
    BookTextModelBuilder m_bookTextModelBuilder;
};

} // end namespace

#endif
