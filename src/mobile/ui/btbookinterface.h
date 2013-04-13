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
//    Q_PROPERTY(QString book READ getBook WRITE setBook)
//    Q_PROPERTY(QString chapter READ getChapter WRITE setChapter)
//    Q_PROPERTY(QString verse READ getVerse WRITE setVerse)
    Q_PROPERTY(QString displayed READ getDisplayed WRITE setDisplayed NOTIFY displayedChanged)
    Q_PROPERTY(QString text READ getText NOTIFY textChanged)

public:
    BtBookInterface(QObject *parent = 0);

    QString getText() const;

//    QString getBook() const;
//    void setBook(const QString& book);

//    QString getChapter() const;
//    void setChapter(const QString& chapter);

//    QString getVerse() const;
//    void setVerse(const QString& chapter);

    QString getModuleName() const;
    void setModuleName(const QString& moduleName);

    QString getDisplayed() const;

    void setDisplayed(const QString& text);

    Q_INVOKABLE void changeModule();
    Q_INVOKABLE void changeReference();

//    QStringList getBooks() const;
//    QStringList getChapters() const;
//    QStringList getVerses() const;

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
