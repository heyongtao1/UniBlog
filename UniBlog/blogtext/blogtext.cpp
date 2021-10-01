#include "blogtext.h"

BlogText::BlogText(QString _publishname,QString _posttime,QString _publishhead,QString _blogtexthead,int _blogtextid,QString _blogtext)
{
    setPublishName(_publishname);
}

void BlogText::setPublishName(QString _publishname)
{
    m_publishname = _publishname;
}

void BlogText::setPostTime(QString _posttime)
{
    m_posttime = _posttime;
}

void BlogText::setPublishHead(QString _publishhead)
{
    m_publishhead = _publishhead;
}

void BlogText::setBlogTextHead(QString _blogtexthead)
{
    m_blogtexthead = _blogtexthead;
}

void BlogText::setBlogTextId(int _blogtextid)
{
    m_blogtextid = _blogtextid;
}

void BlogText::setBlogText(QString _blogtext)
{
    m_blogtext = _blogtext;
}

QString BlogText::getPublishName()
{
    return m_publishname;
}

QString BlogText::getPostTime()
{
    return m_posttime;
}

QString BlogText::getPublishHead()
{
    return m_publishhead;
}

QString BlogText::getBlogTextHead()
{
    return m_blogtexthead;
}

int BlogText::getBlogTextId()
{
    return m_blogtextid;
}

QString BlogText::getBlogText()
{
    return m_blogtext;
}
