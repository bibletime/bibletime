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

BtmModuleTextFilter::BtmModuleTextFilter() :
    m_showReferences(false) {
}

QString BtmModuleTextFilter::processText(const QString &text) {
    m_doc.setContent(text);
    QDomNodeList nodes = m_doc.elementsByTagName("html");
    if (nodes.count() != 1)
        return QString();
    QDomNode htmlNode = nodes.at(0);
    QDomElement bodyElement = htmlNode.firstChildElement("body");
    QDomNodeList childNodes = bodyElement.childNodes();
    traverse(childNodes);
    QString newText = m_doc.toString();
    return newText;
}

void BtmModuleTextFilter::traverse(const QDomNodeList& nodeList) {
    int count = nodeList.count();
    bool breakFound = false;
    QDomNode breakNode;
    for (int i=0; i<count; i++) {
        QDomNode node = nodeList.at(i);
        QDomNode::NodeType nodeType = node.nodeType();
        QDomElement element = node.toElement();
        if ( m_showReferences) {
            if (isFootnote(element)) {
                convertFootNoteToLink(element);
            }
            if (isLemmaOrMorph(element)) {
                convertLemmaAndMorphToLink(element);
            }
        }
        if (element.tagName() == "br") {
            if (breakFound) {
                breakNode = node;
                breakFound = false;
            }
            breakFound = true;
        }
        QDomNodeList childrenNodes = node.childNodes();
        traverse(childrenNodes);
    }
    if (!breakNode.isNull()) {
        QDomNode parentNode = breakNode.parentNode();
        parentNode.removeChild(breakNode);
    }
}

bool BtmModuleTextFilter::isFootnote(const QDomElement& element) const {
    QDomNamedNodeMap nodeMap = element.attributes();
    if (nodeMap.contains("class")) {
        QString classValue = getAttributeValue(element, "class");
        return classValue == "footnote";
    }
    return false;
}

void BtmModuleTextFilter::convertFootNoteToLink(QDomElement& element) {

    QString noteValue = getAttributeValue(element, "note");
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

            QString url = "sword://footnote/" + noteValue + "/" + textValue;
            aNode.setAttribute("href", url);

            QDomText textNode = m_doc.createTextNode(newText);
            aNode.appendChild(textNode);
        }
    }
}

bool BtmModuleTextFilter::isLemmaOrMorph(const QDomElement& element) const {
    QDomNamedNodeMap nodeMap = element.attributes();
    if (nodeMap.contains("lemma") || nodeMap.contains("morph"))
        return true;
    return false;
}

void BtmModuleTextFilter::convertLemmaAndMorphToLink(QDomElement& element) {

    QString value;
    if (elementHasAttribute(element, "lemma"))
        value += "lemma=" + getAttributeValue(element, "lemma") + "||";
    if (elementHasAttribute(element, "morph"))
        value += "morph=" + getAttributeValue(element, "morph") + "||";


    QDomNodeList childrenNodes = element.childNodes();
    int count = childrenNodes.count();
    for (int i=0; i<count; i++) {
        QDomNode node = childrenNodes.at(i);
        QDomNode::NodeType nodeType = node.nodeType();
        if (nodeType == QDomNode::TextNode){
            QDomText text = node.toText();
            QString textValue = text.data();
            int i=0;
            element.removeChild(node);

            QDomElement aNode = m_doc.createElement("a");
            element.appendChild(aNode);

            QString url = "sword://lemmamorph/" +
                    value + "/" + textValue;
            aNode.setAttribute("href", url);

            textValue += "  ";
            QDomText textNode = m_doc.createTextNode(textValue);
            aNode.appendChild(textNode);
        }
    }
}

bool BtmModuleTextFilter::elementHasAttribute(
        const QDomElement& element,
        const QString& attrName) const {
    QDomNamedNodeMap nodeMap = element.attributes();
    if (nodeMap.contains(attrName))
        return true;
    return false;
}

QString BtmModuleTextFilter::getAttributeValue(
        const QDomElement& element,
        const QString& attrName) const {
    QString value;
    QDomNamedNodeMap nodeMap = element.attributes();
    if (nodeMap.contains(attrName)) {
        QDomNode node = nodeMap.namedItem(attrName);
        QDomAttr attr = node.toAttr();
        value = attr.value();
    }
    return value;
}

void BtmModuleTextFilter::setShowReferences(bool on) {
    m_showReferences = on;
}

} // end namespace
