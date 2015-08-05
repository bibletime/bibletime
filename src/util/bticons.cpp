/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bticons.h"

#include <QByteArray>
#include <QBuffer>
#include <QPixmap>
#include <QDebug>
#include <QIconEngine>
#include <QPainter>
#include "../util/directory.h"


namespace {

class BtOverlayIconEngine: public QIconEngine {

public: /* Methods: */

    BtOverlayIconEngine(QIcon const & icon, QIcon const & overlay)
        : m_icon(icon)
        , m_overlayIcon(overlay)
    {}

    virtual QIconEngine * clone() const
    { return new BtOverlayIconEngine(m_icon, m_overlayIcon); }

    virtual void paint(QPainter * painter,
                       QRect const & rect,
                       QIcon::Mode mode,
                       QIcon::State state)
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

    virtual QPixmap pixmap(QSize const & size,
                           QIcon::Mode mode,
                           QIcon::State state)
    {
        QImage img(size, QImage::Format_ARGB32);
        img.fill(qRgba(0,0,0,0));
        QPixmap pix = QPixmap::fromImage(img, Qt::NoFormatConversion);
        QPainter painter(&pix);
        this->paint(&painter, QRect(QPoint(0, 0), size), mode, state);
        return pix;
    }

private: /* Fields: */

    QIcon const & m_icon;
    QIcon const & m_overlayIcon;

};

} // anonymous namespace

BtIcons * BtIcons::m_instance = NULL;

BtIcons::BtIcons()
    /* Regular icons: */
    : icon_add(loadIcon("add"))
    , icon_back(loadIcon("back"))
    , icon_bible(loadIcon("bible"))
    , icon_bibletime(loadIcon("bibletime"))
    , icon_book(loadIcon("book"))
    , icon_bookmark(loadIcon("bookmark"))
    , icon_books(loadIcon("books"))
    , icon_calendar(loadIcon("calendar"))
    , icon_cascade(loadIcon("cascade"))
    , icon_cascade_auto(loadIcon("cascade_auto"))
    , icon_checkbox(loadIcon("checkbox"))
    , icon_commentary(loadIcon("commentary"))
    , icon_configure(loadIcon("configure"))
    , icon_contents2(loadIcon("contents2"))
    , icon_default(loadIcon("default"))
    , icon_dictionary(loadIcon("dictionary"))
    , icon_displayconfig(loadIcon("displayconfig"))
    , icon_displaytranslit(loadIcon("displaytranslit"))
    , icon_document_magnifier(loadIcon("document_magnifier"))
    , icon_edit_clear(loadIcon("edit_clear"))
    , icon_edit_clear_locationbar(loadIcon("edit_clear_locationbar"))
    , icon_edit_copy(loadIcon("edit_copy"))
    , icon_edit_delete(loadIcon("edit_delete"))
    , icon_edit_undo(loadIcon("edit_undo"))
    , icon_exit(loadIcon("exit"))
    , icon_export(loadIcon("export"))
    , icon_file_save(loadIcon("file_save"))
    , icon_fileclose(loadIcon("fileclose"))
    , icon_fileprint(loadIcon("fileprint"))
    , icon_find(loadIcon("find"))
    , icon_flag(loadIcon("flag"))
    , icon_folder_new(loadIcon("folder_new"))
    , icon_folder_open(loadIcon("folder-open"))
    , icon_folder(loadIcon("folder"))
    , icon_fontconfig(loadIcon("fontconfig"))
    , icon_fonts(loadIcon("fonts"))
    , icon_forward(loadIcon("forward"))
    , icon_import(loadIcon("import"))
    , icon_info(loadIcon("info"))
    , icon_key_bindings(loadIcon("key_bindings"))
    , icon_layer_visible_on(loadIcon("layer-visible-on"))
    , icon_lexicon(loadIcon("lexicon"))
    , icon_light_bulb(loadIcon("light_bulb"))
    , icon_lock(loadIcon("lock"))
    , icon_map(loadIcon("map"))
    , icon_pencil(loadIcon("pencil"))
    , icon_plus(loadIcon("plus"))
    , icon_pointing_arrow(loadIcon("pointing_arrow"))
    , icon_print(loadIcon("print"))
    , icon_remove(loadIcon("remove"))
    , icon_questionable(loadIcon("questionable"))
    , icon_questionmark(loadIcon("questionmark"))
    , icon_refresh(loadIcon("refresh"))
    , icon_startconfig(loadIcon("startconfig"))
    , icon_stop(loadIcon("stop"))
    , icon_swordconfig(loadIcon("swordconfig"))
    , icon_sync(loadIcon("sync"))
    , icon_tabbed(loadIcon("tabbed"))
    , icon_text_bold(loadIcon("text_bold"))
    , icon_text_center(loadIcon("text_center"))
    , icon_text_italic(loadIcon("text_italic"))
    , icon_text_leftalign(loadIcon("text_leftalign"))
    , icon_text_rightalign(loadIcon("text_rightalign"))
    , icon_text_under(loadIcon("text_under"))
    , icon_tile(loadIcon("tile"))
    , icon_tile_auto(loadIcon("tile_auto"))
    , icon_tile_horiz(loadIcon("tile_horiz"))
    , icon_tile_vert(loadIcon("tile_vert"))
    , icon_trash(loadIcon("trash"))
    , icon_unlock(loadIcon("unlock"))
    , icon_view_tree(loadIcon("view-tree"))
    , icon_view_index(loadIcon("view_index"))
    , icon_view_mag(loadIcon("view_mag"))
    , icon_view_profile(loadIcon("view_profile"))
    , icon_window_fullscreen(loadIcon("window_fullscreen"))
    /* Overlayed icons: */
    , icon_bible_add(new BtOverlayIconEngine(icon_bible, icon_add))
    , icon_bible_locked(new BtOverlayIconEngine(icon_bible, icon_lock))
    , icon_bible_remove(new BtOverlayIconEngine(icon_bible, icon_remove))
    , icon_book_add(new BtOverlayIconEngine(icon_book, icon_add))
    , icon_book_locked(new BtOverlayIconEngine(icon_book, icon_lock))
    , icon_commentary_add(new BtOverlayIconEngine(icon_commentary, icon_add))
    , icon_commentary_locked(new BtOverlayIconEngine(icon_commentary,
                                                     icon_lock))
    , icon_lexicon_add(new BtOverlayIconEngine(icon_lexicon, icon_add))
    , icon_lexicon_locked(new BtOverlayIconEngine(icon_lexicon, icon_lock))
{ m_instance = (Q_ASSERT(!m_instance), this); }

QIcon const & BtIcons::loadIcon(QString const & name) {
    {
        IconCache::const_iterator const i = m_iconCache.find(name);
        if (i != m_iconCache.end())
            return *i;
    }

    QString const iconFileName(util::directory::getIconDir().canonicalPath()
                               + "/" + name + ".svg");
    QIcon const & r = *m_iconCache.insert(name, QIcon(iconFileName));
    if (r.isNull())
        qWarning() << "Invalid icon file: " << iconFileName;
    return r;
}

QString iconToHtml(QIcon const & icon, int const extent) {
    QByteArray bytes;
    QBuffer buffer(&bytes);
    icon.pixmap(extent).save(&buffer, "PNG");
    return "<img src=\"data:image/png;base64," + bytes.toBase64() + "\" />";
}
