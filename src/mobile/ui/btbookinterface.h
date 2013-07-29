#ifndef BT_BOOK_INTERFACE_H
#define BT_BOOK_INTERFACE_H

#include <QObject>
#include <QString>

class CSwordTreeKey;
class CSwordBookModuleInfo;

namespace btm {

class BtBookInterface : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString moduleName READ getModuleName WRITE setModuleName NOTIFY moduleChanged)
    Q_PROPERTY(QString displayed READ getDisplayed WRITE setDisplayed NOTIFY displayedChanged)
    Q_PROPERTY(QString text READ getText NOTIFY textChanged)

public:
    Q_INVOKABLE void changeModule();
    Q_INVOKABLE void chooseKey();

    BtBookInterface(QObject *parent = 0);

    CSwordTreeKey* getKey() const;

    QString getText() const;

    QString getModuleName() const;
    void setModuleName(const QString& moduleName);

    QString getDisplayed() const;

    void setDisplayed(const QString& text);

signals:
    void displayedChanged();
    void moduleChanged();
    void textChanged();

private:
    const CSwordBookModuleInfo* module() const;


    CSwordTreeKey* m_key;
};

} // end namespace

#endif
