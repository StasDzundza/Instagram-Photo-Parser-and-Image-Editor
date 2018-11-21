#include "insta_parser.h"
#include "ui_insta_parser.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QDir>

insta_parser::insta_parser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::insta_parser)
{
    ui->setupUi(this);
    network_manager = new QNetworkAccessManager(this);
    network_manager->setStrictTransportSecurityEnabled(true);
    connect(network_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));

    manager_photo = new QNetworkAccessManager(this);
    manager_photo->setStrictTransportSecurityEnabled(true);
    connect(manager_photo, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinishedPhoto(QNetworkReply*)));

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
    account->set_nickname(byte.mid(s, f - s));
}

void insta_parser::get_count_likes(const QByteArray &byte, instagram_account *account)
{
    int count_likes = 0;
    int idx = 0, start = -1, finish = -1;
    QString RefStart = "\"edge_liked_by\":{\"count\":";
    QString RefFinish = "}";
    for (;;)
    {
        idx = byte.indexOf(RefStart, idx);
        if(idx == -1)break;
        start = idx = idx + RefStart.length();

        idx = byte.indexOf(RefFinish, idx);
        Q_ASSERT(idx > 0);
        finish = idx;
        idx = idx + RefFinish.length();
        QString likes = byte.mid(start, finish - start);
        count_likes+=likes.toInt();
       // writeStream << ref + '\n';
     }
    account->set_count_likes(count_likes);
}

void insta_parser::get_count_comments(const QByteArray &byte, instagram_account *account)
{
    int count_comments = 0;
    int idx = 0, start = -1, finish = -1;
    QString RefStart = "\"edge_media_to_comment\":{\"count\":";
    QString RefFinish = "}";
    for (;;)
    {
        idx = byte.indexOf(RefStart, idx);
        if(idx == -1)break;
        start = idx = idx + RefStart.length();

        idx = byte.indexOf(RefFinish, idx);
        Q_ASSERT(idx > 0);
        finish = idx;
        idx = idx + RefFinish.length();
        QString comments = byte.mid(start, finish - start);
        count_comments+=comments.toInt();
       // writeStream << ref + '\n';
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
            finish = idx;
            idx = idx + followers_finish.length();
            QString count_followers = byte.mid(start, finish - start);
            followers = count_followers.toInt();
            account->set_count_followers(followers);
        }
        else
        {
            finish = temp_idx2;
            idx = temp_idx2 + followers_finish.length();
            QString count_followers = byte.mid(start, finish - start);
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
        QString count_followers = byte.mid(start, finish - start);
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
    QString biography_start = "\"user\":{\"biography\":\"", biography_finish = "\"";


    index = byte.indexOf(biography_start, index);
    Q_ASSERT(index > 0);
    start = index = index + biography_start.length();

    index = byte.indexOf(biography_finish, index);
    Q_ASSERT(index > 0);
    finish = index;
    index = index + biography_finish.length();
    account->set_biography(byte.mid(start, finish - start));
}


void insta_parser::download_photos(const QByteArray &byte, instagram_account *account)
{
    int idx = 0, start = -1, finish = -1;
    QString RefStart = "\"username\":\"" + account->get_nickname() +"\"},\"thumbnail_src\":\"";
    QString RefFinish = "\"";

    QDir dir("../my_project");
    dir.mkdir(account->get_nickname());
    current_nickname = account->get_nickname();
    QFile out("../my_project/" + account->get_nickname() + "/" + "AccountInfo.txt");
    out.open(QIODevice::WriteOnly | QIODevice::Text);
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
    //request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
    network_manager->get(request);
}

void insta_parser::replyFinished(QNetworkReply *reply)
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
    accounts.push_back(account);

    get_user_name(byte,account);
    download_photos(byte,account);
    get_count_likes(byte,account);
    get_count_comments(byte,account);
    get_page_info(byte,account);

    QListWidgetItem *item = new QListWidgetItem;
    item->setText(account->get_nickname());
    ui->accounts_list->addItem(item);

    qDebug()<<"finished";
    ui->status->setText("Finished");

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
            ui->lbl_biography->setText("Biography : " + accounts[current]->get_biography());
            ui->lbl_followers->setText("Followers : " + QString::number(accounts[current]->get_count_followers()));
            ui->lbl_following->setText("Following : " + QString::number(accounts[current]->get_count_following()));
            ui->lbl_posts->setText("Count Posts : " + QString::number(accounts[current]->get_count_posts()));
            ui->lbl_likes->setText("Count Likes : " + QString::number(accounts[current]->get_count_likes()));
            ui->lbl_comments->setText("Count Comments : " + QString::number(accounts[current]->get_count_comments()));
        }
    }

}
