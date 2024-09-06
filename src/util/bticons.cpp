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

#include "bticons.h"

#include <QBrush>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QIconEngine>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <Qt>
#include <QtGui>
#include "directory.h"


class QSize;

namespace {

class BtOverlayIconEngine: public QIconEngine {

public: // methods:

    BtOverlayIconEngine(QIcon const & icon, QIcon const & overlay)
        : m_icon(icon)
        , m_overlayIcon(overlay)
    {}

    QIconEngine * clone() const override
    { return new BtOverlayIconEngine(m_icon, m_overlayIcon); }

    void paint(QPainter * painter,
               QRect const & rect,
               QIcon::Mode mode,
               QIcon::State state) override
    {
        {
            QBrush brush(painter->background());
            brush.setColor(Qt::transparent);
            painter->setBackground(brush);
        }
        painter->eraseRect(rect);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        m_icon.paint(painter, rect, Qt::AlignCenter, mode, state);
        m_overlayIcon.paint(painter,
                            rect.adjusted(0.4 * rect.width(),
                                          0.4 * rect.height(),
                                          0,
                                          0),
                            Qt::AlignCenter,
                            mode,
                            state);
    }

    QPixmap pixmap(QSize const & size,
                   QIcon::Mode mode,
                   QIcon::State state) override
    {
        QImage img(size, QImage::Format_ARGB32);
        img.fill(qRgba(0,0,0,0));
        QPixmap pix = QPixmap::fromImage(img, Qt::NoFormatConversion);
        QPainter painter(&pix);
        this->paint(&painter, QRect(QPoint(0, 0), size), mode, state);
        return pix;
    }

private: // fields:

    QIcon const & m_icon;
    QIcon const & m_overlayIcon;

};

} // anonymous namespace

BtIcons::RegularIcon::RegularIcon(QString const & name)
    : QIcon(util::directory::getIconDir().filePath(name))
{}

BtIcons::OverlayedIcon::OverlayedIcon(QIcon const & icon,
                                      QIcon const & overlayedIcon)
    : QIcon(new BtOverlayIconEngine(icon, overlayedIcon))
{}

BtIcons * BtIcons::m_instance = nullptr;

BtIcons::BtIcons()
    /* Regular icons: */
    : icon_add(QStringLiteral("add.svg"))
    , icon_audio(QStringLiteral("audio.svg"))
    , icon_automatically(QStringLiteral("automatically.svg"))
    , icon_back(QStringLiteral("back.svg"))
    , icon_bible(QStringLiteral("bible.svg"))
    , icon_bibletime(QStringLiteral("bibletime.svg"))
    , icon_book(QStringLiteral("book.svg"))
    , icon_bookmark(QStringLiteral("bookmark.svg"))
    , icon_books(QStringLiteral("books.svg"))
    , icon_calendar(QStringLiteral("calendar.svg"))
    , icon_cascade(QStringLiteral("cascade.svg"))
    , icon_checkbox(QStringLiteral("checkbox.svg"))
    , icon_commentary(QStringLiteral("commentary.svg"))
    , icon_configure(QStringLiteral("configure.svg"))
    , icon_contents2(QStringLiteral("contents2.svg"))
    , icon_delete(QStringLiteral("delete.svg"))
    , icon_dictionary(QStringLiteral("dictionary.svg"))
    , icon_displayconfig(QStringLiteral("displayconfig.svg"))
    , icon_document_magnifier(QStringLiteral("document_magnifier.svg"))
    , icon_edit_clear_locationbar(QStringLiteral("edit_clear_locationbar.svg"))
    , icon_edit_copy(QStringLiteral("edit_copy.svg"))
    , icon_exit(QStringLiteral("exit.svg"))
    , icon_export(QStringLiteral("export.svg"))
    , icon_file_save(QStringLiteral("file_save.svg"))
    , icon_fileclose(QStringLiteral("fileclose.svg"))
    , icon_find(QStringLiteral("find.svg"))
    , icon_flag(QStringLiteral("flag.svg"))
    , icon_folder_open(QStringLiteral("folder-open.svg"))
    , icon_folder(QStringLiteral("folder.svg"))
    , icon_fonts(QStringLiteral("fonts.svg"))
    , icon_forward(QStringLiteral("forward.svg"))
    , icon_import(QStringLiteral("import.svg"))
    , icon_info(QStringLiteral("info.svg"))
    , icon_key_bindings(QStringLiteral("key_bindings.svg"))
    , icon_layer_visible_on(QStringLiteral("layer-visible-on.svg"))
    , icon_lexicon(QStringLiteral("lexicon.svg"))
    , icon_light_bulb(QStringLiteral("light_bulb.svg"))
    , icon_lock(QStringLiteral("lock.svg"))
    , icon_manual(QStringLiteral("manual.svg"))
    , icon_map(QStringLiteral("map.svg"))
    , icon_pencil(QStringLiteral("pencil.svg"))
    , icon_plus(QStringLiteral("plus.svg"))
    , icon_pointing_arrow(QStringLiteral("pointing_arrow.svg"))
    , icon_print(QStringLiteral("print.svg"))
    , icon_remove(QStringLiteral("remove.svg"))
    , icon_questionable(QStringLiteral("questionable.svg"))
    , icon_questionmark(QStringLiteral("questionmark.svg"))
    , icon_refresh(QStringLiteral("refresh.svg"))
    , icon_startconfig(QStringLiteral("startconfig.svg"))
    , icon_stop(QStringLiteral("stop.svg"))
    , icon_swordconfig(QStringLiteral("swordconfig.svg"))
    , icon_sync(QStringLiteral("sync.svg"))
    , icon_tabbed(QStringLiteral("tabbed.svg"))
    , icon_text_bold(QStringLiteral("text_bold.svg"))
    , icon_text_center(QStringLiteral("text_center.svg"))
    , icon_text_italic(QStringLiteral("text_italic.svg"))
    , icon_text_leftalign(QStringLiteral("text_leftalign.svg"))
    , icon_text_rightalign(QStringLiteral("text_rightalign.svg"))
    , icon_text_under(QStringLiteral("text_under.svg"))
    , icon_tile(QStringLiteral("tile.svg"))
    , icon_tile_horiz(QStringLiteral("tile_horiz.svg"))
    , icon_tile_vert(QStringLiteral("tile_vert.svg"))
    , icon_trash(QStringLiteral("trash.svg"))
    , icon_unlock(QStringLiteral("unlock.svg"))
    , icon_view_tree(QStringLiteral("view-tree.svg"))
    , icon_view_index(QStringLiteral("view_index.svg"))
    , icon_view_mag(QStringLiteral("view_mag.svg"))
    , icon_view_profile(QStringLiteral("view_profile.svg"))
    , icon_window_fullscreen(QStringLiteral("window_fullscreen.svg"))
    /* Overlayed icons: */
    , icon_bible_add(icon_bible, icon_add)
    , icon_bible_install(icon_bible, icon_plus)
    , icon_bible_locked(icon_bible, icon_lock)
    , icon_bible_remove(icon_bible, icon_remove)
    , icon_bible_uninstall(icon_bible, icon_delete)
    , icon_book_add(icon_book, icon_add)
    , icon_book_locked(icon_book, icon_lock)
    , icon_cascade_auto(icon_cascade, icon_automatically)
    , icon_commentary_add(icon_commentary, icon_add)
    , icon_commentary_locked(icon_commentary, icon_lock)
    , icon_folder_add(icon_folder, icon_add)
    , icon_lexicon_add(icon_lexicon, icon_add)
    , icon_lexicon_locked(icon_lexicon, icon_lock)
    , icon_tile_auto(icon_tile, icon_automatically)
    , icon_tile_horiz_auto(icon_tile_horiz, icon_automatically)
    , icon_tile_vert_auto(icon_tile_vert, icon_automatically)
{
    BT_ASSERT(!m_instance);
    m_instance = this;
}

QString iconToHtml(QIcon const & icon, int const extent) {
    QByteArray bytes;
    QBuffer buffer(&bytes);
    icon.pixmap(extent).save(&buffer, "PNG");
    return "<img src=\"data:image/png;base64," + bytes.toBase64() + "\" />";
}
