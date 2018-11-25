#include "photo_edit.h"
#include "ui_photo_edit.h"
#include<QLabel>
#include<QDebug>
#include<QPainter>
#include<QPen>
#include<QColorDialog>
#include<QPalette>
#include<QColor>
#include<QRgb>
#include<QDir>
#include<QFileDialog>
#include<random>
#include<vector>
#include<QTextStream>
#include<QByteArray>

#include<QStyle>
photo_edit::photo_edit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::photo_edit)
{
    ui->setupUi(this);
    this->showMaximized();

    get_count_of_changed_images();
    if(count_of_changed_images == 0)
    {
        count_of_changed_images++;
    }

    ui->gray_level_3->setMaximum(7);
    ui->border_width->setMinimum(1);
    ui->border_width->setMaximum(30);
    ui->brightness_value->setMinimum(-255);
    ui->brightness_value->setMaximum(255);
    ui->brightness_value->setValue(0);
    ui->transparency_level->setMaximum(2.5);
    ui->transparency_level->setMinimum(1.0);
    ui->transparency_level->setSingleStep(0.05);

    ui->horizontal_mirror->setChecked(true);

    manager_photo = new QNetworkAccessManager(this);
    manager_photo->setStrictTransportSecurityEnabled(true);
    connect(manager_photo, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinishedPhoto(QNetworkReply*)));
}


photo_edit::~photo_edit()
{
    delete ui;
}

int photo_edit::get_count_of_changed_images()
{
    QFile in("../my_project/count_changed_images.txt");
    in.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream writeStream(&in);
    writeStream>>count_of_changed_images;
    in.close();
    return count_of_changed_images;
}


void photo_edit::on_grayscale_button_clicked()
{
    if(changed_img)
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
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }

}

void photo_edit::on_negative_button_clicked()
{
    if(changed_img)
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
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_sepia_button_clicked()
{
    if(changed_img)
    {
        for(int i = 0; i < image_size.height();i++)
        {
            for(int j = 0; j < image_size.width();j++)
            {
                double rgb_sum = 0;
                QColor clrCurrent( changed_img->pixel( j, i ) );
                rgb_sum = 0.3*(double)clrCurrent.red()+0.59*(double)clrCurrent.green()+0.11*(double)clrCurrent.blue();
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
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_swap_colors_button_clicked()
{
    if(changed_img)
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
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_add_text_button_clicked()
{
    if(changed_img)
    {
        QPainter painter;
        painter.begin(changed_img);
        painter.setFont(QFont("Tahoma", 12, QFont::Bold));
        painter.setPen(QPen(Qt::red));
        QPen pen;
        QColor c;
        //c.setGreen(255);
        //c.setRed(255);
        pen.setColor(c);
        //painter.setPen(pen);
        painter.drawText(changed_img->rect(), Qt::AlignCenter, QString("Text on photo"));
        painter.end();

        QPixmap pix(QPixmap::fromImage(*changed_img));
        int w = pix.width();
        int h = pix.height();
        image_size = pix.size();
        ui->image->resize(w,h);
        ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_to_ascii_button_clicked()
{
    if(changed_img)
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
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_save_btn_clicked()
{
    if(changed_img)
    {
        QString filename_save = "../my_project/edited/photo_" + QString::number(count_of_changed_images++)+".jpg";
        changed_img->save(filename_save);
        QFile out("../my_project/count_changed_images.txt");
        out.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream writeStream(&out);
        writeStream<<count_of_changed_images;
        out.close();
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_noise_btn_clicked()
{
    if(changed_img)
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
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_gray_level_3_valueChanged(int value)
{
    if(changed_img)
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
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }

}

void photo_edit::on_open_photo_button_clicked()
{
    QFileDialog dialog(this);
      //dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
      //dialog.setViewMode(QFileDialog::Detail);
     fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      "../my_project/",
                                                      tr("Images (*.png *.xpm *.jpg)"));
     if(fileName!="")
     {
        original_img = new QImage(fileName);
        current_photo.set_image(original_img);
        current_photo.set_path(fileName);
        QPixmap pix(QPixmap::fromImage(*original_img));
        int w = pix.width();
        int h = pix.height();
        image_size = original_img->size();
        ui->image->resize(w,h);
        ui->image->setPixmap(pix.scaled(w,h,Qt::IgnoreAspectRatio));

        changed_img = new QImage(fileName);

        ui->x_point->setMaximum(image_size.width());
        ui->y_point->setMaximum(image_size.height());
        ui->x_point->setValue(image_size.width());
        ui->y_point->setValue(image_size.height());

        //якщо б ми мали графік сцену

        /*changed_img = new QImage(fileName);
         image_size = original_img->size();
         graphic_scene = new paintScene();
         graphic_scene->addPixmap(QPixmap::fromImage(*original_img));
         ui->graphicsView->resize(image_size);
         ui->graphicsView->setScene(graphic_scene);*/
     }
}

void photo_edit::on_deep_sepia_button_clicked()
{
    if(changed_img)
    {
        for(int i = 0; i < image_size.height();i++)
        {
            for(int j = 0; j < image_size.width();j++)
            {
                QColor clrCurrent( changed_img->pixel( j, i ) );
                double r,g,b;
                r = ((double)clrCurrent.red() * 0.393 + (double)clrCurrent.green() * 0.769 + (double)clrCurrent.blue() * 0.189 ) / 1.351;
                g = ((double)clrCurrent.red() * 0.349 + (double)clrCurrent.green() * 0.686 + (double)clrCurrent.blue() * 0.168 ) / 1.351;
                b = ((double)clrCurrent.red() * 0.272 + (double)clrCurrent.green() * 0.534 + (double)clrCurrent.blue() * 0.131 ) / 1.351;
                changed_img->setPixel(j,i,qRgb(r,g,b));
            }
        }
        QPixmap pix(QPixmap::fromImage(*changed_img));
        int w = pix.width();
        int h = pix.height();
        image_size = pix.size();
        ui->image->resize(w,h);
        ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

        /*
        graphic_scene = new QGraphicsScene();
        graphic_scene->addPixmap(pix);
        ui->graphicsView->resize(image_size);
        ui->graphicsView->setScene(graphic_scene);*/
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_transparency_button_clicked()
{
    if(changed_img)
    {
        QImage fragment( changed_img->size(), QImage::Format_ARGB32 );
        int transparency_level = ui->transparency_level->value()*100;
        fragment.fill(transparency_level);

        changed_img->setAlphaChannel(fragment);
        QPixmap pix(QPixmap::fromImage(*changed_img));
        int w = pix.width();
        int h = pix.height();
        image_size = pix.size();
        ui->image->resize(w,h);
        ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_reset_button_clicked()
{
    if(changed_img)
    {
        changed_img->operator=(*original_img);
        QPixmap pix(QPixmap::fromImage(*changed_img));
        int w = pix.width();
        int h = pix.height();
        image_size = pix.size();
        ui->image->resize(w,h);
        ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_scale_button_clicked()
{
    if(changed_img)
    {
        changed_img->scaled(ui->x_point->value(),ui->y_point->value()).save("../my_project/edited/scaled_photo_" + QString::number(count_of_changed_images)+".jpg");
        changed_img->load("../my_project/edited/scaled_photo_" + QString::number(count_of_changed_images)+".jpg");
        original_img->load("../my_project/edited/scaled_photo_" + QString::number(count_of_changed_images)+".jpg");
        count_of_changed_images++;
        QFile out("../my_project/count_changed_images.txt");
        out.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream writeStream(&out);
        writeStream<<count_of_changed_images;
        out.close();
        QPixmap pix(QPixmap::fromImage(*changed_img));
        int w = pix.width();
        int h = pix.height();
        image_size = pix.size();
        ui->image->resize(w,h);
        ui->image->setPixmap(pix.scaled(ui->x_point->value(),ui->y_point->value(),Qt::IgnoreAspectRatio));
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_screen_button_clicked()
{
    QPixmap pixmap(this->size());
      this->render(&pixmap, QPoint(), QRegion(this->rect()));
      QPainter ptr(&pixmap);


      QRect border(0,0,this->width() - 1, this->height() - 1);
      //ptr.setBrush(QBrush(QColor(0,0,255,125)));
      ptr.drawRect(border);

     // ptr.setBrush(QBrush());
      ptr.setPen(QPen(Qt::red));
      ptr.drawRect(border);

      pixmap.save("../my_project/edited/screen_" + QString::number(count_of_changed_images++) + ".png", 0, 100);
      QFile out("../my_project/count_changed_images.txt");
      out.open(QIODevice::WriteOnly | QIODevice::Text);
      QTextStream writeStream(&out);
      writeStream<<count_of_changed_images;
      out.close();
}

void photo_edit::on_draw_smile_button_clicked()
{
    if(changed_img)
    {
        QImage sourceImage;

        int dx = (changed_img->width())/2;
        int dy = (changed_img->height())/2;

        switch (ui->smile_list->currentIndex())
        {
            case 0:
            {
                sourceImage.load(":/smiles/smiles/128smile1.png");
                break;
            }
            case 1:
            {
                sourceImage.load(":/smiles/smiles/128smile2.png");
                break;
            }
            case 2:
            {
                sourceImage.load(":/smiles/smiles/128smile3.png");
                break;
            }
        }
        QPainter p(changed_img);
        p.drawImage(dx, dy, sourceImage);
        p.end();
        QPixmap pix(QPixmap::fromImage(*changed_img));
        int w = pix.width();
        int h = pix.height();
        image_size = pix.size();
        ui->image->resize(w,h);
        ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_mirror_button_clicked()
{
    if(changed_img)
    {
        if(ui->vertical_mirror->isChecked())
        {
            mirrored_image = changed_img->mirrored(false,true);
        }
        if(ui->horizontal_mirror->isChecked())
        {
            mirrored_image = changed_img->mirrored(true,false);
        }
        if(ui->horizontal_mirror->isChecked()||ui->vertical_mirror->isChecked())
        {
            changed_img = &mirrored_image;
            QPixmap pix(QPixmap::fromImage(*changed_img));
            int w = pix.width();
            int h = pix.height();
            image_size = pix.size();
            ui->image->resize(w,h);
            ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
        }
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }

}

void photo_edit::on_paint_button_clicked()
{
    if(changed_img)
    {
        paint_window = new photo_paint(this);
        paint_window->show();
        emit paint_object(changed_img);
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_open_by_url_button_clicked()
{
    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    request_photo.setSslConfiguration(sslConfiguration);
    request_photo.setUrl(QUrl(ui->source_img->text()));
    manager_photo->get(request_photo);
}

void photo_edit::replyFinishedPhoto(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray data = reply->readAll();
            QImage *image =new QImage(QImage::fromData(data)) ;
            changed_img = original_img = image;
            current_photo.set_image(original_img);
            current_photo.set_path("from internet");
            QPixmap pix(QPixmap::fromImage(*changed_img));
            int w = pix.width();
            int h = pix.height();
            image_size = pix.size();
            ui->image->resize(w,h);
            ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
            changed_img->save("../my_project/downloaded by reference/image" + QString::number(count_of_changed_images++) + ".jpg");

            ui->x_point->setMaximum(image_size.width());
            ui->y_point->setMaximum(image_size.height());
            ui->x_point->setValue(image_size.width());
            ui->y_point->setValue(image_size.height());
        }
    else
    {
        ui->image->setText("Invalid reference");
    }
}

void photo_edit::on_draw_border_button_clicked()
{
    if(changed_img)
    {
        QPainter ptr(changed_img);
        QRect border(0,0,changed_img->width() - 1, changed_img->height() - 1);
        QPen pen;
        pen.setColor(current_color);
        pen.setWidth(ui->border_width->value());
        ptr.setPen(pen);
        ptr.drawRect(border);
        QPixmap pix(QPixmap::fromImage(*changed_img));
        int w = pix.width();
        int h = pix.height();
        image_size = pix.size();
        ui->image->resize(w,h);
        ui->image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
        changed_img->save("../my_project/edited/photo_" + QString::number(count_of_changed_images++) + ".jpg");
        QFile out("../my_project/count_changed_images.txt");
        out.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream writeStream(&out);
        writeStream<<count_of_changed_images;
        out.close();
    }
    else
    {
        ui->image->setText("There is no photo! Open it!");
    }
}

void photo_edit::on_border_color_clicked()
{
    ui->border_color->setAutoFillBackground(true);
    current_color = QColorDialog::getColor();
    int red = current_color.red();
    int green = current_color.green();
    int blue = current_color.blue();
    ui->border_color->setStyleSheet("background-color: rgb(" + QString::number(red) + ',' + QString::number(green) + ',' + QString::number(blue) + ')');
}

void photo_edit::on_brightness_button_clicked()
{
    int brightness = ui->brightness_value->value();
    for(int i = 0; i < image_size.height();i++)
    {
        for(int j = 0; j < image_size.width();j++)
        {
            QColor clrCurrent( changed_img->pixel( j, i ) );
            int r,g,b;
            r = clrCurrent.red();
            g = clrCurrent.green();
            b = clrCurrent.blue();
            r+=brightness;
            g+=brightness;
            b+=brightness;
            if(r>255){r = 255;}
            if(g>255){g = 255;}
            if(b>255){b = 255;}
            if(r<0){r = 0;}
            if(g<0){g = 0;}
            if(b<0){b = 0;}
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
