#include <QDebug>
#include "kennel_fan_controller.h"

kennel_fan_controller::kennel_fan_controller(int &argc, char **argv) : QCoreApplication(argc, argv),
    kennel_fan_1(e_pwm_fan_id::e_pwm_fan_1) {
    QString on_off_button_gpio_value_path;

    fan_1_tach = new gpio_tach("GPIO_25", "Kennel Fan 1 tach");

    fan_rotary_encoder = new gpio_rotary("GPIO_27", "GPIO_17", "Kennel Fan 1 speed dial");
    fan_rotary_encoder->set_bounds(0, 20);
    connect(fan_rotary_encoder, SIGNAL(value_changed(int)), &kennel_fan_1, SLOT(set_run_speed(int)));

    on_off_button = new lin_gpio("GPIO_13");
    fan_power_on_off = new lin_gpio("GPIO_12");
    fan_power_on_off->gpio_set_direction("out");
    fan_power_on_off->gpio_set_value("0");              /* Start out with the fan off */
    fan_power_state = true;                             /* And set the state to on, because ready_read will fire. */

    on_off_button->gpio_set_direction("in");
    on_off_button->gpio_set_edge("rising");             /* Interrupt on rising edge */
    on_off_button_gpio_value_path = QString::fromStdString(on_off_button->gpio_get_path());
    on_off_button_gpio_value.setFileName(on_off_button_gpio_value_path);
    on_off_button_gpio_value.open(QFile::ReadOnly);
    on_off_button_gpio_value_notifier = new QSocketNotifier(on_off_button_gpio_value.handle(),
                                                            QSocketNotifier::Exception);
    on_off_button_gpio_value_notifier->setEnabled(true);
    connect(on_off_button_gpio_value_notifier, SIGNAL(activated(int)), this, SLOT(on_off_button_gpio_ready_read(int)));

    if (!cmd_server.listen(QHostAddress::Any, 45046)) {
       abort();
    } /* endif */
    connect(&cmd_server, SIGNAL(command_received(QString &)), this, SLOT(command_proc(QString &)));
    disp_svc = new ZConfService(this);
    disp_svc->registerService("kennel_fan_controller", 45046, "_kennel_fan_ctrlr._tcp");
}

void kennel_fan_controller::on_off_button_gpio_ready_read(int) {
    QByteArray on_off_button_line;          /* Value read from encoder on/off button GPIO value */

    on_off_button_gpio_value.seek(0);
    on_off_button_line = on_off_button_gpio_value.read(2);
    if (on_off_button_line.size() < 1) {
        qDebug() << __func__ << ":" << __LINE__ << " gpio on/off button read failed.." ;
    } else {
        /* This needs to toggle the power on and off.... */
        fan_power_state = (fan_power_state ^ true);
        if (fan_power_state) {
            fan_power_on_off->gpio_set_value("1");
        } else {
            fan_power_on_off->gpio_set_value("0");
        }
    } /* endif */
} /* on_off_button_gpio_ready_read */

kennel_fan_controller::~kennel_fan_controller() {
    delete fan_1_tach;
}

void kennel_fan_controller::on_fan1speed_valueChanged(int value) {

    kennel_fan_1.set_pwm_fan_rotation_speed(float(value));
}

void kennel_fan_controller::command_proc(QString &cmd) {
    qDebug() << "kennel fan controller, command received" << cmd;
}

void kennel_fan_controller::on_actionClose_triggered() {
    QCoreApplication::quit();
}
