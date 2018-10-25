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

#include "frontend/searchdialog/btsearchsyntaxhelpdialog.h"

#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QVBoxLayout>
#include "frontend/messagedialog.h"
#include "util/btconnect.h"


namespace Search {

BtSearchSyntaxHelpDialog::BtSearchSyntaxHelpDialog(QWidget *parent, Qt::WindowFlags wflags)
    : QDialog(parent, wflags)
{
    resize(550, 340);

    QVBoxLayout *l = new QVBoxLayout;

    m_textBrowser = new QTextBrowser(this);
    QFont font = m_textBrowser->font();
    font.setPointSize(font.pointSize()+2);
    m_textBrowser->setFont(font);
    BT_CONNECT(m_textBrowser, SIGNAL(anchorClicked(QUrl)),
               this,      SLOT(linkClicked(QUrl)));
    l->addWidget(m_textBrowser);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    BT_CONNECT(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
    l->addWidget(m_buttons);

    setLayout(l);

    retranslateUi();
}

void BtSearchSyntaxHelpDialog::retranslateUi() {
    QString theTitle(tr("Search Syntax Help"));
    setWindowTitle(theTitle);

    QString html("<html><head><title>");
    html += theTitle;
    html += "</title><style type=\"text/css\">"
        "body{"
            "background-color:";
    html += palette().color(QPalette::Window).name();
    html += ";"
            "color:";
    html += palette().color(QPalette::WindowText).name();
    html +=
        "}h3{"
            "font-weight:bold;"
            "text-align:center"
        "}a{"
            "text-decoration:underline"
        "}a:link{"
            "color:";
    html += palette().color(QPalette::Link).name();
    html +=
        "}a:visited{"
            "color:";
    html += palette().color(QPalette::LinkVisited).name();
    html +=
        "}h1 a{"
            "font-size:medium"
        "}table{"
            "margin-left:2em;"
            "border-collapse:collapse"
        "}td{"
            "padding:0.2em 0.3em;"
            "border:3px solid ";
    html += palette().color(QPalette::Foreground).name();
    html += ";"
            "color:";
    html += palette().color(QPalette::Text).name();
    html += ";"
            "background-color:";
    html += palette().color(QPalette::Base).name();
    html +=
        "}ul#links{"
            "list-style-type:none"
        "}"
        "</style></head><body><p>";

    html += tr("This help is mainly for 'Full syntax' option. 'All words' and 'Some words' "
        "options have more limited syntax; <a href='#wildcards'>wildcards</a> and "
        "<a href='#fields'>text fields</a> are supported for them. Some other syntax "
        "features may give strange or wrong results with All words/Some words.");
    html += "</p><p><ul id='links'><li><a href='#allsome'>";
    html += tr("Which words to find");
    html += "</a></li><li><a href='#grouping'>";
    html += tr("Grouping and order");
    html += "</a></li><li><a href='#wildcards'>";
    html += tr("Wildcards (partial words)");
    html += "</a></li><li><a href='#fields'>";
    html += tr("Text fields (different parts of text)");
    html += "</a></li><li><a href='#lucene'>";
    html += tr("Other syntax features");

    html += "</a></li></ul></p><h1><a name='allsome'>";
    html += tr("Which words to find");
    html += "</a></h1><p>";
    html += tr("Search terms are separated by spaces. <strong>AND</strong> (all words), "
        "<strong>OR</strong> (some words) and <strong>NOT</strong> (not the following word) "
        "can be added between the words. If none is added explicitly OR is used "
        "automatically. '<strong>+</strong>word' means the word must be in the results, "
        "'<strong>-</strong>word' means it must not be in the results.",
        "Do not translate \"AND\", \"OR\" or \"NOT\".");
    html += "</p><p><table><tr><td>";
    html += tr("jesus AND god", "Do not translate \"AND\".");
    html += "</td><td>";
    html += tr("Finds verses with both 'Jesus' and 'God'");
    html += "</td></tr><tr><td>";
    html += tr("jesus OR god", "Do not translate \"OR\".");
    html += "</td><td>";
    html += tr("Finds verses with 'Jesus' or 'God' or both");
    html += "</td></tr><tr><td>";
    html += tr("jesus NOT god", "Do not translate \"NOT\".");
    html += "</td><td>";
    html += tr("Finds verses with 'Jesus' but with no 'God'");
    html += "</td></tr><tr><td>";
    html += tr("+jesus -god");
    html += "</td><td>";
    html += tr("Finds verses with 'Jesus' but with no 'God'");

    html += "</td></tr></table></p><h1><a name='grouping'>";
    html += tr("Grouping and order");
    html += "</a></h1><p>";
    html += tr("Words can be grouped with <strong>parenthesis</strong>. Strict word order "
        "can be defined with <strong>quotes</strong>.");
    html += "</p><p><table><tr><td>";
    html += tr("(a AND b) OR c", "Do not translate \"AND\" or \"OR\".");
    html += "</td><td>";
    html += tr("Finds verses with both 'a' AND 'b', and verses with 'c'");
    html += "</td></tr><tr><td>";
    html += tr("\"says lord\"");
    html += "</td><td>";
    html += ("Finds e.g. '...Isaiah says, \"Lord...' but not '...says the LORD'");
    html += "</td></tr><tr><td>";
    html += tr("\"says the lord\"");
    html += "</td><td>";
    html += tr("Finds all verses with 'says the LORD'");

    html += "</td></tr></table></p><h1><a name='wildcards'>";
    html += tr("Wildcards (partial words)");
    html += "</a></h1><p>";
    html += tr("'<strong>*</strong>' matches any sequence of 0 or more characters, while "
        "'<strong>?</strong>' matches any single character. A wildcard can not be used in "
        "the beginning of a word.");
    html += "</p><p><table><tr><td>";
    html += tr("a*");
    html += "</td><td>";
    html += tr("All words beginning with 'a'");
    html += "</td></tr><tr><td>";
    html += tr("a*a");
    html += "</td><td>";
    html += tr("'Assyria', 'aroma', 'abba' etc.");
    html += "</td></tr><tr><td>";
    html += tr("a?");
    html += "</td><td>";
    html += tr("'at' and 'an'");
    html += "</td></tr><tr><td>";
    html += tr("a??a");
    html += "</td><td>";
    html += tr("'abba', 'area', 'Asia' etc.");

    html += "</td></tr></table></p><h1><a name='fields'>";
    html += tr("Text fields (different parts of text)");
    html += "</a></h1><p>";
    html += tr("Available text fields:" );
    html += "<br/><table><tr><td>heading:</td><td>";
    html += tr("Searches headings");
    html += "</td></tr><tr><td>footnote:</td><td>";
    html += tr("Searches footnotes");
    html += "</td></tr><tr><td>strong:</td><td>";
    html += tr("Searches Strong's numbers");
    html += "</td></tr><tr><td>morph:</td><td>";
    html += tr("Searches morphology codes");
    html += "</td></tr></table></p><p>";
    html += tr("Examples:" );
    html += "<br/><table><tr><td>";
    html += tr("heading:Jesus", "Do not translate \"heading:\".");
    html += "</td><td>";
    html += tr("Finds headings with 'Jesus'");
    html += "</td></tr><tr><td>";
    html += tr("footnote:Jesus AND footnote:said",
               "Do not translate \"footnote:\" or \"AND\".");
    html += "</td><td>";
    html += tr("Finds footnotes with 'Jesus' and 'said'");
    html += "</td></tr><tr><td>";
    html += tr("strong:G846", "Do not translate \"strong:\".");
    html += "</td><td>";
    html += tr("Finds verses with Strong's Greek number 846");
    html += "</td></tr><tr><td>";
    html += tr("morph:\"N-NSF\"", "Do not translate \"morph:\".");
    html += "</td><td>";
    html += tr("Finds verses with morphology code 'N-NSF'");
    html += "</td></tr></table></p><h1><a name='lucene'>";
    html += tr("Other syntax features");
    html += "</a></h1><p>";
    html += tr("BibleTime uses the CLucene search engine. You can read more on the "
            "<a href='%1'>lucene syntax web page</a> (in external browser).")
                .arg("http://lucene.apache.org/java/1_4_3/queryparsersyntax.html");
    html += "</p></body></html>";

    m_textBrowser->setHtml(html);

    message::prepareDialogBox(m_buttons);
}

void BtSearchSyntaxHelpDialog::linkClicked(const QUrl &url) {
    QDesktopServices::openUrl(url);
}

} // namespace Search
