#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"photo_edit.h"
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

private:
    Ui::MainWindow *ui;
    photo_edit*photo_edit_window = nullptr;
};

#endif // MAINWINDOW_H
