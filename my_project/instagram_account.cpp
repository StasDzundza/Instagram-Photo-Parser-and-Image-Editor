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
