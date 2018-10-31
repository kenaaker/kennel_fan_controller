#ifndef LIN_PWM_H
#define LIN_PWM_H

#include <QFile>
#include "lin_gpio_util.h"

using namespace std;

enum class motor_dirs {
    md_cw,              /* Motor direction clockwise */
    md_ccw              /* Motor direction counter clockwise ... aka widdershins */
};

class lin_pwm {

public:
    lin_pwm();
    lin_pwm(const string &pwm_name);
    ~lin_pwm();
    void set_frequency(float in_freq);
    void set_duty_cycle(float in_duty);
    void set_polarity(bool in_polarity);
    float get_frequency(void);
    float get_duty_cycle(void);
    bool get_polarity(void);
private:
    void pwm_export(void);
    void pwm_unexport(void);
    void pwm_enable(void);
    void pwm_disable(void);
    bool get_enable(void);
    void set_enable(bool in_enabled);
    string name;
    string chip_name;
    string pwm_link_name;
    int chip_number;
    int unit_number;
    string enable_path_file_name;
    string period_path_file_name;
    string duty_path_file_name;
    string polarity_path_file_name;
    string export_path_file_name;
    string unexport_path_file_name;
    QFile period_file;
    QFile duty_file;
    QFile polarity_file;
    float freq;
    float duty_cycle_percent;
    bool enabled;
    bool polarity;
    pwms_table pwm_lookup;
};

#endif // ADAFRUIT_BBIO_PWM_H
