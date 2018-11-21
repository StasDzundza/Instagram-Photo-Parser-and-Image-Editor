#include "instagram_account.h"

instagram_account::instagram_account()
{

}

void instagram_account::set_nickname(const QString &nickname)
{
    this->nickname = nickname;
}

QString &instagram_account::get_nickname()
{
    return this->nickname;
}

void instagram_account::set_count_likes(const int count_likes)
{
    this->count_likes = count_likes;
}

int instagram_account::get_count_likes()
{
    return count_likes;
}

void instagram_account::set_count_comments(const int count_comments)
{
    this->count_comments = count_comments;
}

int instagram_account::get_count_comments()
{
    return count_comments;
}

void instagram_account::set_count_followers(const int count_followers)
{
    this->count_followers = count_followers;
}

int instagram_account::get_count_followers()
{
    return count_followers;
}

void instagram_account::set_count_following(const int count_following)
{
    this->count_following = count_following;
}

int instagram_account::get_count_following()
{
    return count_following;
}

void instagram_account::set_count_posts(const int count_posts)
{
    this->count_posts = count_posts;
}

int instagram_account::get_count_posts()
{
    return count_posts;
}
