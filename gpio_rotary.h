#ifndef GPIO_ROTARY_H
#define GPIO_ROTARY_H

#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <QFile>
#include <QTime>

#include "lin_gpio.h"

struct state_of_clk_and_dt {
    unsigned int clk_state;
    unsigned int dt_state;
};

class gpio_rotary : public QObject {
    Q_OBJECT
public:
    gpio_rotary();
    gpio_rotary(QString clk_gpio, QString dt_gpio, QString in_dial_name);
    ~gpio_rotary();
    int rotary_fd(void);                  /* Return the file descriptor for the rotary encoder device */
    QString const get_dial_name(void);  /* Return the name of the dial for this object */
    void set_bounds(int lower_bound, int upper_bound);
signals:
    void value_changed(int counter);
private:
    lin_gpio *clk_state_gpio;       /* GPIO object for first pulse pin */
    lin_gpio *dt_state_gpio;        /* GPIO object for second pulse pin */
    QFile clk_gpio_value;           /* first GPIO value file for rotary encoder file. */
    QFile dt_gpio_value;            /* second GPIO value file for rotary encoder file. */
    QString dial_name;              /* User name for this dial */
    int limit_lower_bound;
    int limit_upper_bound;
    QSocketNotifier *clk_notifier;
    QSocketNotifier *dt_notifier;
    int current_value;              /* Current calculated value of rotary, counts by +/- 1. */
    int edges_counted_value;        /* The number of edge transitions counted. updates by multiples of 4. */
    void set_initial_encoder_state(void);
    void calc_current_value(const state_of_clk_and_dt current_table_state);
    unsigned int encoder_current_state_index;

public slots:
    void clk_ready_read(int);
    void dt_ready_read(int);
};

#endif /* GPIO_ROTARY_H */
