#ifndef LIN_GPIO_H
#define LIN_GPIO_H

#include <QDir>
#include <QFile>
#include <list>

#include "lin_pwm.h"

using namespace std;

#include <string>
#include <list>
#include <initializer_list>
#include <algorithm>

static const string io_output = "out";
static const string io_input = "in";

static const string edge_none = "none";
static const string edge_rising = "rising";
static const string edge_falling = "falling";
static const string edge_both = "both";

class lin_gpio {

public:

    lin_gpio();
    lin_gpio(const string &in_gpio_name);
    ~lin_gpio(void);
    int gpio_set_direction(const string &direction);
    const string gpio_get_direction() const;
    int gpio_set_value(const string &value);
    const string gpio_get_value() const;
    int gpio_set_edge(const string &edge_name);
    const string gpio_get_path() const;
    const string gpio_get_key() const;
    int gpio_get_number() const;

private:
    int gpio_export(const string &in_gpio_name);
    int gpio_number;            /* The GPIO pin number */
    string gpio_key;            /* Key string for the pin */
    QDir gpio_directory;        /* Directory controlling this GPIO */
    string io_direction;        /* "out" or "in" */
    pins_table pin_lookup;
};

#endif /* LIN_GPIO_H */
