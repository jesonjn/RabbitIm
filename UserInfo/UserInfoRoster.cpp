#include "UserInfoRoster.h"
#include "../Global/Global.h"
#include <QSettings>
#include <QBuffer>
#include <QImageReader>
#include <QImageWriter>

CUserInfoRoster::CUserInfoRoster(QObject *parent) : CUserInfo(parent)
{
}

int CUserInfoRoster::UpdateUserInfo(const QXmppRosterIq::Item &rosterItem)
{
    m_szJid = rosterItem.bareJid();
    m_Group = rosterItem.groups();
    return 0;
}

int CUserInfoRoster::UpdateUserInfo(const QXmppVCardIq &vCard, QString jid)
{
    int nRet = 0;
    nRet = CUserInfo::UpdateUserInfo(vCard, jid);
    if(nRet)
        return nRet;

    //保存头像到本地  
    QImageWriter imageWriter(CGlobal::Instance()->GetFileUserAvatar(GetBareJid()), "png");
    if(!imageWriter.write(GetPhoto()))
        LOG_MODEL_ERROR("CUserInfo", "Save avater error, %s", imageWriter.errorString().toStdString().c_str());
    return 0;
}

QXmppPresence::AvailableStatusType CUserInfoRoster::GetStatus()
{
    return m_LocalStatus;
}

int CUserInfoRoster::SetStatus(QXmppPresence::AvailableStatusType status)
{
    m_LocalStatus = status;
    return 0;
}

QDataStream & operator <<(QDataStream &output, const CUserInfoRoster &roster)
{
    output << (CUserInfo&)roster;
    output << roster.m_Group.size();
    QString szGroup;
    foreach(szGroup, roster.m_Group)
    {
        output << szGroup;
    }
    return output;
}

QDataStream & operator >>(QDataStream &input, CUserInfoRoster &roster)
{
    input >> (CUserInfo&)roster;
    int nSize = 0;
    input >> nSize;
    while(nSize--)
    {
        QString szGroup;
        input >> szGroup;
        roster.m_Group << szGroup;
    }
    return input;
}
