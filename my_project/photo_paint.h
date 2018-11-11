#ifndef PHOTO_PAINT_H
#define PHOTO_PAINT_H

#include <QDialog>
#include "paintscene.h"
namespace Ui {
class photo_paint;
}

class photo_paint : public QDialog
{
    Q_OBJECT

public:
    explicit photo_paint(QWidget *parent = nullptr);
    ~photo_paint();
private slots:
    void get_photo(QImage*image);

private:
    Ui::photo_paint *ui;
    QImage*image;
    QWidget*parent;
    paintScene*scene = nullptr;
};

#endif // PHOTO_PAINT_H
