#ifndef LIN_GPIO_UTIL_H
#define LIN_GPIO_UTIL_H

using namespace std;
#include <string>
#include <list>
#include <initializer_list>
#include <algorithm>

class pins_table {
public:
    pins_table();
    pins_table(const string &table_name);
    ~pins_table();
    int gpio_by_key(const string &key);
    int gpio_by_name(const string &name);
    string gpio_key_by_name(const string &name);
    string ain_by_key(const string &key);
    string ain_by_name(const string &name);
    int gpio_number(const string &key);
    int adc_ain_number(const string &n);

private:
    void init_pins_table(const string &table_name);
    typedef struct pins_entry {
        string name;
        string key;
        int16_t gpio;
        int16_t pwm_mux_mode;
        int16_t ain;
    } pins_entry_t;
    string pin_table_name;
    list<pins_entry_t> pins;
};

class pwms_table {
public:
    pwms_table();
    pwms_table(const string &table_name);
    ~pwms_table();
    string pwmchip_by_key(const string &key);
    string pwmchip_by_name(const string &name);
    const string get_pwm_table_name(void);
    int pwm_chip_number_by_key(const string &key);
    int pwm_chip_number_by_name(const string &name);
    int pwm_chip_unit_by_key(const string &key);
    int pwm_chip_unit_by_name(const string &name);

private:
    void init_pwms_table(const string pwm_name);
    typedef struct pwms_entry {
        string name;
        string key;
        string pwm_chip_name;
        unsigned char pwm_chip_number;
        int pwm_chip_unit;
    } pwms_entry_t;
    string pwms_table_name;
    list<pwms_entry_t> pwms;
};

#endif // LIN_GPIO_UTIL_H
