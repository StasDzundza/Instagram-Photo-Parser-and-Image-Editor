#include "insta_parser.h"
#include "ui_insta_parser.h"
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
    qDebug()<<account->get_nickname();
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

void insta_parser::get_count_subscribers(const QByteArray &byte, instagram_account *account)
{

}

void insta_parser::get_count_subscribed_on(const QByteArray &byte, instagram_account *account)
{

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

    get_user_name(byte,account);
    download_photos(byte,account);


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
