#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_edit_photo_button_clicked()
{
    photo_edit_window = new photo_edit(this);
    photo_edit_window->show();
}

void MainWindow::on_instagram_info_button_clicked()
{
    insta_parser_window = new insta_parser(this);
    insta_parser_window->show();
}

