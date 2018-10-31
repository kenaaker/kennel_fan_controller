#include "lin_pwm.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <time.h>
#include <string>

void lin_pwm::pwm_export(void) {
    if (unit_number == -1) {
        qDebug() << __func__ << ":" << __LINE__ << " Internal error. pwm_export_file=" <<
                    export_path_file_name.c_str() << "unit_number=" << unit_number;
    } else {
        QFile pwm_export(QString::fromStdString(export_path_file_name));
        if (pwm_export.open(QIODevice::WriteOnly)) {
            QTextStream export_stream(&pwm_export);
            export_stream << unit_number;
            pwm_export.close();
        } /* endif */
    } /* endif */
} /* export PWM unit to enable */

void lin_pwm::pwm_unexport(void) {
    if (unit_number != -1) {
        QFile pwm_unexport(QString::fromStdString(unexport_path_file_name));
        if (pwm_unexport.open(QIODevice::WriteOnly)) {
            QTextStream unexport_stream(&pwm_unexport);
            unexport_stream << unit_number;
            pwm_unexport.close();
        } /* endif */
    } /* endif */
} /* unexport PWM unit to disable */

bool lin_pwm::get_enable(void) {
    return enabled;
} /* export PWM unit to enable */

void lin_pwm::set_enable(bool in_enabled) {
    enabled = in_enabled;
} /* export PWM unit to enable */

void lin_pwm::pwm_enable(void) {
    QFile pwm_enable_file(QString::fromStdString(enable_path_file_name));
    if (pwm_enable_file.open(QIODevice::WriteOnly)) {
        QTextStream enable_stream(&pwm_enable_file);
        enable_stream << 1;
        pwm_enable_file.close();
        set_enable(true);
    } /* endif */
} /* export PWM unit to enable */

void lin_pwm::pwm_disable(void) {
    QFile pwm_disable(QString::fromStdString(enable_path_file_name));
    if (pwm_disable.open(QIODevice::WriteOnly)) {
        QTextStream enable_stream(&pwm_disable);
        enable_stream << 0;
        pwm_disable.close();
        set_enable(false);
    } /* endif */
} /* export PWM unit to enable */

void lin_pwm::set_frequency(float in_freq) {
    if (in_freq >= 0.0) {
        QTextStream out(&period_file);
        out << 1.0E9 / in_freq;
        freq = in_freq;
    } /* endif */
} /* set_frequency */

void lin_pwm::set_duty_cycle(float in_duty) {
    if ((in_duty >= 0.0) && (in_duty <= 100.0)) {
        QTextStream out(&duty_file);
        out << ((1.0E9 / freq) * (in_duty / 100.0));
        duty_cycle_percent = in_duty;
    } /* endif */
} /* set_duty_cycle */

void lin_pwm::set_polarity(bool in_polarity) {
    bool saved_enable;

    saved_enable = get_enable();
    QTextStream out(&polarity_file);
    pwm_disable();
    if (!in_polarity) {
        out << "normal";
    } else {
        out << "inversed";
    }
    polarity = in_polarity;
    if (saved_enable) {
        pwm_enable();
    }
} /* set_polarity */

float lin_pwm::get_frequency(void) {
    return freq;
} /* get_frequency */

float lin_pwm::get_duty_cycle(void) {
    return duty_cycle_percent;
} /* get_duty_cycle */

bool lin_pwm::get_polarity(void) {
    return polarity;
} /* get_polarity */

lin_pwm::lin_pwm() {

}

lin_pwm::lin_pwm(const string &pwm_name) {
    string pwm_generic_path;
    int wait_count = 100;

    chip_name = pwm_lookup.pwmchip_by_name(pwm_name);
    chip_number = pwm_lookup.pwm_chip_number_by_name(pwm_name);
    unit_number = pwm_lookup.pwm_chip_unit_by_name(pwm_name);

    if ((chip_name.length() == 0) || (chip_number == -1) || (unit_number == -1)) {
        cout << __func__ << ":" << __LINE__ << " set failed for pwm, pwm_name=\"" << pwm_name << "\"" << endl;
        abort();
    } else {

        export_path_file_name = "/sys/class/pwm/" + chip_name + "/export";
        unexport_path_file_name = "/sys/class/pwm/" + chip_name + "/unexport";
        pwm_export();

        if (pwm_lookup.get_pwm_table_name() == "pi-zero") {
            pwm_generic_path = "/sys/class/pwm/pwm" + std::to_string(unit_number);
        } else if (pwm_lookup.get_pwm_table_name() == "bbb") {
            pwm_generic_path = "/sys/class/pwm/pwm-" +
                            std::to_string(chip_number) + ":" + std::to_string(unit_number);
        } else {
            cout << __func__ << ":" << __LINE__ << " set failed for pwm, pwm_name=\"" << pwm_name << "\"" << endl;
            abort();
        }

        enable_path_file_name = pwm_generic_path + "/enable";
        // cout << " enable_path_file_name = \"" << enable_path_file_name << "\"" << endl;
        period_path_file_name = pwm_generic_path + "/period";
        period_file.setFileName(QString::fromStdString(period_path_file_name));
        duty_path_file_name = pwm_generic_path + "/duty_cycle";
        duty_file.setFileName(QString::fromStdString(duty_path_file_name));
        polarity_path_file_name = pwm_generic_path + "/polarity";
        polarity_file.setFileName(QString::fromStdString(polarity_path_file_name));
        /* Now open all the files */
        while ((!period_file.open(QIODevice::ReadWrite)) && (wait_count > 0)) {
            nanosleep((const struct timespec[]){{0, 200000000}}, NULL);
            --wait_count;
        } /* endwhile */
        if (wait_count == 0) {
            qDebug() << __func__ << ":" << __LINE__ << "Couldn't open period file.";
            abort();
        } else {
            pwm_enable();
            if (!duty_file.open(QIODevice::ReadWrite)) {
                period_file.close();
                qDebug() << __func__ << ":" << __LINE__ << "Couldn't open duty cycle file.";
                abort();
            } else {
                if (!polarity_file.open(QIODevice::ReadWrite)) {
                    duty_file.close();
                    period_file.close();
                    qDebug() << __func__ << ":" << __LINE__ << "Couldn't set PWM polarity.";
                    abort();
                } else {
                    set_frequency(2000);
                    set_duty_cycle(0);
                    set_polarity(false);
                } /* endif */
            } /* endif */
        } /* endif */
    } /* endif */
} /* lin_pmw() */

lin_pwm::~lin_pwm() {
    pwm_disable();
    period_file.close();
    duty_file.close();
    polarity_file.close();
    pwm_unexport();
}
