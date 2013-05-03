#ifndef VERSE_CHOOSER_H
#define VERSE_CHOOSER_H

#include <QObject>
#include <QList>

class QtQuick2ApplicationViewer;
class QQmlComponent;
class QQuickItem;
class QStringList;

namespace btm {

class GridChooser;

class BtWindowInterface;

class VerseChooser : public QObject {
    Q_OBJECT

    enum State {
        CLOSED,
        BOOK,
        CHAPTER,
        VERSE
    };

public:
    VerseChooser(QtQuick2ApplicationViewer* viewer, BtWindowInterface* bibleVerse);
    void open();

private slots:
    void stringAccepted(const QString& value);
    void stringCanceled();

private:
    void showGridChooser(const QStringList& list);
    void setProperties(const QStringList& list);


    QtQuick2ApplicationViewer* viewer_;
    GridChooser* gridChooser_;
    BtWindowInterface* bibleVerse_;
    int state_;
    QString oldBook_;
    QString oldChapter_;
    QString oldVerse_;
    QString newBook_;
    QString newChapter_;
    QString newVerse_;
};

} // end namespace

#endif
