#ifndef INSTA_PARSER_H
#define INSTA_PARSER_H

#include <QDialog>
#include <QSslError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QBitArray>
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
private slots:
    void on_get_info_button_clicked();
    void replyFinished(QNetworkReply*reply);

private:
    Ui::insta_parser *ui;
    static int count_images;
    QNetworkReply *reply;
    QNetworkRequest request;
    QNetworkAccessManager *network_manager;

    QNetworkReply *reply_photo;
    QNetworkRequest request_photo;
    QNetworkAccessManager *manager_photo;

    vector<instagram_account*> accounts;
    void save_photo(QString &ref);
};

#endif // INSTA_PARSER_H
