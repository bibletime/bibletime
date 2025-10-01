/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btbookchooserbutton.h"
#include "btbiblekeywidget.h"

#include <QApplication>
#include <QScreen>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWheelEvent>
#include <QMenu>
#include <QWidgetAction>

const unsigned int ARROW_HEIGHT = 15;

// BtTwoColumnBookWidget - Widget for displaying books in two columns within a QMenu
class BtTwoColumnBookWidget : public QWidget {
    Q_OBJECT

public:
    explicit BtTwoColumnBookWidget(const QStringList& books, QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setupLayout(books);
    }

Q_SIGNALS:
    void bookSelected(const QString& bookName);

private Q_SLOTS:
    void onBookButtonClicked() {
        if (QPushButton* button = qobject_cast<QPushButton*>(sender())) {
            QString bookName = button->text();
            Q_EMIT bookSelected(bookName);
        }
    }

private:
    void setupLayout(const QStringList& books) {
        // Main horizontal layout for two columns
        QHBoxLayout* mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(8, 8, 8, 8);
        mainLayout->setSpacing(12);
        
        // Left and right column layouts
        QVBoxLayout* leftColumn = new QVBoxLayout();
        QVBoxLayout* rightColumn = new QVBoxLayout();
        leftColumn->setSpacing(2);
        rightColumn->setSpacing(2);
        leftColumn->setAlignment(Qt::AlignTop);
        rightColumn->setAlignment(Qt::AlignTop);
        
        // Distribute books between columns
        int totalBooks = books.size();
        int booksPerColumn = (totalBooks + 1) / 2; // Round up for left column
        
        for (int i = 0; i < totalBooks; ++i) {
            QPushButton* button = new QPushButton(books[i], this);
            button->setFlat(true);
            button->setStyleSheet(
                "QPushButton {"
                "   text-align: left;"
                "   padding: 0 12px;"
                "   border: 1px solid transparent;"
                "   background: transparent;"
                "   min-width: 120px;"
                "   min-height: 24px;"
                "}"
                "QPushButton:hover {"
                "   background-color: rgba(128,128,128,0.1);"
                "}"
            );
            
            connect(button, &QPushButton::clicked, this, &BtTwoColumnBookWidget::onBookButtonClicked);
            
            // Add to left column for first half, right column for remainder
            if (i < booksPerColumn) {
                leftColumn->addWidget(button);
            } else {
                rightColumn->addWidget(button);
            }
        }
        
        mainLayout->addLayout(leftColumn, 1);
        mainLayout->addLayout(rightColumn, 1);
        
        // Set reasonable size
        setMinimumSize(300, 400);
        setMaximumSize(600, 800);
    }
};

// BtBookChooserButton implementation
BtBookChooserButton::BtBookChooserButton(BtBibleKeyWidget& parent)
    : QToolButton(&parent)
    , m_parent(parent)
    , m_menu(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAutoRaise(false);
    setArrowType(Qt::NoArrow);
    setFixedHeight(ARROW_HEIGHT);
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet(
        "QToolButton{margin:0px;}"
        "QToolButton::menu-indicator{"
        "   subcontrol-position:center center"
        "}"
    );
    
    // Create the menu
    m_menu = new QMenu(this);
    setMenu(m_menu);
    setPopupMode(QToolButton::InstantPopup);
    
    // Connect to populate menu when it's about to show
    connect(m_menu, &QMenu::aboutToShow, this, &BtBookChooserButton::populateMenu);
}

void BtBookChooserButton::populateMenu() {
    // Clear existing menu items
    m_menu->clear();
    
    // Get books from parent module
    if (!m_parent.module()) {
        return;
    }
    
    QStringList books = m_parent.module()->books();
    if (books.isEmpty()) {
        return;
    }
    
    // Create the two-column widget
    BtTwoColumnBookWidget* bookWidget = new BtTwoColumnBookWidget(books, m_menu);
    
    // Connect the book selection signal
    connect(bookWidget, &BtTwoColumnBookWidget::bookSelected, 
            this, &BtBookChooserButton::onBookSelected);
    
    // Create a QWidgetAction to hold our custom widget
    QWidgetAction* widgetAction = new QWidgetAction(m_menu);
    widgetAction->setDefaultWidget(bookWidget);
    m_menu->addAction(widgetAction);
}

void BtBookChooserButton::onBookSelected(const QString& bookName) {
    // Hide menu and emit signals
    m_menu->hide();
    Q_EMIT bookSelected(bookName);
}

void BtBookChooserButton::wheelEvent(QWheelEvent* e) {
    int const delta = e->angleDelta().y();
    if (delta == 0) {
        e->ignore();
    } else {
        Q_EMIT stepItem((delta > 0) ? -1 : 1);
        e->accept();
    }
}

#include "btbookchooserbutton.moc"
