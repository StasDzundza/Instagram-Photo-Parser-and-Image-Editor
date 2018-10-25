#include "photo_edit.h"
#include "ui_photo_edit.h"
#include<QLabel>
#include<QDebug>
#include<QPainter>
#include<QRgb>
#include<QDir>
#include<QFileDialog>
#include<random>
#include<vector>
#include<QTextStream>
photo_edit::photo_edit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::photo_edit)
{
    ui->setupUi(this);
    QFileDialog dialog(this);
      //dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
      //dialog.setViewMode(QFileDialog::Detail);
     fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      "C:/",
                                                      tr("Images (*.png *.xpm *.jpg)"));
    //img = new QImage(":/res/lena_color.jpg");
     original_img = new QImage(fileName);
    QPixmap pix(QPixmap::fromImage(*original_img));
    int w = pix.width();
    int h = pix.height();
    image_size = original_img->size();
    ui->image->resize(w,h);
    ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    changed_img = new QImage(fileName);
    //якщо б ми мали графік сцену
     /*image_size = original_img->size();
     graphic_scene = new QGraphicsScene();
     graphic_scene->addPixmap(QPixmap::fromImage(*original_img));
     ui->graphicsView->resize(image_size);
     ui->graphicsView->setScene(graphic_scene);*/

ui->gray_level_3->setMaximum(7);
}

photo_edit::~photo_edit()
{
    delete ui;
}


void photo_edit::on_grayscale_button_clicked()
{
    for(int i = 0; i < image_size.height();i++)
    {
        for(int j = 0; j < image_size.width();j++)
        {
            int rgb_sum = 0;
            QColor clrCurrent( changed_img->pixel( j, i ) );
            rgb_sum = rgb_sum+clrCurrent.red()+clrCurrent.green()+clrCurrent.blue();
            rgb_sum/=3;

            changed_img->setPixel(j,i,qRgb(rgb_sum,rgb_sum,rgb_sum));
        }
    }
    QPixmap pix(QPixmap::fromImage(*changed_img));
    int w = pix.width();
    int h = pix.height();
    image_size = pix.size();
    ui->image->resize(w,h);
    ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
}

void photo_edit::on_negative_button_clicked()
{
    for(int i = 0; i < image_size.height();i++)
    {
        for(int j = 0; j < image_size.width();j++)
        {
            QColor clrCurrent( changed_img->pixel( j, i ) );
            changed_img->setPixel(j,i,qRgb(255-clrCurrent.red(),255-clrCurrent.green(),255-clrCurrent.blue()));
        }
    }
    QPixmap pix(QPixmap::fromImage(*changed_img));
    int w = pix.width();
    int h = pix.height();
    image_size = pix.size();
    ui->image->resize(w,h);
    ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
}

void photo_edit::on_sepia_button_clicked()
{
    for(int i = 0; i < image_size.height();i++)
    {
        for(int j = 0; j < image_size.width();j++)
        {
            double rgb_sum = 0;
            QColor clrCurrent( changed_img->pixel( j, i ) );
            rgb_sum = 0.3*clrCurrent.red()+0.59*clrCurrent.green()+0.11*clrCurrent.blue();
            changed_img->setPixel(j,i,qRgb(rgb_sum+100,rgb_sum+50,rgb_sum));
        }
    }
    QPixmap pix(QPixmap::fromImage(*changed_img));
    int w = pix.width();
    int h = pix.height();
    image_size = pix.size();
    ui->image->resize(w,h);
    ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
}

void photo_edit::on_swap_colors_button_clicked()
{

}
