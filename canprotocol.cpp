#include "canprotocol.h"

CanProtocol::CanProtocol()
{
    anDebugWrap(CanPtcDbgEn,
           anDebug("New Empty Object Created"));
}

CanProtocol::CanProtocol(quint8 identifier, const QByteArray &data)
    :QCanBusFrame(  identifier, data)
{
    anDebugWrap(CanPtcDbgEn,
           anDebug("New Object Created")
           anqDebug("   Id/hex="<<QByteArray::number(frameId(),16))
           anqDebug("   Data/hex="<<payload().toHex()));
}

CanProtocol::CanProtocol(const QCanBusFrame &CanMsg)
    :QCanBusFrame(  CanMsg.frameId(), CanMsg.payload())
{
    anDebugWrap(CanPtcDbgEn,
           anDebug("New Object Imported")
           anqDebug("   Id/hex="<<QByteArray::number(frameId(),16))
           anqDebug("   Data/hex="<<payload().toHex()));
}

void CanProtocol::operator=(const QCanBusFrame &CanMsg)
{
    setFrameId(CanMsg.frameId());
    setPayload(CanMsg.payload());
    anDebugWrap(CanPtcDbgEn,
           anDebug("Properties Assigned")
           anqDebug("   Id/hex="<<QByteArray::number(frameId(),16))
           anqDebug("   Data/hex="<<payload().toHex()));
}

bool CanProtocol::IsOfRightFormat() const
{
    if ((payload().size()==8) && ((frameId()>>8)==0))
    {
        anDebugWrap(CanPtcDbgEn,anDebug("OK"));
        return true;
    }
    anDebugWrap(CanPtcDbgEn,anDebug("KO"));
    return false;
}

CanProtocol &CanProtocol::SetSdcsId(quint8 sdcsid)
{
    setFrameId(frameId()|(sdcsid<<4));
    anDebugWrap(CanPtcDbgEn,
           anDebug("Property Changed")
           anqDebug("   Id/hex="<<QByteArray::number(frameId(),16)));
    return *this;
}

CanProtocol &CanProtocol::SetChId(quint8 chid)
{
    setFrameId(frameId()|chid);
    anDebugWrap(CanPtcDbgEn,
           anDebug("Property Changed")
           anqDebug("   Id/hex="<<QByteArray::number(frameId(),16)));
    return *this;
}

CanProtocol &CanProtocol::SetRFID(const QByteArray &rfid)
{
    QByteArray QBArrTmp( 5-rfid.size(), '\0');
    QBArrTmp << rfid;
    quint8 QInt8Tmp = payload().size();
    if (QInt8Tmp >= 8)
        setPayload(payload().replace(QInt8Tmp-8,5,QBArrTmp));
    else
    {
        if (QInt8Tmp >= 4)
            setPayload(payload().remove(0,QInt8Tmp-3));
        setPayload(payload().prepend(QBArrTmp));
    }
    anDebugWrap(CanPtcDbgEn,
           anDebug("Property Changed")
           anqDebug("   Data/hex="<<payload().toHex()));
    return *this;
}

CanProtocol &CanProtocol::SetValveControl(bool SetBit, bool ResetBit)
{
    quint8 QInt8Tmp = payload().size()-1;
    quint8 AByte = payload().at(QInt8Tmp);
    if (SetBit)
        AByte|=0x02;
    else
        AByte&=0xfd;
    if (ResetBit)
        AByte|=0x01;
    else
        AByte&=0xfe;
    setPayload(payload().remove(QInt8Tmp,1)<<AByte);
    anDebugWrap(CanPtcDbgEn,
           anDebug("Property Changed")
           anqDebug("   Data/hex="<<payload().toHex()));
    return *this;
}

CanProtocol &CanProtocol::SetValveOn()
{
    return SetValveControl(true,false);
}

CanProtocol &CanProtocol::SetValveOff()
{
    return SetValveControl(false,true);
}

const QCanBusFrame &CanProtocol::GetMsg() const
{
    return *this;
}

const QString CanProtocol::GetMsgStr() const
{
    return GetMsg().toString();
}

quint8 CanProtocol::GetSdcsId() const
{
    return (frameId()&0xf0)>>4;
}

quint8 CanProtocol::GetChId() const
{
    return frameId()&0x0f;
}

const QByteArray CanProtocol::GetRFID() const
{
    quint8 QInt8Tmp = payload().size();
    if (QInt8Tmp>=8)
        return payload().mid(QInt8Tmp-8,5);
    else
        return "@#$%&";
}

quint8 CanProtocol::GetValveControl() const
{
    return payload().at(payload().size()-1)&0x03;
}

quint8 CanProtocol::GetValveStatus() const
{
    return (payload().at(payload().size()-1)&0x0c)>>2;
}

const CanProtocol &CanProtocol::PresentRequest = * new CanProtocol(0xf0,".");
