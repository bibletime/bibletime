/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btsearchsyntaxhelpdialog.h"

#include <QColor>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFont>
#include <QPalette>
#include <QTextBrowser>
#include <QVBoxLayout>
#include "../../util/btconnect.h"
#include "../messagedialog.h"


class QUrl;

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
    m_textBrowser->setOpenLinks(false);
    BT_CONNECT(m_textBrowser, &QTextBrowser::anchorClicked,
               [this](QUrl const & url) {
                   if (url.scheme().isEmpty()) {
                       if (auto fragment = url.fragment(); !fragment.isEmpty())
                           m_textBrowser->scrollToAnchor(std::move(fragment));
                   } else {
                       QDesktopServices::openUrl(url);
                   }
               });
    l->addWidget(m_textBrowser);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    BT_CONNECT(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    l->addWidget(m_buttons);

    setLayout(l);

    retranslateUi();
}

void BtSearchSyntaxHelpDialog::retranslateUi() {
    QString theTitle(tr("Search Syntax Help"));
    setWindowTitle(theTitle);

    m_textBrowser->setHtml(
        QStringLiteral(
            "<html><head><title>%1</title>"
            "<style type=\"text/css\">"
                "body{"
                    "background-color:%2;"
                    "color:%3"
                "}h3{"
                    "font-weight:bold;"
                    "text-align:center"
                "}a{"
                    "text-decoration:underline"
                "}a:link{"
                    "color:%4"
                "}a:visited{"
                    "color:%5"
                "}h1 a{"
                    "font-size:medium"
                "}table{"
                    "margin-left:2em;"
                    "border-collapse:collapse"
                "}td{"
                    "padding:0.2em 0.3em;"
                    "border:3px solid %6;"
                    "color:%7;"
                    "background-color:%8;"
                "}ul#links{"
                    "list-style-type:none"
                "}"
            "</style></head><body>"
                "<p>%9</p>"
                "<p><ul id='links'>"
                    "<li><a href='#allsome'>%10</a></li>"
                    "<li><a href='#grouping'>%11</a></li>"
                    "<li><a href='#wildcards'>%12</a></li>"
                    "<li><a href='#fields'>%13</a></li>"
                    "<li><a href='#lucene'>%14</a></li>"
                "</ul></p>"
                "<h1><a name='allsome'>%15</a></h1>"
                "<p>%16</p>"
                "<p><table>"
                    "<tr><td><code>%17</code></td><td>%18</td></tr>"
                    "<tr><td><code>%19</code></td><td>%20</td></tr>"
                    "<tr><td><code>%21</code></td><td>%22</td></tr>"
                    "<tr><td><code>%23</code></td><td>%24</td></tr>"
                "</table></p>"
                "<h1><a name='grouping'>%25</a></h1>"
                "<p>%26</p>"
                "<p><table>"
                    "<tr><td><code>%27</code></td><td>%28</td></tr>"
                    "<tr><td><code>%29</code></td><td>%30</td></tr>"
                    "<tr><td><code>%31</code></td><td>%32</td></tr>"
                "</table></p>"
                "<h1><a name='wildcards'>%33</a></h1>"
                "<p>%34</p>"
                "<p><table>"
                    "<tr><td><code>%35</code></td><td>%36</td></tr>"
                    "<tr><td><code>%37</code></td><td>%38</td></tr>"
                    "<tr><td><code>%39</code></td><td>%40</td></tr>"
                    "<tr><td><code>%41</code></td><td>%42</td></tr>"
                "</table></p>"
                "<h1><a name='fields'>%43</a></h1>"
                "<p>"
                    "%44<br/>"
                    "<table>"
                        "<tr><td><code>heading:</code></td><td>%45</td></tr>"
                        "<tr><td><code>footnote:</code></td><td>%46</td></tr>"
                        "<tr><td><code>strong:</code></td><td>%47</td></tr>"
                        "<tr><td><code>morph:</code></td><td>%48</td></tr>"
                    "</table>"
                "</p>"
                "<p>"
                    "%49<br/>"
                    "<table>"
                        "<tr><td><code>%50</code></td><td>%51</td></tr>"
                        "<tr><td><code>%52</code></td><td>%53</td></tr>"
                        "<tr><td><code>%54</code></td><td>%55</td></tr>"
                        "<tr><td><code>%56</code></td><td>%57</td></tr>"
                    "</table>"
                "</p>"
                "<h1><a name='lucene'>%58</a></h1>"
                "<p>%59</p>"
            "</body></html>"
            )
        .arg(theTitle,
             palette().color(QPalette::Window).name(),
             palette().color(QPalette::WindowText).name(),
             palette().color(QPalette::Link).name(),
             palette().color(QPalette::LinkVisited).name(),
             palette().color(QPalette::WindowText).name(),
             palette().color(QPalette::Text).name(),
             palette().color(QPalette::Base).name(), // %8
             tr("This help is mainly for 'Full syntax' option. 'All words' and "
                "'Some words' options have more limited syntax; "
                "<a href=\"#%1\">wildcards</a> and <a href=\"#%2\">text "
                "fields</a> are supported for them. Some other syntax features "
                "may give strange or wrong results with All words/Some words.")
             .arg(QStringLiteral("wildcards"), QStringLiteral("fields")),
             tr("Which words to find"), // %10
             tr("Grouping and order"),
             tr("Wildcards (partial words)"),
             tr("Text fields (different parts of text)"),
             tr("Other syntax features"),
             tr("Which words to find"), // %15
             tr("Search terms are separated by spaces. <code>AND</code> "
                "(all words), <code>OR</code> (some words) and "
                "<code>NOT</code> (not the following word) can be added "
                "between the words. If none is added explicitly "
                "<code>OR</code> is used automatically. <code>+word</code> "
                "means the word must be in the results, <code>-word</code> "
                "means it must not be in the results.",
                "Do not translate \"AND\", \"OR\" or \"NOT\"."),
             tr("jesus AND god", "Do not translate \"AND\"."),
             tr("Finds verses with both 'Jesus' and 'God'"),
             tr("jesus OR god", "Do not translate \"OR\"."),
             tr("Finds verses with 'Jesus' or 'God' or both"), // %20
             tr("jesus NOT god", "Do not translate \"NOT\"."),
             tr("Finds verses with 'Jesus' but with no 'God'"),
             tr("+jesus -god"),
             tr("Finds verses with 'Jesus' but with no 'God'"),
             tr("Grouping and order"), // %25
             tr("Words can be grouped with <strong>parenthesis</strong>. Strict"
                " word order can be defined with <strong>quotes</strong>."),
             tr("(a AND b) OR c", "Do not translate \"AND\" or \"OR\"."),
             tr("Finds verses with both 'a' AND 'b', and verses with 'c'"),
             tr("\"says lord\""),
             tr("Finds e.g. '...Isaiah says, \"Lord...' but not '...says the "
                "LORD'"), // %30
             tr("\"says the lord\""),
             tr("Finds all verses with 'says the LORD'"),
             tr("Wildcards (partial words)"),
             tr("<code>*</code> matches any sequence of 0 or more "
                "characters, while <code>?</code> matches any single "
                "character. A wildcard can not be used in the beginning of a "
                "word."),
             tr("a*"), // %35
             tr("All words beginning with 'a'"),
             tr("a*a"),
             tr("'Assyria', 'aroma', 'abba' etc."),
             tr("a?"),
             tr("'at' and 'an'"), // %40
             tr("a??a"),
             tr("'abba', 'area', 'Asia' etc."),
             tr("Text fields (different parts of text)"),
             tr("Available text fields:" ),
             tr("Searches headings"), // %45
             tr("Searches footnotes"),
             tr("Searches Strong's numbers"),
             tr("Searches morphology codes"),
             tr("Examples:"),
             tr("heading:Jesus", "Do not translate \"heading:\"."), // %50
             tr("Finds headings with 'Jesus'"),
             tr("footnote:Jesus AND footnote:said",
                "Do not translate \"footnote:\" or \"AND\"."),
             tr("Finds footnotes with 'Jesus' and 'said'"),
             tr("strong:G846", "Do not translate \"strong:\"."),
             tr("Finds verses with Strong's Greek number 846"), // %55
             tr("morph:\"N-NSF\"", "Do not translate \"morph:\"."),
             tr("Finds verses with morphology code 'N-NSF'"),
             tr("Other syntax features"),
             tr("BibleTime uses the CLucene search engine. You can read more "
                "on the <a href=\"%1\">lucene syntax web page</a> (in external "
                "browser).")
             .arg(QStringLiteral("http://lucene.apache.org/java/1_4_3/"
                                 "queryparsersyntax.html"))
        ));

    message::prepareDialogBox(m_buttons);
}

} // namespace Search
