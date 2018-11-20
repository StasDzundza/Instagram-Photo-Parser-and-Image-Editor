#include "insta_parser.h"
#include "ui_insta_parser.h"

insta_parser::insta_parser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::insta_parser)
{
    ui->setupUi(this);
}

insta_parser::~insta_parser()
{
    delete ui;
}

void insta_parser::on_get_info_button_clicked()
{

}
