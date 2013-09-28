#ifndef BT_WINDOW_INTERFACE_H
#define BT_WINDOW_INTERFACE_H

#include <QObject>
#include <QString>
#include "mobile/ui/roleitemmodel.h"
#include "mobile/models/bibletextmodel.h"

class CSwordKey;
class CSwordVerseKey;
class CSwordModuleInfo;

namespace btm {

class BtWindowInterface : public QObject {

    Q_OBJECT

    Q_PROPERTY(QString moduleName READ getModuleName WRITE setModuleName NOTIFY moduleChanged)
    Q_PROPERTY(QString displayed READ getDisplayed WRITE setDisplayed NOTIFY displayedChanged)
    Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize NOTIFY textChanged)
    Q_PROPERTY(QVariant textModel READ getTextModel NOTIFY textModelChanged)
    Q_PROPERTY(int currentModelIndex READ getCurrentModelIndex NOTIFY currentModelIndexChanged)

public:
    Q_INVOKABLE void changeModule();
    Q_INVOKABLE void changeReference();

    BtWindowInterface(QObject *parent = 0);

    CSwordKey* getKey() const;

    QString getModuleName() const;
    void setModuleName(const QString& moduleName);

    QString getDisplayed() const;
    void setDisplayed(const QString& text);

    int getCurrentModelIndex() const;

    int getFontSize() const;
    void setFontSize(int size);

    QVariant getTextModel();

signals:
    void displayedChanged();
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
    BibleTextModelBuilder m_bibleTextModelBuilder;
};

} // end namespace

#endif
