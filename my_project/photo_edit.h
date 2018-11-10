#ifndef PHOTO_EDIT_H
#define PHOTO_EDIT_H

#include <QMainWindow>
#include<QGraphicsScene>
namespace Ui {
class photo_edit;
}

class photo_edit : public QMainWindow
{
    Q_OBJECT

public:
    explicit photo_edit(QWidget *parent = nullptr);
    ~photo_edit();

private slots:
    void on_grayscale_button_clicked();

    void on_negative_button_clicked();

    void on_sepia_button_clicked();

    void on_swap_colors_button_clicked();

    void on_add_text_button_clicked();

    void on_to_ascii_button_clicked();

    void on_save_btn_clicked();

    void on_noise_btn_clicked();

    void on_gray_level_3_valueChanged(int value);

    void on_open_photo_button_clicked();

    void on_deep_sepia_button_clicked();

    void on_transparency_button_clicked();

private:
    Ui::photo_edit *ui;
    int count_of_changed_images = 0;
    int current_gray_level = 0;
    QImage *original_img;
    QImage *changed_img;
    QGraphicsScene *graphic_scene;
    QSize image_size;
    QString fileName;
};

#endif // PHOTO_EDIT_H
