#include "pixelbatch.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QApplication::setApplicationName(APPLICATION_FULLNAME);
  QApplication::setDesktopFileName("com.ktechpit.pixelbatch");
  QApplication::setOrganizationDomain("com.ktechpit");
  QApplication::setOrganizationName("org.keshavnrj.ubuntu");
  QApplication::setApplicationVersion(VERSIONSTR);

  PixelBatch w;
  w.show();

  return a.exec();
}
