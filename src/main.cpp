#include "ui/MainWindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>

int main(int argc, char **argv) {
  // Set OpenGL format before application starts (Best practice for Qt6/VTK9)
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  QApplication app(argc, argv);

  MainWindow window;
  window.show();

  return app.exec();
}
