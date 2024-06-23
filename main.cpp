#include "pixelbatch.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PixelBatch w;
    w.show();
    return a.exec();
}
