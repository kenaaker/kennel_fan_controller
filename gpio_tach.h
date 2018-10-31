#ifndef GPIO_TACH_H
#define GPIO_TACH_H

#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <QFile>
#include <QTime>

#include "lin_gpio.h"

class gpio_tach : public QObject {
    Q_OBJECT
public:
    gpio_tach();
    gpio_tach(QString tach_name, QString in_fan_name);
    ~gpio_tach();
    int tach_fd(void);              /* Return the file descriptor for the tach interrupt device */
    QString const get_fan_name(void);    /* Return the name of the fan for this object */
signals:
    void value_changed(int on_off);
private:
    lin_gpio *this_gpio;  /* GPIO object for this pin */
    QFile tach_value;               /* GPIO tach value file for tach file. */
    QString fan_name;               /* User name for this fan */
    QSocketNotifier *tach_notifier;
    QTime last_calculate_time;      /* The time we did the last calculation */
    unsigned int current_value;     /* The last calculated RPM value. */
    unsigned int pulses_counted;    /* Number of interrupt pulses counted. */
    void gpio_pullup(const lin_gpio &in_gpio);
    QString const base_model_name();
public slots:
    void ready_read(int);
};

#endif /* GPIO_TACH_H */
