#include "ListWidgetItem.h"

QString ListWidgetItem::typeName(const Type type)
{
    switch (type)
    {
    case None:
        return QString("None");
    case ValidVictim:
        return QString("Victim Val");
    case VictimRequest:
        return QString("Victim Req");
    case Hazat:
        return QString("Hazat");
    case Qr:
        return QString("Qr");
    default:
        return QString("Unkown");
    }
}
