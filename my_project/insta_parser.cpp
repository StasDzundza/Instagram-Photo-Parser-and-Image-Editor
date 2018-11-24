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
    network_manager_next_page = new QNetworkAccessManager(this);
    network_manager->setStrictTransportSecurityEnabled(true);
    network_manager->setStrictTransportSecurityEnabled(true);

    connect(network_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
    connect(network_manager_next_page, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinishedNextPage(QNetworkReply*)));

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
    write_in_file_account_info("Nickname : ",nickname,nickname);
}

void insta_parser::get_count_likes(const QByteArray &byte, instagram_account *account)
{
    int count_likes = 0;
    int idx = 0, start = -1, finish = -1;
    QString RefStart = "\"edge_liked_by\":{\"count\":";
    QString RefFinish = "}";
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

    write_in_file_account_info("Count likes : ",QString::number(count_likes),account->get_nickname());
}

void insta_parser::get_count_comments(const QByteArray &byte, instagram_account *account)
{
    int count_comments = 0;
    int idx = 0, start = -1, finish = -1;
    QString RefStart = "\"edge_media_to_comment\":{\"count\":";
    QString RefFinish = "}";
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
    write_in_file_account_info("Count comments : ",QString::number(count_comments),account->get_nickname());
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
    write_in_file_account_info("Count followers : ",count_followers,account->get_nickname());
    write_in_file_account_info("Count following : ",correct_count_following,account->get_nickname());
    write_in_file_account_info("Count posts : ",correct_count_posts,account->get_nickname());

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
    if(biography == "")
    {
        write_in_file_account_info("Biography : ","none",account->get_nickname());
    }
    else
    {
        write_in_file_account_info("Biography : ",biography,account->get_nickname());
    }
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
    write_in_file_account_info("Account id : ",id,account->get_nickname());
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
    qDebug()<<next_page_URL;
    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    request_next_page.setSslConfiguration(sslConfiguration);
    request_next_page.setUrl(QUrl(next_page_URL));
    network_manager_next_page->get(request_next_page);
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
    network_manager->get(request);
}

void insta_parser::replyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        qDebug()<<"reply finished";
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
        out.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
        QTextStream writeStream2(&out);
        writeStream2<<"Information about only first twelve photos\n";
        out.close();
        get_count_likes(byte,account);
        get_count_comments(byte,account);
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

        ui->status->setText("Finished");
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
    QByteArray byte = reply_next_page->readAll();
    qDebug()<<byte;
    //get_count_comments(byte,current_account);
    //get_count_likes(byte,current_account);
    download_photos_on_the_next_page(byte,current_account);
    QString next_page_url  = get_next_page_url(byte,current_account);
    if(next_page_url !="")
    {
        send_request_to_next_page(next_page_url);
    }
    reply_next_page->deleteLater();
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
