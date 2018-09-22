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

#ifndef BTHTMLJSOBJECT_H
#define BTHTMLJSOBJECT_H

#include <QObject>

#include <QPoint>


class BtHtmlReadDisplay;

class BtHtmlJsObject: public QObject {
        Q_OBJECT

    public:
        BtHtmlJsObject(BtHtmlReadDisplay* display);

        void moveToAnchor(const QString& anchor);
        void clearPrevAttribute();

    public slots:
        void mouseMoveEvent(const QString& attributes, const int& x, const int& y, const bool& shiftKey);
        void mouseClick(const QString& url);
        void mouseDownLeft(const QString& url, const int& X, const int& Y);
        void mouseDownRight(const QString& url, const QString& lemma);
        void timeOutEvent(const QString& attributes);

    signals:
        void startTimer(int time);
        void mouseMoveAttribute(const QString& attrName, const QString& attrValue);
        void gotoAnchor(const QString& anchor);
        void selectAll();

    private:
        int m_int;
        BtHtmlReadDisplay* m_display;
        QString m_prev_attributes;

        struct DNDData {
            bool mousePressed;
            bool isDragging;
            QPoint startPos;
            QString url;
            enum DragType {
                Link,
                Text
            } dragType;
        }        m_dndData;

};

#endif
