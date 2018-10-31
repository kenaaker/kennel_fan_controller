#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QString>
#include <QCoreApplication>
#include <QDebug>
#include <list>
#include <sstream>
#include <iostream>
#include "kennel_fan_controller.h"


#include "lin_gpio.h"

static inline int string_to_int(string s) {
    stringstream ss(s);
    int x;
    ss >> x;
    return x;
}

static inline string int_to_string(int i) {
    stringstream ss;
    ss << i;
    return ss.str();
}

lin_gpio::lin_gpio() {
    gpio_number = 9999;
    gpio_key = "uninit";
    gpio_directory = "";
    io_direction = io_input;
}
lin_gpio::lin_gpio(const string &in_gpio_name) {
    int tentative_gpio_number = gpio_export(in_gpio_name);
    if (tentative_gpio_number < 0) {
        qDebug() << __func__ << ":" << __LINE__ << " GPIO couldn't be allocated. in_gpio_name is " <<
                    QString::fromStdString(in_gpio_name);
        QCoreApplication::quit();
    } else {
        unsigned int wait_count;
        gpio_number = tentative_gpio_number;
        gpio_key = pin_lookup.gpio_key_by_name(in_gpio_name);
        gpio_directory = QDir("/sys/class/gpio/gpio" + QString::number(gpio_number) + "/");
        io_direction = io_input;
        /* Now, wait for the directory to show up. */
        while ((!gpio_directory.exists()) && (wait_count > 0)) {
            nanosleep((const struct timespec[]){{0, 200000000}}, NULL);
            --wait_count;
        } /* endwhile */
        if (wait_count == 0) {
            qDebug() << __func__ << ":" << __LINE__ << "GPIO directory failed to show up, quitting.";
            QCoreApplication::quit();
        }
    } /* endif */
} /* constructor */

lin_gpio::~lin_gpio(void) {
    static const string gpio_unexport_filename = "/sys/class/gpio/unexport";
    // Close the value file descriptor for this GPIO, if one exists
    if (gpio_number >= 0) {
        QFile gpio_unexport(QString::fromStdString(gpio_unexport_filename));
        if (gpio_unexport.open(QIODevice::WriteOnly)) {
            QTextStream unexport_stream(&gpio_unexport);
            unexport_stream << gpio_number;
            gpio_unexport.close();
        } /* endif */
    } /* endif */
} /* destructor */

int lin_gpio::gpio_export(const string &in_gpio_name) {
    static const string gpio_export_filename = "/sys/class/gpio/export";
    int rc = -1;

    gpio_number = pin_lookup.gpio_by_name(in_gpio_name);
    if (gpio_number == -1) {
        cout << " Lookup by name failed for " << in_gpio_name << endl;
        rc = -1;
    } else {
        QFile gpio_export(QString::fromStdString(gpio_export_filename));
        if (!gpio_export.open(QIODevice::WriteOnly)) {
            cout << " Open failed for " << __func__ << " gpio_export_filename is " << gpio_export_filename << endl;
            QCoreApplication::quit();
            rc = -1;
        } else {
            QTextStream export_stream(&gpio_export);
            export_stream << gpio_number;
            gpio_export.close();
            rc = gpio_number;
        } /* endif */
    } /* endif */
    return rc;
} /* export GPIO pin to enable */

int lin_gpio::gpio_set_direction(const string &direction) {
    int rc = -1;
    int wait_count = 100;

    QFile direction_file(gpio_directory.path() + "/direction");
    while ((!direction_file.open(QIODevice::WriteOnly)) && (wait_count > 0)) {
        nanosleep((const struct timespec[]){{0, 200000000}}, NULL);
        --wait_count;
    } /* endwhile */
    if (wait_count == 0) {
        qDebug() << __func__ << ":" << __LINE__ << " Cannot open direction file=" <<
                    direction_file.fileName();
        QCoreApplication::quit();
        rc = -1;
    } else {
        QTextStream out_direction(&direction_file);
        out_direction << QString::fromStdString(direction);
        direction_file.close();
        rc = 1;
    } /* endif */

    return rc;
} /* gpio_set_direction */

const string lin_gpio::gpio_get_direction() const {
    QFile direction_file(gpio_directory.path() + "/direction");
    QString direction_str;
    if (direction_file.open(QIODevice::ReadOnly)) {
        QTextStream in_direction(&direction_file);
        in_direction >> direction_str;
        direction_file.close();
    } /* endif */
    return direction_str.toStdString();
} /* gpio_get_direction */

int lin_gpio::gpio_set_value(const string &value) {
    int rc=-1;

    QFile value_file(gpio_directory.path() + "/value");
    if (!value_file.open(QIODevice::WriteOnly)) {
        rc = -1;
    } else {
        QTextStream out_value(&value_file);
        out_value << QString::fromStdString(value);
        value_file.close();
        rc = 1;
    } /* endif */
    return rc;
} /* gpio_set_value */

const string lin_gpio::gpio_get_value() const {
    QFile value_file(gpio_directory.path() + "/value");
    unsigned char value = '0';
    string value_str;
    if (value_file.open(QIODevice::ReadOnly)) {
        QDataStream in_value(&value_file);
        in_value >> value;
        value_str = value;
        value_file.close();
    } /* endif */
    return value_str;
} /* gpio_get_value */

// Return the file path of this gpio for the value file */
const string lin_gpio::gpio_get_path() const {
    return (gpio_directory.path() + "/value").toStdString();
}

string const lin_gpio::gpio_get_key() const {
    return gpio_key;
}

int lin_gpio::gpio_get_number() const {
    return gpio_number;
} /* gpio_get_path */

int lin_gpio::gpio_set_edge(const string &edge_name) {
    int rc = -1;
    unsigned int wait_count = 100;

    QFile edge_file(gpio_directory.path() + "/edge");
    while ((!edge_file.open(QIODevice::WriteOnly)) && (wait_count > 0)) {
        nanosleep((const struct timespec[]){{0, 200000000}}, NULL);
        --wait_count;
    } /* endwhile */
    if (wait_count == 0) {
        qDebug() << __func__ << ":" << __LINE__ << "GPIO directory failed to show up, quitting.";
        QCoreApplication::quit();
    } else {
        QTextStream out_edge(&edge_file);
        out_edge << QString::fromStdString(edge_name);
        edge_file.close();
        rc = 1;
    } /* endif */
    return rc;
} /* gpio_set_edge */
