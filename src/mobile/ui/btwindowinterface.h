#ifndef BT_WINDOW_INTERFACE_H
#define BT_WINDOW_INTERFACE_H

#include <QObject>
#include <QString>

class CSwordKey;
class CSwordVerseKey;
class CSwordBibleModuleInfo;

namespace btm {

class BtWindowInterface : public QObject {

    Q_OBJECT

    Q_PROPERTY(QString moduleName READ getModuleName WRITE setModuleName NOTIFY moduleChanged)
    Q_PROPERTY(QString displayed READ getDisplayed WRITE setDisplayed NOTIFY displayedChanged)
    Q_PROPERTY(QString text READ getText NOTIFY textChanged)

public:
    Q_INVOKABLE void changeModule();
    Q_INVOKABLE void changeReference();

    BtWindowInterface(QObject *parent = 0);

    QString getText() const;

    QString getModuleName() const;
    void setModuleName(const QString& moduleName);

    QString getDisplayed() const;

    void setDisplayed(const QString& text);

signals:
    void displayedChanged();
    void moduleChanged();
    void textChanged();

private slots:
    void referenceChanged();
private:
    const CSwordBibleModuleInfo* module() const;

    CSwordKey* m_key;
};

} // end namespace

#endif
