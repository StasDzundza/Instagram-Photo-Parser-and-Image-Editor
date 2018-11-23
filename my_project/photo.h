#ifndef PHOTO_H
#define PHOTO_H
#include<QImage>

class photo
{
public:
    photo();
    photo(QImage*image);
    int get_count_width_pixels();
    int get_count_height_pixels();
private:
    QImage*image;
    int count_width_pixels = 0;
    int count_height_pixels = 0;
    QSize image_size;
};

#endif // PHOTO_H
