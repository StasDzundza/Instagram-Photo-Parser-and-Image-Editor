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
    void download_photos_on_the_next_page(const QByteArray &byte, instagram_account *account);
    void get_user_name(const QByteArray&byte,instagram_account*account);
    void get_count_likes(const QByteArray&byte,instagram_account*account);
    void get_count_comments(const QByteArray&byte,instagram_account*account);
    void get_page_info(const QByteArray&byte,instagram_account*account);
    void get_biography(const QByteArray&byte,instagram_account*account);
    void get_id(const QByteArray&byte,instagram_account*account);
    QString get_next_page_url(const QByteArray&byte,instagram_account*account);
    void send_request_to_next_page(QString next_page_URL);

private slots:
    void on_get_info_button_clicked();
    void replyFinished(QNetworkReply*reply);
    void replyFinishedPhoto(QNetworkReply*);
    void on_show_info_button_clicked();
    void replyFinishedNextPage(QNetworkReply*);

private:
    Ui::insta_parser *ui;
    void write_in_file_account_info(QString phrase,QString value,QString nickname);
    static int count_images;
    QNetworkReply *reply;
    QNetworkRequest request;
    QNetworkAccessManager *network_manager;

    QNetworkRequest request_next_page;
    QNetworkAccessManager *network_manager_next_page;

    QNetworkReply *reply_photo;
    QNetworkRequest request_photo;
    QNetworkAccessManager *manager_photo;

    QString current_nickname;
    instagram_account*current_account = nullptr;
    vector<instagram_account*> accounts;
    void save_photo(QString &ref);

   QString next_page_url_start = "https://www.instagram.com/graphql/query/?query_hash=50d3631032cf38ebe1a2d758524e3492&variables=%7B%22id%22%3A%22";
   QString after_id = "%22%2C%22first%22%3A12%2C%22after%22%3A%22";
   QString after_end_cursor_url = "%3D%3D%22%7D";
};

#endif // INSTA_PARSER_H
