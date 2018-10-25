/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

var X = 0;
var Y = 0;
var attribs = [];
var eventType = "";
var prevNode = 0;
var currentNode = 0;
var timeOutId = -1;

// Scroll window to html anchor
function gotoAnchor(anchor)
{
    document.location=document.location + "#" + anchor;
}

// Mouse button clicked handler
function mouseClickHandler (mEvent)
{
    var mTarget = mEvent.target;
    if (mTarget)
    {
        var url = "";
        var tmpUrl = mEvent.target.getAttribute("href");
        if (tmpUrl)
            url = tmpUrl;
        btHtmlJsObject.mouseClick(url);
    }
}

// Mouse button pressed down handler
function mouseDownHandler (mEvent)
{
    var node;
    var url = "";
    var lemma = "";
    var mTarget = mEvent.target;
    if (mTarget)
    {
        var tmpUrl = mEvent.target.getAttribute("href");
        if (tmpUrl)
            url = tmpUrl;
        var tmpLemma = mEvent.target.getAttribute("lemma");
        if (tmpLemma)
            lemma = tmpLemma;
    }

    if (mEvent.button === 2) // Right mouse button
    {
        btHtmlJsObject.mouseDownRight(url, lemma);
    }
    if (mEvent.button === 0) // Left mouse button
    {
        if (!(mEvent.target === undefined))
        {
            var X = mEvent.clientX;
            var Y = mEvent.clientY;
            btHtmlJsObject.mouseDownLeft(url, X, Y);
        }
    }
}

// Mouse moved event handler
function mouseMoveHandler (mEvent)
{
    currentNode = mEvent.target;
    var shiftKey = mEvent.shiftKey;
    var x = mEvent.clientX;
    var y = mEvent.clientY;
    var node = mEvent.target;
    if ( node != undefined && node != prevNode )
    {
        prevNode = node;
        var attribList;
        if (node.attributes.length > 0)
        {
            attribList = getNodeAttributes(node);
            if (attribList == "")
                attribList = getNodeAttributes(node.parentNode);
            btHtmlJsObject.mouseMoveEvent(attribList, x, y, shiftKey);
        }
    }
}

// Get attributes of a DOM node and put into a single string
function getNodeAttributes(node)
{
    var attribList = '';
    if (node.attributes.length > 0)
    {
        var i;
        for (i = 0; i < node.attributes.length; i++)
        {
            attribList = attribList + node.attributes[i].nodeName + '=' + node.attributes[i].value + '||';
        }
    }
    return attribList;
}

// Start a timer event
function startTimer(time)
{
    clearTimeout(timeOutId);
    timeOutId = setTimeout("timerEvent()",time);
}

// Handles a timer event
function timerEvent()
{
    timeOutId = -1;
    if (currentNode != 0  && currentNode == prevNode)
    {
        var attributes = getNodeAttributes(currentNode);
        if (attributes == "")
            attributes = getNodeAttributes(currentNode.parentNode);
        btHtmlJsObject.timeOutEvent(attributes);
    }
}

function selectAll () {
    console.log("select all");
}

document.getElementsByTagName("body")[0].addEventListener ('mousedown', function (eve) { mouseDownHandler (eve); }, true);
document.getElementsByTagName("body")[0].addEventListener ('mousemove', function (eve) { mouseMoveHandler (eve); }, true);
document.getElementsByTagName("body")[0].addEventListener ('click',     function (eve) { mouseClickHandler (eve); }, true);

var btHtmlJsObject = 0;

new QWebChannel(qt.webChannelTransport, function (channel) {
        btHtmlJsObject = channel.objects.btHtmlJsObject;
        btHtmlJsObject.startTimer.connect(startTimer);
        btHtmlJsObject.gotoAnchor.connect(gotoAnchor);
        btHtmlJsObject.selectAll.connect(selectAll);
    });


