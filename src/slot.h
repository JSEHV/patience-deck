#ifndef SLOT_H
#define SLOT_H

#include <QObject>
#include <libguile.h>

class Card;
class Aisleriot;
class AisleriotPrivate;
class Slot : public QObject
{
    Q_OBJECT

public:
    enum SlotType {
        UnknownSlot,
        ChooserSlot,
        FoundationSlot,
        ReserveSlot,
        StockSlot,
        TableauSlot,
        WasteSlot
    };

    enum ExpansionType {
        DoesNotExpand = 0x00,
        ExpandsInX = 0x01,
        ExpandsInY = 0x02,
        ExpandedAtX = 0x04,
        ExpandedAtY = 0x08,
    };
    Q_ENUM(ExpansionType)
    Q_DECLARE_FLAGS(ExpansionTypes, ExpansionType)

    Slot(int id, SlotType type, double x, double y,
         int expansionDepth, bool expandedDown, bool expandedRight);

    void setCards(SCM cards);

    bool expandsRight();
    bool expandedRight();
    void setExpansionToRight(double expansion);
    bool expandsDown();
    bool expandedDown();
    void setExpansionToDown(double expansion);

    SCM toSCM() const;

private:
    int m_id;
    SlotType m_type;
    QList<Card*> m_cards;
    bool m_exposed;
    double m_x;
    double m_y;
    double m_expansionDelta;
    ExpansionTypes m_expansion;
    int m_expansionDepth;
    bool m_needsUpdate;
};

#endif // SLOT_H
