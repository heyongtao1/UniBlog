#ifndef BLOGTEXT_H
#define BLOGTEXT_H

#include <QString>

class BlogText
{
public:
    BlogText(QString,QString,QString,QString,int,QString);
    ~BlogText();
    void setPublishName(QString _publishname);
    void setPostTime(QString _posttime);
    void setPublishHead(QString _publishhead);
    void setBlogTextHead(QString _blogtexthead);
    void setBlogTextId(int _blogtextid);
    void setBlogText(QString _blogtext);

    QString getPublishName();
    QString getPostTime();
    QString getPublishHead();
    QString getBlogTextHead();
    int getBlogTextId();
    QString getBlogText();
private:
    //发布者用户名
    QString m_publishname;
    //发布时间
    QString m_posttime;
    //发布者头像
    QString m_publishhead;
    //博客文章ID
    int m_blogtextid;
    //博客文章标题
    QString m_blogtexthead;
    //文章内容
    QString m_blogtext;
    //文本颜色、背景、加粗、斜体、字体.....
};

#endif // BLOGTEXT_H
