/*
 * Patience Deck is a collection of patience games.
 * Copyright (C) 2020  Tomi Leppänen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QPainter>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QSvgRenderer>
#include "table.h"
#include "card.h"
#include "constants.h"
#include "logging.h"
#include "slot.h"

namespace {

int getColumn(Rank rank) {
    switch (rank) {
    case RankAceHigh:
    case RankJoker:
    case BlackJoker:
        return 0;
    case RedJoker:
        return 1;
    case CardBack:
        return 2;
    default:
        return rank-1;
    }
}

int getRow(Rank rank, Suit suit)
{
    switch (rank) {
    case CardBack:
    case RankJoker:
    case BlackJoker:
    case RedJoker:
        return 4;
    default:
        return suit;
    }
}

} // namespace

Card::Card(const CardData &card, Table *table, Slot *slot)
    : QQuickItem(slot)
    , m_table(table)
    , m_slot(slot)
    , m_data(card)
    , m_drag(nullptr)
    , m_dirty(true)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QQuickItem::ItemHasContents);
    setSmooth(true);
}

QSGNode *Card::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto texture = cardTexture(size(), window());
    if (!texture)
        return oldNode;

    auto *node = static_cast<QSGSimpleTextureNode *>(oldNode);
    if (!node || m_dirty) {
        if (!node)
            node = new QSGSimpleTextureNode();
        node->setTexture(texture);
        int column = getColumn(show() ? rank() : CardBack);
        int row = getRow(show() ? rank() : CardBack, suit());
        QRect rect(column * width(), row * height(), width(), height());
        node->setSourceRect(rect);
        m_dirty = false;
    }
    node->setRect(boundingRect());
    return node;
}

QSizeF Card::size() const
{
    return QSizeF(width(), height());
}

void Card::setSize(const QSizeF &size)
{
    m_dirty = true;
    setWidth(size.width());
    setHeight(size.height());
}

Suit Card::suit() const
{
    return m_data.suit;
}

Rank Card::rank() const
{
    return m_data.rank;
}

bool Card::show() const
{
    return m_data.show;
}

bool Card::isBlack() const
{
    return m_data.suit == SuitClubs || m_data.suit == SuitSpade;
}

bool Card::dragged() const
{
    return m_drag != nullptr;
}

CardData Card::data() const
{
    return m_data;
}

bool Card::operator==(const Card &other) const
{
    return m_data == other.m_data;
}

void Card::mousePressEvent(QMouseEvent *event)
{
    qCDebug(lcMouse) << event << "for" << *this;

    if (m_drag)
        m_drag->cancel();

    setKeepMouseGrab(true);

    m_drag = new Drag(event, m_table, m_slot, this, sinceLastDrag());
    Drag *drag = m_drag;
    connect(m_drag, &Drag::destroyed, this, [this, drag] {
        if (m_drag == drag)
            m_drag = nullptr;
    });
}

void Card::mouseReleaseEvent(QMouseEvent *event)
{
    qCDebug(lcMouse) << event << "for" << *this;

    if (!m_drag) {
        qCCritical(lcPatience) << "Can not handle mouse release! There is no drag ongoing!";
        return;
    }

    m_drag->finish(event);

    setKeepMouseGrab(false);
}

void Card::mouseMoveEvent(QMouseEvent *event)
{
    qCDebug(lcMouse) << event << "for" << *this;

    if (!m_drag) {
        qCCritical(lcPatience) << "Can not handle mouse move! There is no drag ongoing!";
        return;
    }

    m_drag->update(event);
}

qint64 Card::sinceLastDrag()
{
    if (m_doubleClickTimer.isValid())
        return m_doubleClickTimer.restart();
    m_doubleClickTimer.start();
    return std::numeric_limits<qint64>::max();
}

QSvgRenderer *Card::cardRenderer()
{
    static QSvgRenderer *cardRenderer = nullptr;
    if (!cardRenderer)
        cardRenderer = new QSvgRenderer(Constants::DataDirectory + QStringLiteral("/anglo.svg"));
    return cardRenderer;
}

QSGTexture *Card::cardTexture(const QSizeF &cardSize, QQuickWindow *window)
{
    if (cardSize.isEmpty())
        return nullptr;

    static QSizeF previousCardSize;
    static QSGTexture *texture = nullptr;
    if (!texture || previousCardSize != cardSize) {
        auto renderer = cardRenderer();
        QSize size(cardSize.width()*13, cardSize.height()*5);
        QImage image(size, QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);
        renderer->render(&painter);
        if (texture)
            texture->deleteLater();
        texture = window->createTextureFromImage(image);
        connect(window, &QQuickWindow::sceneGraphInvalidated, texture, [&] {
            texture->deleteLater();
            texture = nullptr;
        });
        qCDebug(lcPatience) << "Drew new card texture for card size of" << cardSize;
    }
    previousCardSize = cardSize;
    return texture;
}

QDebug operator<<(QDebug debug, const Card &card)
{
    debug.nospace() << "Card(rank=";
    debug.nospace() << card.rank();
    debug.nospace() << ", suit=";
    debug.nospace() << card.suit();
    debug.nospace() << ", show=";
    debug.nospace() << card.show();
    debug.nospace() << ")";
    return debug.maybeSpace();
}
