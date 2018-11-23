#ifndef PHOTO_EDIT_H
#define PHOTO_EDIT_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "paintscene.h"
#include "photo_paint.h"
#include "photo.h"
namespace Ui {
class photo_edit;
}

class photo_edit : public QMainWindow
{
    Q_OBJECT

public:
    explicit photo_edit(QWidget *parent = nullptr);
    ~photo_edit();
    int get_count_of_changed_images();

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

    void on_reset_button_clicked();

    void on_scale_button_clicked();

    void on_screen_button_clicked();

    void on_draw_smile_button_clicked();

    void on_mirror_button_clicked();

    void on_paint_button_clicked();

    void on_open_by_url_button_clicked();

signals:
    void paint_object(QImage *image);
private slots:
    void replyFinishedPhoto(QNetworkReply*);
private:
    Ui::photo_edit *ui;
    int count_of_changed_images = 1;
    int current_gray_level = 0;
    QImage *original_img = nullptr;
    QImage *changed_img = nullptr;
    photo current_photo;
    QGraphicsScene *graphic_scene;
    QSize image_size;
    QString fileName;

    QImage mirrored_image;

    paintScene *scene;
    photo_paint *paint_window = nullptr;


    QNetworkReply *reply_photo;
    QNetworkRequest request_photo;
    QNetworkAccessManager *manager_photo;
};

#endif // PHOTO_EDIT_H
