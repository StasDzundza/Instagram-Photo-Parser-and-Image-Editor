#ifndef INSTA_PARSER_H
#define INSTA_PARSER_H

#include <QDialog>
#include <QSslError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QByteArray>
#include <QString>
#include "instagram_account.h"

namespace Ui {
class insta_parser;
}

class insta_parser : public QDialog
{
    Q_OBJECT

public:
    explicit insta_parser(QWidget *parent = nullptr);
    ~insta_parser();
    void download_photos(const QByteArray&byte, instagram_account *account);
    void get_user_name(const QByteArray&byte,instagram_account*account);
    void get_count_likes(const QByteArray&byte,instagram_account*account);
    void get_count_comments(const QByteArray&byte,instagram_account*account);
    void get_count_subscribers(const QByteArray&byte,instagram_account*account);
    void get_count_subscribed_on(const QByteArray&byte,instagram_account*account);

private slots:
    void on_get_info_button_clicked();
    void replyFinished(QNetworkReply*reply);
    void replyFinishedPhoto(QNetworkReply*);
private:
    Ui::insta_parser *ui;
    static int count_images;
    QNetworkReply *reply;
    QNetworkRequest request;
    QNetworkAccessManager *network_manager;

    QNetworkReply *reply_photo;
    QNetworkRequest request_photo;
    QNetworkAccessManager *manager_photo;

    QString current_nickname;
    vector<instagram_account*> accounts;
    void save_photo(QString &ref);
};

#endif // INSTA_PARSER_H
