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

#include "btmmoduletextfilter.h"

#include <QDomDocument>
#include <QStringList>

/*
 * This filter provides a method for modifying text generated
 * by BtModuleTextModel. Specifically is can modify footnotes
 * to hyperlinks. This is needed because QML rich text usage
 * does not allow DOM access for hovered items as implemented
 * by BibleTime Desktop. By using hyperlinks the footnotes and
 * cross references can be clicked and corresponding actions
 * will show the necessary information.
 */

namespace btm {

QString BtmModuleTextFilter::processText(const QString &text) {
    m_doc.setContent(text);
    QDomNodeList nodes = m_doc.elementsByTagName("html");
    if (nodes.count() != 1)
        return QString();
    QDomNode htmlNode = nodes.at(0);
    QDomElement bodyElement = htmlNode.firstChildElement("body");
    if (bodyElement.isNull())
        return QString();
    QDomNodeList spanNodes = bodyElement.elementsByTagName("span");
    traverse(spanNodes);
    QString newText = m_doc.toString();
    return newText;
}

void BtmModuleTextFilter::traverse(const QDomNodeList& nodeList) {
    int count = nodeList.count();
    for (int i=0; i<count; i++) {
        QDomNode node = nodeList.at(i);
        QDomNode::NodeType nodeType = node.nodeType();
        if (nodeType == QDomNode::TextNode){
            QDomText text = node.toText();
            QString textValue = text.data();
        }
        QDomElement element = node.toElement();
        if (isFootnote(element)) {
            editFootnoteText(element);
        }
        QDomNodeList childrenNodes = node.childNodes();
        traverse(childrenNodes);
    }
}

bool BtmModuleTextFilter::isFootnote(const QDomElement& element) const {
    QDomNamedNodeMap nodeMap = element.attributes();
    if (nodeMap.contains("class")) {
        QDomNode classNode = nodeMap.namedItem("class");
        QDomAttr classAttr = classNode.toAttr();
        QString classValue = classAttr.value();
        return classValue == "footnote";
    }
    return false;
}

void BtmModuleTextFilter::editFootnoteText(QDomElement& element) {

    QDomNamedNodeMap nodeMap = element.attributes();
    QDomNode refNode = nodeMap.namedItem("note");
    QDomAttr noteAttr = refNode.toAttr();
    QString noteValue = noteAttr.value();

    QDomNodeList childrenNodes = element.childNodes();
    int count = childrenNodes.count();
    for (int i=0; i<count; i++) {
        QDomNode node = childrenNodes.at(i);
        QDomNode::NodeType nodeType = node.nodeType();
        if (nodeType == QDomNode::TextNode){
            QDomText text = node.toText();
            QString textValue = text.data();
            QString newText = QString("(" + textValue + ")");
            element.removeChild(node);

            QDomElement aNode = m_doc.createElement("a");
            element.appendChild(aNode);

            QString url = "sword://Bible/" + noteValue;
            aNode.setAttribute("href", url);

            QDomText textNode = m_doc.createTextNode(newText);
            aNode.appendChild(textNode);
        }
    }
}

} // end namespace
