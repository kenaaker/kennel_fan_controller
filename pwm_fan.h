#ifndef PWM_FAN_H
#define PWM_FAN_H
#include <QElapsedTimer>
#include <QTimer>

#include "lin_pwm.h"

#define pwm_fan_1_pin "PWM0A";
#define pwm_fan_2_pin "PWM0B";

enum class e_pwm_fan_id {
    e_pwm_fan_1,
    e_pwm_fan_2,
};

class pwm_fan : public QObject {
    Q_OBJECT

public:
    pwm_fan();
    pwm_fan(e_pwm_fan_id which_pwm_fan);
    ~pwm_fan();
    void pwm_fan_run(int speed);
    void pwm_fan_stop(void);
    int pwm_fan_get_speed();
    void set_pwm_fan_rotation_speed(float rpm);
public slots:
    void set_run_speed(int fan_speed);
private:
    QString pwm_pin;
    e_pwm_fan_id pwm_fan_id;
    int pwm_fan_speed;
    /* Interfaces to PWM stuff */
    lin_pwm *dc;          /* Pulse Width Modulated motor control for pwm_fan */
    /* Interfaces to GPIO stuff */
    float pwm_fan_rotation_speed;   /* Motor rotation speed in RPM (fractional for slow pwm_fan) */
};

#endif /* PWM_FAN_H */
