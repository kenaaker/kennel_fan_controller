#ifndef KENNEL_FAN_CONTROLLER_H
#define KENNEL_FAN_CONTROLLER_H

#include <QCoreApplication>
#include "gpio_tach.h"
#include "gpio_rotary.h"
#include "pwm_fan.h"
#include "lin_gpio.h"
#include "zconfservice.h"
#include "zconfserviceclient.h"
#include "kennel_fan_sslsock.h"

namespace Ui {
class kennel_fan_controller;
}

class kennel_fan_controller : public QCoreApplication {
    Q_OBJECT

  public:
    explicit kennel_fan_controller(int &argc, char **argv);
    ~kennel_fan_controller();

private slots:
    void on_fan1speed_valueChanged(int value);
    void on_off_button_gpio_ready_read(int value);
    void command_proc(QString &cmd);

    void on_actionClose_triggered();

private:
    pwm_fan kennel_fan_1;
    gpio_tach *fan_1_tach;
    gpio_rotary *fan_rotary_encoder;
    lin_gpio *on_off_button;
    lin_gpio *fan_power_on_off;
    bool fan_power_state;
    QSocketNotifier *on_off_button_gpio_value_notifier;
    QFile on_off_button_gpio_value;           /* on_off_button_gpio GPIO value file. */
    kennel_fan_sslsock cmd_server;
    ZConfService *disp_svc;
};

#endif // KENNEL_FAN_CONTROLLER_H
