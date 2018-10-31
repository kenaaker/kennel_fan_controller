
#include <iostream>

#include <QtCore>
#include <QString>
#include <QtGlobal>
#include <QtCore/QDebug>

#include "pwm_fan.h"
#include "lin_pwm.h"

pwm_fan::pwm_fan() {
    pwm_pin = "Uninit";
    pwm_fan_rotation_speed = 1.0;
}

pwm_fan::~pwm_fan() {
    delete dc;
}

/* Create the control structure to setup, run, and shutdown a pwm_fan  */
/* Attached to a GPIO pin */
pwm_fan::pwm_fan(e_pwm_fan_id m_id) {
    switch (m_id) {
    case e_pwm_fan_id::e_pwm_fan_1:
        pwm_pin = pwm_fan_1_pin;
        break;
    case e_pwm_fan_id::e_pwm_fan_2:
        pwm_pin = pwm_fan_2_pin;
        break;
    default:
        qDebug() << __func__ << ":" << __LINE__ << " Invalid pwm_fan ID.";
        QCoreApplication::quit();
        break;
    } /* endswitch */
    pwm_fan_id = m_id;
    pwm_fan_speed = 0;
    dc = new lin_pwm(pwm_pin.toStdString());
}


/* The expected range here is -100<=speed<=100 */
void pwm_fan::pwm_fan_run(int speed) {
    uint abs_speed;

    speed = (int)qBound(-100, (int)speed, 100);
    abs_speed = abs(speed);
    dc->set_duty_cycle(abs_speed);
    pwm_fan_speed = speed;
}

/* The expected range here is 0<=fan_speed<=20 */
void pwm_fan::set_run_speed(int fan_speed) {
    pwm_fan_run(fan_speed * 5);
}

void pwm_fan::pwm_fan_stop(void) {
    pwm_fan_run(0);
}

int pwm_fan::pwm_fan_get_speed() { /* pwm_fan_get_speed */
    return pwm_fan_speed;
}

void pwm_fan::set_pwm_fan_rotation_speed(float rpm) {
    int int_rpm = (int)rpm;
    pwm_fan_run(int_rpm);
    pwm_fan_rotation_speed = rpm;
}
