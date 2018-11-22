#ifndef INSTAGRAM_ACCOUNT_H
#define INSTAGRAM_ACCOUNT_H

#include<QString>
#include<QImage>
#include<vector>

using std::vector;

class instagram_account
{
public:
    instagram_account();
    void set_nickname(const QString &nickname);
    QString &get_nickname();
    void set_biography(const QString &biography);
    QString &get_biography();
    void set_count_likes(const int count_likes);
    int get_count_likes();
    void set_count_comments(const int count_comments);
    int get_count_comments();
    void set_count_followers(const int count_followers);
    int get_count_followers();
    void set_count_following(const int count_following);
    int get_count_following();
    void set_count_posts(const int count_posts);
    int get_count_posts();
    void set_id(QString&id);
    QString &get_id();
private:
    QString nickname = "";
    QString biography = "";
    int count_likes = 0,count_comments = 0,count_followers = 0,count_following = 0,count_posts = 0;
    QString id = "";
    //vector<QImage*> images;
};

#endif // INSTAGRAM_ACCOUNT_H
