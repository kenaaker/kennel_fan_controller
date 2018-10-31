#include "kennel_fan_controller.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    kennel_fan_controller w;
    w.show();

    return a.exec();
}
