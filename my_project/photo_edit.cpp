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
    original_img = nullptr;
    changed_img = nullptr;


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
    for(int i = 0; i < image_size.height();i++)
    {
        for(int j = 0; j < image_size.width();j++)
        {
            QColor clrCurrent( changed_img->pixel( j, i ) );
            changed_img->setPixel(j,i,qRgb(clrCurrent.green(),clrCurrent.blue(),clrCurrent.red()));
        }
    }
    QPixmap pix(QPixmap::fromImage(*changed_img));
    int w = pix.width();
    int h = pix.height();
    image_size = pix.size();
    ui->image->resize(w,h);
    ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
}

void photo_edit::on_add_text_button_clicked()
{
    QPainter painter;
    painter.begin(changed_img);
    painter.setFont(QFont("Tahoma", 12, QFont::Bold));
    QPen pen;
    QColor c;
    pen.setColor(c);
    painter.setPen(pen);
    painter.drawText(changed_img->rect(), Qt::AlignCenter, QString("Text on photo"));
    painter.end();

    QPixmap pix(QPixmap::fromImage(*changed_img));
    int w = pix.width();
    int h = pix.height();
    image_size = pix.size();
    ui->image->resize(w,h);
    ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
}

void photo_edit::on_to_ascii_button_clicked()
{
    int min = 255,max =0;
    for(int i = 0; i < image_size.height();i++)
    {
        for(int j = 0; j < image_size.width();j++)
        {
            int rgb_sum = 0;
            QColor clrCurrent( changed_img->pixel( j, i ) );
            rgb_sum = rgb_sum+clrCurrent.red()+clrCurrent.green()+clrCurrent.blue();
            rgb_sum/=3;
            if(rgb_sum > max)
                max = rgb_sum;
            if(rgb_sum < min)
                min = rgb_sum;

            changed_img->setPixel(j,i,qRgb(rgb_sum,rgb_sum,rgb_sum));
        }
    }
    char arr[] = {' ','.',',',':',';','o','x','%','#','@'};
    int size_arr = sizeof(arr)/sizeof(char);
   int div = (max-min)/size_arr;

    QFile myFile("ascii2.txt");
    myFile.open(QFile::WriteOnly|QFile::Text);
    QTextStream write(&myFile);
    for(int i = 0; i < image_size.height();i++)
    {
        for(int j = 0; j < image_size.width();j++)
        {
            int rgb_sum = 0;
            QColor clrCurrent( changed_img->pixel( j,i) );
            int index = (clrCurrent.red()-min)/div;
            write<<arr[index];

        }
        write<<endl;
    }
}

void photo_edit::on_save_btn_clicked()
{
    count_of_changed_images++;
    QString filename_save = "photo_number" + QString::number(count_of_changed_images)+".jpg";
    changed_img->save(filename_save);
}

void photo_edit::on_noise_btn_clicked()
{
    for(int i = 0; i < image_size.height();i++)
    {
        for(int j = 0; j < image_size.width();j++)
        {
            double rand_value = (double)(0.5-(rand()%100/100.0))*ui->noise_level->text().toDouble();
            QColor clrCurrent( changed_img->pixel( j, i ) );
            int r,g,b;
            //if(clrCurrent.red() > 255-rand_value)
            //r = 255;
            //else
            r = clrCurrent.red()+rand_value;
            while(r>255)r-=256;
            while(r<0)r+=256;
            //if(clrCurrent.green() > 255-rand_value)
            //g = 255;
            //else
            g = clrCurrent.green()+rand_value;
            while(g>255)g-=256;
            while(g<0)g+=256;
            //if(clrCurrent.blue() > 255-rand_value)
            //b = 255;
            //else
            b = clrCurrent.blue()+rand_value;
            while(b>255)b-=256;
            while(b<0)b+=256;
            qDebug()<<r<<" "<<g<<" "<< b;
            changed_img->setPixel(j,i,qRgb(r,g,b));
        }
    }
    QPixmap pix(QPixmap::fromImage(*changed_img));
    int w = pix.width();
    int h = pix.height();
    image_size = pix.size();
    ui->image->resize(w,h);
    ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
}

void photo_edit::on_gray_level_3_valueChanged(int value)
{
    ui->level_3->setText(QString::number(ui->gray_level_3->value()));
    if(current_gray_level<value)
    {
        int diff = value-current_gray_level;
        for(int k = 0; k < diff; k++)
        {
            for(int i = 0; i < image_size.height();i++)
            {
                for(int j = 0; j < image_size.width();j++)
                {
                    int rgb_sum = 0;
                    QColor clrCurrent( changed_img->pixel( j, i ) );
                    rgb_sum = rgb_sum+clrCurrent.red()+clrCurrent.green()+clrCurrent.blue();
                    rgb_sum/=4;

                    changed_img->setPixel(j,i,qRgb(rgb_sum,rgb_sum,rgb_sum));
                }
            }
        }
    }
    else
    {
        changed_img->load(fileName);
        for(int k = 0; k < value; k++)
        {
            for(int i = 0; i < image_size.height();i++)
            {
                for(int j = 0; j < image_size.width();j++)
                {
                    int rgb_sum = 0;
                    QColor clrCurrent( changed_img->pixel( j, i ) );
                    rgb_sum = rgb_sum+clrCurrent.red()+clrCurrent.green()+clrCurrent.blue();
                    rgb_sum/=4;

                    changed_img->setPixel(j,i,qRgb(rgb_sum,rgb_sum,rgb_sum));
                }
            }
        }
    }
    current_gray_level = value;
    QPixmap pix(QPixmap::fromImage(*changed_img));
    int w = pix.width();
    int h = pix.height();
    image_size = pix.size();
    ui->image->resize(w,h);
    ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

}

void photo_edit::on_open_photo_button_clicked()
{
    QFileDialog dialog(this);
      //dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
      //dialog.setViewMode(QFileDialog::Detail);
     fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      "C:/",
                                                      tr("Images (*.png *.xpm *.jpg)"));
    original_img = new QImage(fileName);
    QPixmap pix(QPixmap::fromImage(*original_img));
    int w = pix.width();
    int h = pix.height();
    image_size = original_img->size();
    ui->image->resize(w,h);
    ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    changed_img = new QImage(fileName);

    //img = new QImage(":/res/lena_color.jpg");

    //якщо б ми мали графік сцену
     /*image_size = original_img->size();
     graphic_scene = new QGraphicsScene();
     graphic_scene->addPixmap(QPixmap::fromImage(*original_img));
     ui->graphicsView->resize(image_size);
     ui->graphicsView->setScene(graphic_scene);*/
}
