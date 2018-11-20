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
private:
    QString nickname;
    int count_likes = 0,count_subscribers = 0,count_you_subscribed = 0;
    vector<QImage*> images;
};

#endif // INSTAGRAM_ACCOUNT_H
