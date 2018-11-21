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
    void set_nickname(const QString&nickname);
    QString &get_nickname();
    void set_count_likes(const int count_likes);
    int get_count_likes();
    void set_count_comments(const int count_comments);
    int get_count_comments();
private:
    QString nickname = "";
    int count_likes = 0,count_comments = 0,count_subscribers = 0,count_you_subscribed = 0;
    vector<QImage*> images;
};

#endif // INSTAGRAM_ACCOUNT_H
