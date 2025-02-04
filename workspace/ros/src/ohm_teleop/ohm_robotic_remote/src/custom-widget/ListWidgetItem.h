#ifndef __LIST_WIDGET_ITEM__
#define __LIST_WIDGET_ITEM__

#include <QTime>
#include <QString>

class ListWidgetItem
{
public:

    enum Type {
        None = 0,
        ValidVictim,
        VictimRequest,
        Hazat,
        Qr,
        CountType
    };

    ListWidgetItem(void) : _time(QTime::currentTime()), _type(None) { }
    ListWidgetItem(const Type type, const QString& text) : _time(QTime::currentTime()), _type(type), _text(text) { }
    ListWidgetItem(const Type type, const QString& text, const QString& meta) :  _time(QTime::currentTime()),
                                                                                 _type(type), _text(text), _meta(meta) { }
    ListWidgetItem(const QTime& time, const Type type, const QString& text, const QString& meta)
        : _time(time), _type(type), _text(text), _meta(meta) { }

    const QTime& time(void) const { return _time; }
    const QString& text(void) const { return _text; }
    const QString& meta(void) const { return _meta; }
    Type type(void) const { return _type; }

    static QString typeName(const Type type);

private:
    QTime _time;
    Type _type;
    QString _text;
    QString _meta;
};

#endif
