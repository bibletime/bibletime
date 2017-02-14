/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2017 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSet>
#include <QStringList>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <utility>


char const * inFilename;
QString inFilePath;

QRegularExpression const hasPotentialUnicodeEscape(
    "u[0123456789abcdefABCDEF]{4}");
QRegularExpression const unicodeEscape("\\\\u[0123456789abcdefABCDEF]{4}");

#define PASS do { writer.writeCurrentToken(reader); } while (false)


bool checkUnicodeEscape(QString & source,
                        QString const & locationFilename,
                        std::size_t const locationLine)
{
    QFile inFile(inFilePath + '/' + locationFilename);
    if (!inFile.open(QIODevice::ReadOnly)) {
        std::cerr << inFilename << ": Failed to open source file: "
                  << locationFilename.toStdString() << std::endl;
        return false;
    }
    QTextStream in(&inFile);
    for (std::size_t line = 1u; line < locationLine; ++line)
        if (in.readLine().isNull())
            return false;
    auto line(in.readLine());
    if (line.isNull())
        return false;
    line = line.trimmed();

    QStringList strings;
    int start = 0;
    for (;;) {
        start = line.indexOf('"', start);
        if (start < 0)
            break;
        ++start;
        auto end = line.indexOf('"', start);
        if (end < 0)
            break;
        if (start != end) {
            auto str(line.mid(start, end - start));
            strings.append(std::move(str));
        }
        start = end;
        ++start;
    }

    for (;;) {
        line = in.readLine();
        if (line.isNull())
            break;
        line = line.trimmed();
        if (line.isEmpty())
            continue;
        while (line.startsWith('"')) {
            auto end = line.indexOf('"', 1);
            if (end < 0)
                goto finish;
            if (end > 1) {
                auto str(line.mid(1, end - 1));
                strings.last().append(std::move(str));
            }
            line = line.mid(end + 1).trimmed();
        }
        if (!line.isEmpty())
            break;
    }

finish:

    QSet<QString> rStrings;
    for (auto const & str : strings) {
        auto s(str);
        s.replace("\\u", "u");
        if (source == s)
            rStrings.insert(str);
    }

    switch (rStrings.size()) {
        case 0:
            return false;
        case 1:
        {
            source = *rStrings.begin();
            int from = 0;
            for (;;) {
                int const i = source.indexOf(unicodeEscape, from);
                if (i < 0)
                    break;
                bool ok = false;
                ushort const codePoint = source.mid(i + 2, 4).toUShort(&ok, 16);
                assert(ok);
                source = (source.left(i) + QChar(codePoint))
                         + source.mid(i + 6);
                from = i + 1;
            }
            return true;
        }
        default:
            std::cerr << inFilename << ": Strange strings in "
                      << locationFilename.toStdString() << ':' << locationLine
                      << std::endl;
            return false;
    }
    if (rStrings.size() != 1)
    return false;

}

void readError(QXmlStreamReader & reader) {
    std::cerr << inFilename << ':' << reader.lineNumber() << ':'
            << reader.columnNumber() << ": "
            << reader.errorString().toStdString() << std::endl;
    std::exit(4);
}

void recurse(QXmlStreamReader & reader,
             QXmlStreamWriter & writer)
{
    std::size_t mud = 0u;
    QString locationFilename;
    unsigned long long locationLine = 0u;
    QString source;
    while (!reader.atEnd()) {
        switch (reader.readNext()) {

        case QXmlStreamReader::Invalid:
            readError(reader);

        case QXmlStreamReader::StartElement:
            if (mud) {
                ++mud;
                PASS;
            } else {
                if (reader.qualifiedName() == "location") {
                    auto const attrs(reader.attributes());
                    locationFilename = attrs.value("filename").toString();
                    locationLine = attrs.value("line").toULongLong();
                    ++mud;
                    PASS;
                } else if (reader.qualifiedName() == "source") {
                    PASS;
                    QString source(reader.readElementText());
                    if (source.contains(hasPotentialUnicodeEscape)) {
                        QString const oldSource(source);
                        if (!checkUnicodeEscape(source,
                                                locationFilename,
                                                locationLine))
                        {
                            std::cerr << inFilename << ": \""
                                      << source.toStdString() << "\" ("
                                      << locationFilename.toStdString() << ':'
                                      << locationLine << ") not substituted!"
                                      << std::endl;
                            assert(oldSource == source);
                        } else {
                            std::cerr << inFilename << ": \""
                                      << oldSource.toStdString() << "\" -> \""
                                      << source.toStdString() << "\" ("
                                      << locationFilename.toStdString() << ':'
                                      << locationLine << ") substituted."
                                      << std::endl;
                            assert(oldSource != source);
                        }
                    }
                    writer.writeCharacters(source);
                    writer.writeEndElement();
                } else {
                    ++mud;
                    PASS;
                }
            }
            break;
        case QXmlStreamReader::EndElement:
            if (mud) {
                --mud;
                PASS;
                break;
            } else {
                PASS;
                return;
            }
        default:
            PASS;
            break;
        };
    }
}

template <typename ... Paths>
void recurse(QXmlStreamReader & reader,
             QXmlStreamWriter & writer,
             char const * const path,
             Paths && ... paths)
{
    std::size_t mud = 0u;
    while (!reader.atEnd()) {
        switch (reader.readNext()) {

        case QXmlStreamReader::Invalid:
            readError(reader);

        case QXmlStreamReader::StartElement:
            if (mud) {
                ++mud;
                PASS;
            } else if (reader.qualifiedName() == path) {
                PASS;
                recurse(reader, writer, paths...);
            } else {
                ++mud;
                PASS;
            }
            break;
        case QXmlStreamReader::EndElement:
            if (mud) {
                --mud;
                PASS;
            } else {
                PASS;
                return;
            }
            break;
        default:
            PASS;
            break;
        };
    }
}

int main(int argc, char * argv[]) {
    QCoreApplication app(argc, argv);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file>"
                  << std::endl;
        return 1;
    }

    inFilename = argv[1];
    QFile inFile(inFilename);
    if (!inFile.open(QIODevice::ReadOnly)) {
        std::cerr << "Failed to open input file: " << inFilename << std::endl;
        return 2;
    }
    inFilePath = QFileInfo(inFilename).canonicalPath();

    QFile outFile(argv[2]);
    if (!outFile.open(QIODevice::WriteOnly)) {
        std::cerr << "Failed to open output file: " << argv[2] << std::endl;
        return 3;
    }


    QXmlStreamReader reader(&inFile);
    QXmlStreamWriter writer(&outFile);
    writer.setAutoFormatting(true);
    recurse(reader, writer, "TS", "context", "message");
}
