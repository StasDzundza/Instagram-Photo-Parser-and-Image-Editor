#ifndef INSTA_PARSER_H
#define INSTA_PARSER_H

#include <QDialog>

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

private:
    Ui::insta_parser *ui;
};

#endif // INSTA_PARSER_H
