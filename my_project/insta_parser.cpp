#include "insta_parser.h"
#include "ui_insta_parser.h"
#include <iostream>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QDir>

insta_parser::insta_parser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::insta_parser)
{
    ui->setupUi(this);
    this->setFixedHeight(696);
    this->setFixedWidth(1096);
    check_existing_accounts();//check accounts from last program start
    network_manager = new QNetworkAccessManager(this);
    //network_manager_next_page = new QNetworkAccessManager(this);
    network_manager->setStrictTransportSecurityEnabled(true);

    connect(network_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
    //connect(network_manager_next_page, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinishedNextPage(QNetworkReply*)));

    manager_photo = new QNetworkAccessManager(this);
    manager_photo->setStrictTransportSecurityEnabled(true);
    connect(manager_photo, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinishedPhoto(QNetworkReply*)));

}
int insta_parser::count_images = 0;
insta_parser::~insta_parser()
{
    delete ui;
}

void insta_parser::get_user_name(const QByteArray &byte, instagram_account *account)
{
    int index = 0, s = -1, f = -1;
    QString username_start = "user?username=", username_finish = "\"";


    index = byte.indexOf(username_start, index);
    Q_ASSERT(index > 0);
    s = index = index + username_start.length();

    index = byte.indexOf(username_finish, index);
    Q_ASSERT(index > 0);
    f = index;
    index = index + username_finish.length();
    QString nickname = byte.mid(s, f - s);
    account->set_nickname(nickname);
    QDir dir("../my_project");
    dir.mkdir(account->get_nickname());
    current_nickname = account->get_nickname();
}

void insta_parser::get_count_likes(const QByteArray &byte, instagram_account *account,bool is_next_page)
{
    int count_likes = 0;
    int idx = 0, start = -1, finish = -1;
    QString RefStart;
    QString RefFinish;
    if(is_next_page)
    {
        RefStart = "\"edge_media_preview_like\":{\"count\":";
        RefFinish = ",";
    }
    else
    {
        RefStart = "\"edge_liked_by\":{\"count\":";
        RefFinish = "}";
    }
    QString likes;
    for (;;)
    {
        idx = byte.indexOf(RefStart, idx);
        if(idx == -1)break;
        start = idx = idx + RefStart.length();

        idx = byte.indexOf(RefFinish, idx);
        Q_ASSERT(idx > 0);
        finish = idx;
        idx = idx + RefFinish.length();
        likes = byte.mid(start, finish - start);
        count_likes+=likes.toInt();
     }
    account->set_count_likes(count_likes);
}

void insta_parser::get_count_comments(const QByteArray &byte, instagram_account *account,bool is_next_page)
{
    int count_comments = 0;
    int idx = 0, start = -1, finish = -1;
    QString RefStart = "\"edge_media_to_comment\":{\"count\":";
    QString RefFinish;
    if(is_next_page)
    {
        RefFinish = ",";
    }
    else
    {
        RefFinish = "}";
    }
    QString comments;
    for (;;)
    {
        idx = byte.indexOf(RefStart, idx);
        if(idx == -1)break;
        start = idx = idx + RefStart.length();

        idx = byte.indexOf(RefFinish, idx);
        Q_ASSERT(idx > 0);
        finish = idx;
        idx = idx + RefFinish.length();
        comments = byte.mid(start, finish - start);
        count_comments+=comments.toInt();
     }
    account->set_count_comments(count_comments);
}

void insta_parser::get_page_info(const QByteArray &byte, instagram_account *account)
{
    int followers = 0;
    int following = 0;
    int posts = 0;

    int idx = 0, start = -1, finish = -1;
    QString followers_start = "<meta content=\"";
    QString followers_finish = " млн";
    QString count_followers;

    QString following_start = ", ";
    QString following_finish = " ";

    QString posts_start = ", ";
    QString posts_finish = " ";

    //followers
    int temp_idx = idx;
    int temp_idx2 = idx;
    idx = byte.indexOf(followers_start, idx);
    start = idx = idx + followers_start.length();

    temp_idx = byte.indexOf(followers_finish, idx);
    if(temp_idx == -1)
    {
        followers_finish = " тис";
        temp_idx2 = byte.indexOf(followers_finish, idx);
        if(temp_idx2 == -1)
        {
            followers_finish = " ";
            idx = byte.indexOf(followers_finish, idx);
            Q_ASSERT(idx > 0);
            finish = idx;
            idx = idx + followers_finish.length();
            count_followers = byte.mid(start, finish - start);
            followers = count_followers.toInt();
            account->set_count_followers(followers);
        }
        else
        {
            finish = temp_idx2;
            idx = temp_idx2 + followers_finish.length();
            count_followers = byte.mid(start, finish - start);
            double fol = count_followers.toDouble();
            fol*=1000;
            followers = fol;
            account->set_count_followers(followers);
        }

    }
    else
    {
        finish = temp_idx;
        idx = temp_idx + followers_finish.length();
        count_followers = byte.mid(start, finish - start);
        double fol = count_followers.toDouble();
        fol*=1000000;
        followers = fol;
        account->set_count_followers(followers);
    }

    // writeStream << ref + '\n';

    //following
    idx = byte.indexOf(following_start, idx);
    Q_ASSERT(idx > 0);
    start = idx = idx + following_start.length();

    idx = byte.indexOf(following_finish, idx);
    Q_ASSERT(idx > 0);
    finish = idx;
    idx = idx + following_finish.length();
    bool is_bad_symbol = false;
    QString count_following = byte.mid(start, finish - start);
    QString correct_count_following;
    for(int i = 0; i < count_following.size();i++)
    {
        if(count_following[i] != ',')
            correct_count_following+=count_following[i];

    }
    following = correct_count_following.toInt();
    account->set_count_following(following);

    //posts
    idx = byte.indexOf(posts_start, idx);
    Q_ASSERT(idx > 0);
    start = idx = idx + posts_start.length();

    idx = byte.indexOf(posts_finish, idx);
    Q_ASSERT(idx > 0);
    finish = idx;
    idx = idx + posts_finish.length();
    QString count_posts = byte.mid(start, finish - start);
    QString correct_count_posts;
    for(int i = 0; i < count_posts.size();i++)
    {
        if(count_posts[i] != ',')
            correct_count_posts+=count_posts[i];

    }
    posts = correct_count_posts.toInt();
    account->set_count_posts(posts);
}

void insta_parser::get_biography(const QByteArray &byte, instagram_account *account)
{
    int index = 0, start = -1, finish = -1;
    QString biography_start = "{\"biography\":\"", biography_finish = "\"";


    index = byte.indexOf(biography_start, index);
    qDebug()<<index;
    Q_ASSERT(index > 0);
    start = index = index + biography_start.length();

    index = byte.indexOf(biography_finish, index);
    Q_ASSERT(index > 0);
    finish = index;
    index = index + biography_finish.length();
    QString biography = byte.mid(start, finish - start);
    account->set_biography(biography);
}

void insta_parser::get_id(const QByteArray &byte, instagram_account *account)
{
    int index = 0, start = -1, finish = -1;
    QString id_start = "\"owner\":{\"id\":\"", id_finish = "\"";


    index = byte.indexOf(id_start, index);
    qDebug()<<index;
    Q_ASSERT(index > 0);
    start = index = index + id_start.length();

    index = byte.indexOf(id_finish, index);
    Q_ASSERT(index > 0);
    finish = index;
    index = index + id_finish.length();
    QString id = byte.mid(start, finish - start);
    account->set_id(id);
}

QString insta_parser::get_next_page_url(const QByteArray &byte, instagram_account *account)
{
    int index = 0, start = -1, finish = -1;
    QString end_cursor_start = "\"has_next_page\":true,\"end_cursor\":\"", end_cursor_finish = "==\"";


    index = byte.indexOf(end_cursor_start, index);
    if(index!=-1)
    {
        Q_ASSERT(index > 0);
        start = index = index + end_cursor_start.length();

        index = byte.indexOf(end_cursor_finish, index);
        Q_ASSERT(index > 0);
        finish = index;
        index = index + end_cursor_finish.length();
        QString end_cursor = byte.mid(start, finish - start);
        QString url_of_next_page = next_page_url_start + account->get_id() + after_id + end_cursor + after_end_cursor_url;
        return url_of_next_page;
    }
    else
    {
        return "";
    }

}

void insta_parser::send_request_to_next_page(QString next_page_URL)
{
    //qDebug()<<next_page_URL;
    QNetworkAccessManager*network_manager_next_page = new QNetworkAccessManager;
    connect(network_manager_next_page, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinishedNextPage(QNetworkReply*)));
    QNetworkRequest*request_next_page = new QNetworkRequest;
    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    request_next_page->setSslConfiguration(sslConfiguration);
    request_next_page->setUrl(QUrl(next_page_URL));
    request_next_page->setRawHeader(QByteArray("Content-Type"), QByteArray("application/json"));
    request_next_page->setRawHeader(QByteArray("Accept"), QByteArray("*/*"));
    request_next_page->setRawHeader(QByteArray("Accept-Language"), QByteArray("uk-UA,uk;q=0.9,ru;q=0.8,en-US;q=0.7,en;q=0.6"));
    request_next_page->setRawHeader(QByteArray("Cookie"), QByteArray("mcd=3; mid=W9xLywALAAGg7OoEbY0nfoKohew7; csrftoken=3aGyihYEWBtwmIRi4N2ehWDgcAs6GK4e; ds_user_id=4749051444; rur=ATN; shbid=247; shbts=1543613568.5991576; sessionid=IGSC40734e174da7dfed5472b3c86e9c833c4750945968c8bf3a7bb3d9aba212270f%3AlmJZSseKFTWZfSJqMHKD0GRN9kK0umgT%3A%7B%22_auth_user_id%22%3A4749051444%2C%22_auth_user_backend%22%3A%22accounts.backends.CaseInsensitiveModelBackend%22%2C%22_auth_user_hash%22%3A%22%22%2C%22_platform%22%3A4%2C%22_token_ver%22%3A2%2C%22_token%22%3A%224749051444%3AtrRse8wH1gMOIeKxKSysHDkjA1aK2wPX%3Aaa69cd1831af31c4a01c2a1c24131d640fdce8d95dc37b264622b90d5950b081%22%2C%22last_refreshed%22%3A1543613568.6006875038%7D; urlgen=\"{\"91.202.131.116\": 13032\054 \"91.202.130.116\": 13032\054 \"91.202.128.1\": 13032}:1gT6b5:WQwapvKaDEaXzCJaa1_W1oLZDH0\""));
    //request_next_page->setRawHeader(QByteArray("Refer"), QByteArray("https://www.instagram.com/ivannazavadetska/"));
    //request_next_page->setRawHeader(QByteArray("x-instagram-gis"), QByteArray("63fc162934f563ce68356277c11aae5c"));
    request_next_page->setRawHeader(QByteArray("x-requested-with"), QByteArray("XMLHttpRequest"));
    request_next_page->setRawHeader(QByteArray("User-Agent"), QByteArray("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.110 Safari/537.36"));

    network_manager_next_page->get(*request_next_page);
}

void insta_parser::download_photos(const QByteArray &byte, instagram_account *account)
{
    int idx = 0, start = -1, finish = -1;
    QString RefStart = "\"username\":\"" + account->get_nickname() +"\"},\"thumbnail_src\":\"";
    QString RefFinish = "\"";

    QFile out("../my_project/" + account->get_nickname() + "/" + "PhotoReferences.txt");
    out.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream writeStream(&out);

    for (;;)
    {
        idx = byte.indexOf(RefStart, idx);
        if(idx == -1)break;
        start = idx = idx + RefStart.length();

        idx = byte.indexOf(RefFinish, idx);
        Q_ASSERT(idx > 0);
        finish = idx;
        idx = idx + RefFinish.length();
        QString ref = byte.mid(start, finish - start);

        writeStream << ref + '\n';

        save_photo(ref);
     }
    out.close();
}

void insta_parser::download_photos_on_the_next_page(const QByteArray &byte, instagram_account *account)
{
    int idx = 0, start = -1, finish = -1;
    QString RefStart = "\"thumbnail_src\":\"";
    QString RefFinish = "\"";
    QFile out("../my_project/" + account->get_nickname() + "/" + "PhotoReferences.txt");
    out.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream writeStream(&out);

    for (;;)
    {
        idx = byte.indexOf(RefStart, idx);
        if(idx == -1)break;
        start = idx = idx + RefStart.length();

        idx = byte.indexOf(RefFinish, idx);
        Q_ASSERT(idx > 0);
        finish = idx;
        idx = idx + RefFinish.length();
        QString ref = byte.mid(start, finish - start);

        writeStream << ref + '\n';

        save_photo(ref);
     }
    out.close();
}

void insta_parser::on_get_info_button_clicked()
{
    QString URL;
    if(ui->comboBox->currentIndex() == 0)
    {
        URL += "https://www.instagram.com/"+ui->reference->text()+"/";
    }
    else
    {
        URL = ui->reference->text();
    }
    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setSslConfiguration(sslConfiguration);
    request.setUrl(QUrl(URL));
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("Content-Type", "text/html");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
    network_manager->get(request);
}

void insta_parser::replyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        //qDebug()<<"reply finished";
        ui->status->setText("");

        QFile fileOut("fileout.txt");
        fileOut.open(QIODevice::WriteOnly);
        QByteArray byte = reply->readAll();

        fileOut.write(byte);
        fileOut.close(); // Закрываем fileout.txt

        //parsing
        instagram_account*account = new instagram_account;
        current_account = account;
        accounts.push_back(account);

        get_user_name(byte,account);

        QFile out("../my_project/Accounts.txt");
        out.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
        QTextStream writeStream(&out);
        writeStream<< account->get_nickname()  +'\n';
        out.close();

        download_photos(byte,account);
        get_page_info(byte,account);

        get_count_likes(byte,account,false);
        get_count_comments(byte,account,false);
        get_biography(byte,account);
        get_id(byte,account);

        QString next_page_url  = get_next_page_url(byte,account);

        if(next_page_url !="")
        {
          send_request_to_next_page(next_page_url);
        }

        QListWidgetItem *item = new QListWidgetItem;
        item->setText(account->get_nickname());
        ui->accounts_list->addItem(item);
    }
    else
    {
        ui->status_line->setText("Error");
    }

}

void insta_parser::replyFinishedPhoto(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray data = reply->readAll();
            QImage image = QImage::fromData(data);
            //ui->photo->setPixmap(QPixmap::fromImage(image));
            //ui->photo->setMinimumHeight(image.height());
            image.save("../my_project/" + current_nickname + "/" + "image" + QString::number(++count_images) + ".jpg");
        }
}

void insta_parser::save_photo(QString &ref)
{
    //QNetworkAccessManager*manager = new QNetworkAccessManager;
    //QNetworkRequest*req = new QNetworkRequest;
    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    request_photo.setSslConfiguration(sslConfiguration);
    request_photo.setUrl(QUrl(ref));
    manager_photo->get(request_photo);
}

void insta_parser::on_show_info_button_clicked()
{
    if(!accounts.empty())
    {
        QList<QListWidgetItem *> items = ui->accounts_list->selectedItems();
        if(!items.isEmpty())
        {
            int current = ui->accounts_list->currentRow();
            ui->lbl_name->setText("Nickname : " + accounts[current]->get_nickname());
            ui->biography_text->setText("Biography : " + accounts[current]->get_biography());
            ui->lbl_followers->setText("Followers : " + QString::number(accounts[current]->get_count_followers()));
            ui->lbl_following->setText("Following : " + QString::number(accounts[current]->get_count_following()));
            ui->lbl_posts->setText("Count Posts : " + QString::number(accounts[current]->get_count_posts()));
            ui->lbl_likes->setText("Count Likes : " + QString::number(accounts[current]->get_count_likes()));
            ui->lbl_comments->setText("Count Comments : " + QString::number(accounts[current]->get_count_comments()));
        }
    }

}

void insta_parser::replyFinishedNextPage(QNetworkReply *reply_next_page)
{
    if(reply_next_page->error())
    {
        qDebug() << "ERROR";
        qDebug() << reply_next_page->errorString();
    }
    else
    {
        QByteArray byte = reply_next_page->readAll();
        get_count_comments(byte,current_account,true);
        get_count_likes(byte,current_account,true);

        download_photos_on_the_next_page(byte,current_account);
        QString next_page_url  = get_next_page_url(byte,current_account);
        reply_next_page->deleteLater();
        if(next_page_url !="")
        {
            send_request_to_next_page(next_page_url);
            ui->status->setText("Finished");
            write_in_file_account_info("Nickname : ",current_account->get_nickname(),current_account->get_nickname());
            write_in_file_account_info("Count followers : ",QString::number(current_account->get_count_followers()),current_account->get_nickname());
            write_in_file_account_info("Count following : ",QString::number(current_account->get_count_following()),current_account->get_nickname());
            write_in_file_account_info("Count posts : ",QString::number(current_account->get_count_posts()),current_account->get_nickname());
            write_in_file_account_info("Count comments : ",QString::number(current_account->get_count_comments()),current_account->get_nickname());
            write_in_file_account_info("Count comments : ",QString::number(current_account->get_count_likes()),current_account->get_nickname());
            if(current_account->get_biography() == "")
            {
                write_in_file_account_info("Biography : ","none",current_account->get_nickname());
            }
            else
            {
                write_in_file_account_info("Biography : ",current_account->get_biography(),current_account->get_nickname());
            }
            write_in_file_account_info("Account id : ",current_account->get_id(),current_account->get_nickname());
        }
    }

}

void insta_parser::write_in_file_account_info(QString phrase, QString value, QString nickname)
{
    QFile in("../my_project/" + nickname + "/" + "AccountInfo(for_user).txt");
    in.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream writeStream(&in);
    writeStream<< phrase <<value  +'\n';
    in.close();

    QFile in2("../my_project/" + nickname + "/" + "AccountInfo(for_programmer).txt");
    in2.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream writeStream2(&in2);
    writeStream2<<value +'\n';
    in2.close();
}

void insta_parser::check_existing_accounts()
{
    QFile in("../my_project/Accounts.txt");
    if(in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream writeStream(&in);
        QString account_nickname;
        while(!writeStream.atEnd())
        {
            writeStream>>account_nickname;
            if(account_nickname !="")
            {
                instagram_account*account = new instagram_account;
                if(fill_acoount_info(account,"../my_project/"+account_nickname+ "/AccountInfo(for_programmer).txt") == 1)
                {
                    accounts.push_back(account);
                    QListWidgetItem *item = new QListWidgetItem;
                    item->setText(account->get_nickname());
                    ui->accounts_list->addItem(item);
                }
            }
        }
        in.close();
    }
    else
    {
        ui->status_line->setText("Cannot find file Accounts.txt");
    }
}

void insta_parser::swap_list_items(int index1, int index2)
{
    QListWidgetItem *item1 = ui->accounts_list->item(index1);
    QListWidgetItem *item2 = ui->accounts_list->item(index2);
    QString tmp;
    tmp = item1->text();
    item1->setText(item2->text());
    item2->setText(tmp);
}

bool insta_parser::fill_acoount_info(instagram_account *account, QString path)
{
    QString info;
    QFile in(path);
    if(in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream writeStream(&in);
        writeStream>>info;
        account->set_nickname(info);
        writeStream>>info;
        account->set_count_followers(info.toInt());
        writeStream>>info;
        account->set_count_following(info.toInt());
        writeStream>>info;
        account->set_count_posts(info.toInt());
        writeStream>>info;
        account->set_count_likes(info.toInt());
        writeStream>>info;
        account->set_count_comments(info.toInt());
        writeStream>>info;
        account->set_biography(info);
        writeStream>>info;
        account->set_id(info);
        return 1;
    }
    else
    {
        return 0;
    }
}

void insta_parser::on_sort_button_clicked()
{
    int size = ui->accounts_list->count();
    if(ui->sort_type->currentText() == "Followers")
    {
        for(int i = 0; i < size ;i++)
        {
            for(int j = 1; j < size-i;j++)
            {
                if(accounts.at(j)->get_count_followers() > accounts.at(j-1)->get_count_followers())
                {
                    std::swap(accounts[j],accounts[j-1]);
                    swap_list_items(j,j-1);
                }
            }
        }
    }
    else if(ui->sort_type->currentText() == "Following")
    {
        for(int i = 0; i < size ;i++)
        {
            for(int j = 1; j < size-i;j++)
            {
                if(accounts.at(j)->get_count_following() > accounts.at(j-1)->get_count_following())
                {
                    std::swap(accounts[j],accounts[j-1]);
                    swap_list_items(j,j-1);
                }
            }
        }
    }
    else if(ui->sort_type->currentText() == "Posts")
    {
        for(int i = 0; i < size ;i++)
        {
            for(int j = 1; j < size-i;j++)
            {
                if(accounts.at(j)->get_count_posts() > accounts.at(j-1)->get_count_posts())
                {
                    std::swap(accounts[j],accounts[j-1]);
                    swap_list_items(j,j-1);
                }
            }
        }
    }
    else if(ui->sort_type->currentText() == "Likes")
    {
        for(int i = 0; i < size ;i++)
        {
            for(int j = 1; j < size-i;j++)
            {
                if(accounts.at(j)->get_count_likes() > accounts.at(j-1)->get_count_likes())
                {
                    std::swap(accounts[j],accounts[j-1]);
                    swap_list_items(j,j-1);
                }
            }
        }
    }
    else
    {
        for(int i = 0; i < size ;i++)
        {
            for(int j = 1; j < size-i;j++)
            {
                if(accounts.at(j)->get_count_comments() > accounts.at(j-1)->get_count_comments())
                {
                    std::swap(accounts[j],accounts[j-1]);
                    swap_list_items(j,j-1);
                }
            }
        }
    }

}

void insta_parser::on_delete_account_button_clicked()
{
    if(!accounts.empty())
    {
        QList<QListWidgetItem *> items = ui->accounts_list->selectedItems();
        if(!items.isEmpty())
        {
            QString nickname;
            //remove frome list
            int index = ui->accounts_list->currentRow();
            delete ui->accounts_list->currentItem();
            instagram_account*account = accounts[index];
            nickname = accounts[index]->get_nickname();
            delete account;
            //remove from vector
            auto it = accounts.begin();
            it = it+index;
            accounts.erase(it);
            //remove from file
            QFile in("../my_project/Accounts.txt");
            QFile out("../my_project/AccountsTemp.txt");
            out.open(QIODevice::WriteOnly | QIODevice::Text);
            if(in.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QString current_nick;
                QTextStream writeStreamIn(&in);
                QTextStream writeStreamOut(&out);
                while(!writeStreamIn.atEnd())
                {
                    writeStreamIn>>current_nick;
                    if(current_nick!=nickname && current_nick!="")
                    {
                        writeStreamOut << current_nick + '\n';
                    }
                }
                in.close();
                out.close();
                remove("../my_project/Accounts.txt");
                rename("../my_project/AccountsTemp.txt","../my_project/Accounts.txt");
            }
            else
            {
                ui->status_line->setText("Unexpected error");
            }
        }
    }
}
