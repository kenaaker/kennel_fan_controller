#include "../kennel_fan_controller.h"

int main(int argc, char *argv[]) {
    kennel_fan_controller kennel_fan_ctrlr(argc, argv);

    return kennel_fan_ctrlr.exec();
}
