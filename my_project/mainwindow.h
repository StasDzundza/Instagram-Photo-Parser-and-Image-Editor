#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "photo_edit.h"
#include "insta_parser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_edit_photo_button_clicked();

    void on_instagram_info_button_clicked();

private:
    Ui::MainWindow *ui;
    photo_edit*photo_edit_window = nullptr;
    insta_parser*insta_parser_window = nullptr;
};

#endif // MAINWINDOW_H
