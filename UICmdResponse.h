#ifndef UICMDRESPONSE_H
#define UICMDRESPONSE_H

#include <QString>
#include <QList>

namespace au {

class UICmdResponse
{
public:

    class Item
    {
    public:

        char type;
        QString name;
        QString data;

        Item()
        {
        }

        Item(char t, const QString &n, const QString &d)
            : type(t), name(n), data(d)
        {
        }
    };

    class ItemWp
    {
    public:

        char type;
        QString name;
        QString lat;
        QString lon;
        QString alt;

        ItemWp()
        {
        }

        ItemWp(char t, const QString &n, const QString &la, const QString &lo, const QString &a)
            : type(t), name(n), lat(la), lon(lo), alt(a)
        {
        }
    };
    enum Error
    {
        E_OK = 0, E_BAD_COMMAND, E_INTERNAL_ERR
    };

    enum TypeCode
    {
        TYPE_CODE_NONE = 0,
        TYPE_CODE_INTEGER = 1,
        TYPE_CODE_BOOLEAN = 2,
        TYPE_CODE_STRING = 3,
        TYPE_CODE_FLOAT = 4
    };

    typedef QList<Item> Items;
    typedef QList<ItemWp> ItemsWp;

    UICmdResponse()
    {
    }

    UICmdResponse(const QString &xml)
    {
        fromXML(xml);
    }

    UICmdResponse(const QString &v, const QString &c, int e)
        : version(v), command(c), error(e)
    {
    }

    UICmdResponse(const QString &v, const QString &c, int e, const Items &i)
        : version(v), command(c), error(e), items(i)
    {
    }

    UICmdResponse(const QString &v, const QString &c, int e, const ItemsWp &i)
        : version(v), command(c), error(e), itemswp(i)
    {
    }


    QString version;
    QString command;
    int error;
    Items items;
    ItemsWp itemswp;

    bool fromXML(const QString xml);
    QString toXML() const;
    QString wptoXML() const;
};
}
#endif // UICMDRESPONSE_H
