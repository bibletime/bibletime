/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

var X = 0;
var Y = 0;
var attribs = [];
var eventType = "";
var prevNode = 0;
var currentNode = 0;

btHtmlJsObject.startTimer.connect(this, this.startTimer);
btHtmlJsObject.gotoAnchor.connect(this, this.gotoAnchor);

document.getElementsByTagName("body")[0].addEventListener ('mousedown', function (eve) { mouseDownHandler (eve); }, true);
document.getElementsByTagName("body")[0].addEventListener ('mousemove', function (eve) { mouseMoveHandler (eve); }, true);
document.getElementsByTagName("body")[0].addEventListener ('click',     function (eve) { mouseClickHandler (eve); }, true);

// Scroll window to html anchor
function gotoAnchor(anchor)
{
	document.location=document.location + "#" + anchor;
}

// Mouse button clicked handler
function mouseClickHandler (mEvent)
{
	var url = mEvent.target.getAttribute("href");
	btHtmlJsObject.mouseClick(url);
}

// Mouse button pressed down handler
function mouseDownHandler (mEvent)
{
	var node;
	var X;
	var Y;
	var url;
	url = mEvent.target.getAttribute("href");

	if (mEvent.button === 2) // Right mouse button
	{
		btHtmlJsObject.mouseDownRight(url);
	}
	if (mEvent.button === 0) // Left mouse button
	{
		if (!(mEvent.target === undefined)) 
		{
			X = mEvent.clientX;
			Y = mEvent.clientY;
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

		if (node.attributes.length > 0) 
		{
			attribList = getNodeAttributes(node);
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
		for (i = 0; i < node.attributes.length; i++)
		{
			attribList = attribList + node.attributes[i].nodeName + '=' + node.attributes[i].nodeValue + '|';
		}
	}
	return attribList;
}

// Start a timer event
function startTimer(time)
{
	var t=setTimeout("timerEvent()",time);
}

// Handles a timer event
function timerEvent()
{
	if (currentNode != 0  && currentNode == prevNode)
	{
		var attributes = getNodeAttributes(currentNode);
		btHtmlJsObject.timeOutEvent(attributes);
	}
}

;

