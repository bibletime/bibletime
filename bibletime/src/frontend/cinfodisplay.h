/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CINFODISPLAY_H
#define CINFODISPLAY_H

//Backend
#include "backend/rendering/ctextrendering.h"

//Qt includes
#include <QWidget>
#include <QPair>
#include <QList>

//class forward declarations
class CReadDisplay;
class QAction;

namespace InfoDisplay {

class CInfoDisplay : public QWidget {
	Q_OBJECT
public:
	enum InfoType {
		Abbreviation,
		CrossReference,
		Footnote,
		Lemma,
		Morph,
		WordTranslation,
		WordGloss,
		Text
	};

	typedef QPair<InfoType, QString> InfoData;
	typedef QList<InfoData> ListInfoData;

	CInfoDisplay(QWidget *parent = 0);
	virtual ~CInfoDisplay();

	void setInfo(const InfoType, const QString& data);
	void setInfo(const ListInfoData&);
	void clearInfo();

protected:
	const QString decodeAbbreviation( const QString& data );
	const QString decodeCrossReference( const QString& data );
	const QString decodeFootnote( const QString& data );
	const QString decodeStrongs( const QString& data );
	const QString decodeMorph( const QString& data );
	const QString getWordTranslation( const QString& data );

protected slots:
	void lookup(const QString &, const QString &);

private:
	CReadDisplay* m_htmlPart;
	QAction* m_copyAction;
};

} //end of InfoDisplay namespace

#endif
