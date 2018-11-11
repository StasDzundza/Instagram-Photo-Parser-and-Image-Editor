#include "photo_paint.h"
#include "ui_photo_paint.h"

photo_paint::photo_paint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::photo_paint)
{
    this->parent = parent;

    ui->setupUi(this);
    connect(this->parent,SIGNAL(paint_object(QImage*)),this,SLOT(get_photo(QImage*)));
}

photo_paint::~photo_paint()
{
    delete ui;
}

void photo_paint::get_photo(QImage *image)
{
    this->image = image;
    QSize image_size = image->size();
    scene = new paintScene();
    scene->addPixmap(QPixmap::fromImage(*image));
    ui->graphicsView->resize(image_size);
    ui->graphicsView->setScene(scene);
}
