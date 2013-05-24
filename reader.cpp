#include "reader.h"

Reader::Reader()
{
    _number = 0;
    _placeId = 0;
    _address = "";
    _isConnected = false;
}

/*Reader::Reader(int number, int placeId, QString address, bool isConnected)
{
    _number = number;
    _placeId = placeId;
    _address = address;
    _isConnected = isConnected;
}*/

Reader::Reader(const Reader& reader)
{
    _number = reader._number;
    _placeId = reader._placeId;
    _address = reader._address;
    _isConnected = reader._isConnected;
}

Reader::~Reader()
{

}

unsigned int Reader::number() const
{
    return _number;
}

unsigned int Reader::placeId() const
{
    return _placeId;
}

QString Reader::address() const
{
    return _address;
}

bool Reader::isConnected() const
{
    return _isConnected;
}

void Reader::number(unsigned int number)
{
    _number = number;
}

void Reader::placeId(unsigned int placeId)
{
    _placeId = placeId;
}

void Reader::address(QString address)
{
    _address = address;
}

void Reader::isConnected(bool isConnected)
{
    _isConnected = isConnected;
}

