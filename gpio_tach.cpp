
#include <string>
#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include "gpio_tach.h"

const QString gpio_tach::base_model_name(void) {
    QString ret_value;
    /* Detect the type of platform, Beaglebone or Raspberry Pi */
    static const string platform_type_filename = "/sys/firmware/devicetree/base/model";
    /* Read the model from the Device tree info. */
    QFile platform_type_file(QString::fromStdString(platform_type_filename));
    if (platform_type_file.open(QIODevice::ReadOnly)) {
        QString platform_type_str;
        QTextStream platform_type_stream(&platform_type_file);
        platform_type_stream >> platform_type_str;
        if (platform_type_str.startsWith("Raspberry")) {
            ret_value = "Raspberry PI";
        } else if (platform_type_str.startsWith("TI")) {
            ret_value = "Beaglebone";
        } else {
            qDebug() << "Cannot determine platform type, platform_str is " << platform_type_str << ", quitting.";
            abort();
        } /* endif */
        platform_type_file.close();
    } /* endif */
    return ret_value;
}

void gpio_tach::gpio_pullup(const lin_gpio &in_gpio) {
    string in_gpio_key;
    QString gpio_pullup_filename;

    in_gpio_key = in_gpio.gpio_get_key();
    gpio_pullup_filename = "/sys/devices/platform/ocp/ocp:" +
            QString::fromStdString(in_gpio_key) + "_pinmux/state";
    QFile gpio_pullup_cfg(gpio_pullup_filename);
    if (!gpio_pullup_cfg.open(QIODevice::WriteOnly)) {
        int gpio_to_set_pullup_on = in_gpio.gpio_get_number();
        const QString hw_name(base_model_name());
        qDebug() << " Pullup set open failed for file " << gpio_pullup_filename << endl;
        qDebug() << " Trying platform specific command to set pullup for GPIO " << gpio_to_set_pullup_on << endl;
        if (hw_name.startsWith("Raspberry PI")) {
            QProcess::execute("/usr/bin/raspi-gpio", QStringList () << "set"
                                << QString::number(gpio_to_set_pullup_on) << "pu");
        } else if (hw_name.startsWith("Beaglebone")) {
            QProcess::execute("/usr/local/bin/config-pin", QStringList() << in_gpio_key.c_str() << "in+");
        } else {
            qDebug() << __func__ << ":" << __LINE__ << " Quitting.";
            abort();
        }
    } else {
        QTextStream export_stream(&gpio_pullup_cfg);
        export_stream << "gpio_pu";
        gpio_pullup_cfg.close();
    } /* endif */
} /* enable pullup resistor on requested pin. */

gpio_tach::gpio_tach() {

    this_gpio = nullptr;
}

gpio_tach::gpio_tach(QString tach_name, QString in_fan_name) {

    this_gpio = new lin_gpio(tach_name.toStdString());
    if (!this_gpio) {
        qDebug() << __func__ << ":" << __LINE__ << " GPIO allocation failed.";
        QCoreApplication::quit();
    } else {
        QString gpio_value_path;
        fan_name = in_fan_name;
        pulses_counted = 0;
        gpio_value_path = QString::fromStdString(this_gpio->gpio_get_path());
        tach_value.setFileName(gpio_value_path);    /* Set up the value file */
        /* Added feature, turn on the Pull UP resistor. */
        gpio_pullup(*this_gpio);
        this_gpio->gpio_set_direction("in");
        this_gpio->gpio_set_edge("rising");         /* Interrupt on rising edge */
        tach_value.open(QFile::ReadOnly);
        tach_notifier = new QSocketNotifier(tach_value.handle(), QSocketNotifier::Exception);
        tach_notifier->setEnabled(true);
        connect(tach_notifier, SIGNAL(activated(int)), this, SLOT(ready_read(int)));
        last_calculate_time.start();
    } /* endif */
}

gpio_tach::~gpio_tach() {
    tach_value.close();
    this_gpio->gpio_set_edge("none");             /* Don't generate interrupts. */
    delete tach_notifier;
    delete this_gpio;
}

int gpio_tach::tach_fd(void) {
    return tach_value.handle();
} /* tach_fd */

QString const gpio_tach::get_fan_name(void) {
    return(fan_name);
} /* get_fan_name */

void gpio_tach::ready_read(int) {
    QByteArray line;
    tach_value.seek(0);
    line = tach_value.readAll();
    if (line.size() < 1) {
        qDebug() << __func__ << ":" << __LINE__ << " gpio tach read failed..";
        QCoreApplication::quit();
    } else {
        /* Calculate a new RPM value about once per second */
        if (last_calculate_time.elapsed() > 1000) {
            static const unsigned int pulses_per_rev = 2;
            unsigned int pulses_in_this_second;
            unsigned int revs_in_this_second;
            unsigned int revs_in_this_minute;

            pulses_in_this_second = pulses_counted;
            pulses_counted = 0;

            revs_in_this_second = pulses_in_this_second / pulses_per_rev;
            revs_in_this_minute = revs_in_this_second * 60;
            current_value = revs_in_this_minute;
            last_calculate_time.restart();
            emit value_changed(current_value);
        } else {
            ++pulses_counted;
        }
    } /* endif */
} /* ready_read */

