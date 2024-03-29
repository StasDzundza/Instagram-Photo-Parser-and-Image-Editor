#include "photo.h"

photo::photo()
{

}

photo::photo(QImage *image)
{
    this->image = image;
    image_size = image->size();
    count_height_pixels  =image_size.height();
    count_width_pixels = image_size.width();
}

void photo::set_image(QImage *image)
{
    this->image = image;
    image_size = image->size();
    count_height_pixels  =image_size.height();
    count_width_pixels = image_size.width();
}

int photo::get_count_width_pixels()
{
    return count_width_pixels;
}

int photo::get_count_height_pixels()
{
    return count_height_pixels;
}

QString photo::get_path()
{
    return path;
}

void photo::set_path(QString path)
{
    this->path = path;
}
