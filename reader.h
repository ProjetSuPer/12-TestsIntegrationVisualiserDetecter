#ifndef READER_H
#define READER_H

#include <QString>
#include <QMetaType>

class Reader
{
public:
    Reader();
    //Reader(int=0, int=0, QString="", bool=false);
    Reader(const Reader&);
    ~Reader();

    unsigned int number() const;
    unsigned int placeId() const;
    QString address() const;
    bool isConnected() const;

    void number(unsigned int);
    void placeId(unsigned int);
    void address(QString);
    void isConnected(bool);

private:
    unsigned int _number;
    unsigned int _placeId;
    QString _address;
    bool _isConnected;
};

Q_DECLARE_METATYPE(Reader)

#endif // READER_H
