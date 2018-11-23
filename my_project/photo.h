#ifndef PHOTO_H
#define PHOTO_H
#include<QImage>
#include<QString>

class photo
{
public:
    photo();
    photo(QImage*image);
    void set_image(QImage*);
    int get_count_width_pixels();
    int get_count_height_pixels();
    QString get_path();
    void set_path(QString path);
private:
    QImage*image;
    int count_width_pixels = 0;
    int count_height_pixels = 0;
    QSize image_size;
    QString path;
};

#endif // PHOTO_H
