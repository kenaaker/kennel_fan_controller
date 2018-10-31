#include "lin_gpio_util.h"
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <iostream>

pwms_table::pwms_table() {
    /* Detect the type of platform, Beaglebone or Raspberry Pi */
    static const string platform_type_filename = "/sys/firmware/devicetree/base/model";
    /* Read the model from the Device tree info. */
    QFile platform_type_file(QString::fromStdString(platform_type_filename));
    if (platform_type_file.open(QIODevice::ReadOnly)) {
        QString platform_type_str;
        QTextStream platform_type_stream(&platform_type_file);
        platform_type_stream >> platform_type_str;
        if (platform_type_str.startsWith("Raspberry")) {
            init_pwms_table(string("pi-zero"));
        } else if (platform_type_str.startsWith("TI")) {
            init_pwms_table(string("bbb"));
        } else {
            qDebug() << __func__ << ":" << __LINE__ <<
                        "Cannot determine platform type, platform_str is " << platform_type_str << ", quitting.";
            pwms_table_name = string("");
            abort();
        } /* endif */
        platform_type_file.close();
    } /* endif */

}

void pwms_table::init_pwms_table(const string table_name) {
    initializer_list<pwms_entry_t> beagle_bone_black_p_list = {
        { "PWM0A", "P9_14", "pwmchip4", 4, 0},
        { "PWM0B", "P9_16", "pwmchip4", 4, 1},
        { "PWM1A", "P9_21", "pwmchip1", 1, 0},
        { "PWM2B", "P9_22", "pwmchip1", 1, 1},
        { "PWM2A", "P8_19", "pwmchip7", 7, 0},
        { "PWM2B", "P8_13", "pwmchip7", 7, 1},
        { "PWM0",  "P9_42", "pwmchip0", 0, 0},
        { "PWM1",  "xx_xx", "pwmchip3", 3, 0},
        { "PWM2",  "P9_28", "pwmchip6", 6, 0}
    };
    initializer_list<pwms_entry_t> pi_zero_p_list = {
        { "PWM0A", "J8_12", "pwmchip0", 0, 0},
        { "PWM0B", "J8_32", "pwmchip0", 0, 1},
        { "PWM1",  "J8_33", "pwmchip0", 0, 0},
        { "PWM1",  "J8_35", "pwmchip0", 0, 1}
    };
    QMap<string, int> pwms_table_map;
    pwms_table_map["bbb"] = 1;
    pwms_table_map["pi-zero"] = 2;

    pwms_table_name = table_name;
    switch (pwms_table_map[pwms_table_name]) {
    case 1: // bbb
        pwms.insert(pwms.end(), beagle_bone_black_p_list.begin(), beagle_bone_black_p_list.end());
        break;
    case 2: // pi-zero
        pwms.insert(pwms.end(), pi_zero_p_list.begin(), pi_zero_p_list.end());
        break;
    default: // error.
        break;
    }
    pwms.insert(pwms.end(), beagle_bone_black_p_list.begin(), beagle_bone_black_p_list.end());
} // pwm_table constructor

pwms_table::~pwms_table() {

} // pwms_table destructor

string pwms_table::pwmchip_by_key(const string &key) {
    list<pwms_entry_t>::iterator p_i;
    p_i = find_if(pwms.begin(), pwms.end(), [&](pwms_entry &p){return p.key == key;});
    if (p_i != pwms.end()) {
        return p_i->pwm_chip_name;
    } else {
        return "";
    } /* endif */
}

string pwms_table::pwmchip_by_name(const string &name) {
    list<pwms_entry_t>::iterator p_i;
    p_i = find_if(pwms.begin(), pwms.end(), [&](pwms_entry &p){return p.name == name;});
    if (p_i != pwms.end()) {
        return p_i->pwm_chip_name;
    } else {
        return "";
    } /* endif */
}

const string pwms_table::get_pwm_table_name(void) {
    return pwms_table_name;
}

int pwms_table::pwm_chip_number_by_key(const string &key) {
    list<pwms_entry_t>::iterator p_i;
    p_i = find_if(pwms.begin(), pwms.end(), [&](pwms_entry &p){return p.key == key;});
    if (p_i != pwms.end()) {
        return p_i->pwm_chip_number;
    } else {
        return -1;
    } /* endif */
}

int pwms_table::pwm_chip_number_by_name(const string &name) {
    list<pwms_entry_t>::iterator p_i;
    p_i = find_if(pwms.begin(), pwms.end(), [&](pwms_entry &p){return p.name == name;});
    if (p_i != pwms.end()) {
        return p_i->pwm_chip_number;
    } else {
        return -1;
    } /* endif */
}

int pwms_table::pwm_chip_unit_by_key(const string &key) {
    list<pwms_entry_t>::iterator p_i;
    p_i = find_if(pwms.begin(), pwms.end(), [&](pwms_entry &p){return p.key == key;});
    if (p_i != pwms.end()) {
        return p_i->pwm_chip_unit;
    } else {
        return -1;
    } /* endif */
}

int pwms_table::pwm_chip_unit_by_name(const string &name) {
    list<pwms_entry_t>::iterator p_i;
    p_i = find_if(pwms.begin(), pwms.end(), [&](pwms_entry &p){return p.name == name;});
    if (p_i != pwms.end()) {
        return p_i->pwm_chip_unit;
    } else {
        return -1;
    } /* endif */
}

pins_table::pins_table(void) {
    /* Detect the type of platform, Beaglebone or Raspberry Pi */
    static const string platform_type_filename = "/sys/firmware/devicetree/base/model";
    /* Read the model from the Device tree info. */
    QFile platform_type_file(QString::fromStdString(platform_type_filename));
    if (platform_type_file.open(QIODevice::ReadOnly)) {
        QString platform_type_str;
        QTextStream platform_type_stream(&platform_type_file);
        platform_type_stream >> platform_type_str;
        if (platform_type_str.startsWith("Raspberry")) {
            init_pins_table("pi-zero");
        } else if (platform_type_str.startsWith("TI")) {
            init_pins_table(string("bbb"));
        } else {
            qDebug() << __func__ << ":" << __LINE__ <<
                        "Cannot determine platform type, platform_str is " << platform_type_str << ", quitting.";
            pin_table_name = string("");
            abort();
        } /* endif */
        platform_type_file.close();
    } /* endif */
}

pins_table::pins_table(const string &table_name) {
    this->init_pins_table(table_name);
}

void pins_table::init_pins_table(const string &table_name) {
    initializer_list<pins_entry_t> beagle_bone_black_p_list = {
        { "GPIO_38", "P8_03", 38, -1, -1},
        { "GPIO_39", "P8_04", 39, -1, -1},
        { "GPIO_34", "P8_05", 34, -1, -1},
        { "GPIO_35", "P8_06", 35, -1, -1},
        { "GPIO_66", "P8_07", 66, -1, -1},
        { "GPIO_67", "P8_08", 67, -1, -1},
        { "GPIO_69", "P8_09", 69, -1, -1},
        { "GPIO_68", "P8_10", 68, -1, -1},
        { "GPIO_45", "P8_11", 45, -1, -1},
        { "GPIO_44", "P8_12", 44, -1, -1},
        { "GPIO_23", "P8_13", 23, -1, -1},
        { "GPIO_47", "P8_15", 47, -1, -1},
        { "GPIO_46", "P8_16", 46, -1, -1},
        { "GPIO_48", "P8_18", 65, -1, -1},
        { "GPIO_22", "P8_19", 22, -1, -1},
        { "GPIO_63", "P8_20", 63, -1, -1},
        { "GPIO_62", "P8_21", 62, -1, -1},
        { "GPIO_37", "P8_22", 37, -1, -1},
        { "GPIO_36", "P8_23", 36, -1, -1},
        { "GPIO_33", "P8_24", 33, -1, -1},
        { "GPIO_32", "P8_25", 32, -1, -1},
        { "GPIO_86", "P8_27", 86, -1, -1},
        { "GPIO_88", "P8_28", 88, -1, -1},
        { "GPIO_87", "P8_29", 87, -1, -1},
        { "GPIO_30", "P8_30", 89, -1, -1},
        { "GPIO_10", "P8_31", 10, -1, -1},
        { "GPIO_11", "P8_32", 11, -1, -1},
        { "GPIO_09", "P8_33",  9, -1, -1},
        { "GPIO_81", "P8_34", 81, 2, -1},
        { "GPIO_08", "P8_35",  8, -1, -1},
        { "GPIO_80", "P8_36", 80, 2, -1},
        { "GPIO_78", "P8_37", 78, -1, -1},
        { "GPIO_79", "P8_38", 79, -1, -1},
        { "GPIO_76", "P8_39", 76, -1, -1},
        { "GPIO_77", "P8_40", 77, -1, -1},
        { "GPIO_74", "P8_41", 74, -1, -1},
        { "GPIO_75", "P8_42", 75, -1, -1},
        { "GPIO_72", "P8_43", 72, -1, -1},
        { "GPIO_73", "P8_44", 73, -1, -1},
        { "GPIO_70", "P8_45", 70, 3, -1},
        { "GPIO_71", "P8_46", 71, 3, -1},
        { "GPIO_30", "P9_11", 30, -1, -1},
        { "GPIO_60", "P9_12", 60, -1, -1},
        { "GPIO_31", "P9_13", 31, -1, -1},
        { "GPIO_50", "P9_14", 50, -1, -1},
        { "GPIO_48", "P9_15", 48, -1, -1},
        { "GPIO_51", "P9_16", 51, -1, -1},
        { "GPIO_05", "P9_17",  5, -1, -1},
        { "GPIO_04", "P9_18",  4, -1, -1},
        { "GPIO_13", "P9_19", 13, -1, -1},
        { "GPIO_12", "P9_20", 12, -1, -1},
        { "GPIO_03", "P9_21",  3, 3, -1},
        { "GPIO_02", "P9_22",  2, 3, -1},
        { "GPIO_49", "P9_23", 49, -1, -1},
        { "GPIO_15", "P9_24", 15, -1, -1},
        { "GPIO_117", "P9_25", 117, -1, -1},
        { "GPIO_14", "P9_26", 14, -1, -1},
        { "GPIO_115", "P9_27", 115, -1, -1},
        { "GPIO_113", "P9_28", 113, 4, -1},
        { "GPIO_111", "P9_29", 111, 1, -1},
        { "GPIO_110", "P9_31", 110, 1, -1},
        { "GPIO_20", "P9_41", 20, -1, -1},
        { "GPIO_07", "P9_42", 7, 0, -1},
        { "GPIO_116", "P9_91", 116, 0, -1},
        { "GPIO_114", "P9_92", 114, 0, -1},
    };
    initializer_list<pins_entry_t> pi_zero_p_list = {
        { "GPIO_01", "J8_28", 1, -1, -1},  /* BCM  1 */
        { "GPIO_02", "J8_03", 2, -1, -1},  /* BCM  2 SDA */
        { "GPIO_03", "J8_05", 3, -1, -1},  /* BCM  3 SLC */
        { "GPIO_04", "J8_07", 4, -1, -1},  /* BCM  4 OE */
        { "GPIO_05", "J8_29", 5, -1, -1},  /* BCM  5 */
        { "GPIO_06", "J8_31", 6, -1, -1},  /* BCM  6 */
        { "GPIO_07", "J8_26", 7, -1, -1},  /* BCM  7 CE1 */
        { "GPIO_08", "J8_24", 8, -1, -1},  /* BCM  8 CD0 */
        { "GPIO_09", "J8_21", 9, -1, -1},  /* BCM  9 MISO */
        { "GPIO_10", "J8_19", 10, -1, -1}, /* BCM 10 MOSI */
        { "GPIO_11", "J8_23", 11, -1, -1}, /* BCM 11 SCLK */
        { "GPIO_12", "J8_32", 12, -1, -1}, /* BCM 12 PWM0 */
        { "GPIO_13", "J8_33", 13, -1, -1}, /* BCM 13 PWM1 */
        { "GPIO_14", "J8_08", 14, -1, -1}, /* BCM 14 TXD */
        { "GPIO_15", "J8_10", 15, -1, -1}, /* BCM 15 RXD */
        { "GPIO_16", "J8_36", 16, -1, -1}, /* BCM 16 */
        { "GPIO_17", "J8_11", 17, 57, 58}, /* BCM 17 */
        { "GPIO_18", "J8_12", 18, -1, -1}, /* BCM 18 PWM0 */
        { "GPIO_19", "J8_35", 19, -1, -1}, /* BCM 19 MISO */
        { "GPIO_20", "J8_38", 20, -1, -1}, /* BCM 20 MOSI */
        { "GPIO_21", "J8_40", 21, -1, -1}, /* BCM 21 SCLK */
        { "GPIO_22", "J8_15", 22, -1, -1}, /* BCM 22 */
        { "GPIO_23", "J8_16", 23, -1, -1}, /* BCM 23 */
        { "GPIO_24", "J8_18", 24, -1, -1}, /* BCM 24 */
        { "GPIO_25", "J8_22", 25, -1, -1}, /* BCM 25 */
        { "GPIO_26", "J8_37", 26, -1, -1}, /* BCM 26 */
        { "GPIO_27", "J8_13", 27, -1, -1}, /* BCM 27 */
        /* While these are defined, there is no pin on a connector for them */
        { "GPIO_28", "undefined", 28, -1, -1}, /* BCM 28 */
        { "GPIO_29", "undefined", 29, -1, -1}, /* BCM 29 */
        { "GPIO_30", "undefined", 30, -1, -1}, /* BCM 30 */
        { "GPIO_31", "undefined", 31, -1, -1}, /* BCM 31 */
        { "GPIO_32", "undefined", 32, -1, -1}, /* BCM 32 */
        { "GPIO_33", "undefined", 33, -1, -1}, /* BCM 33 */
        { "GPIO_34", "undefined", 34, -1, -1}, /* BCM 34 */
        { "GPIO_35", "undefined", 35, -1, -1}, /* BCM 35 */
        { "GPIO_36", "undefined", 36, -1, -1}, /* BCM 36 */
        { "GPIO_37", "undefined", 37, -1, -1}, /* BCM 37 */
        { "GPIO_38", "undefined", 38, -1, -1}, /* BCM 38 */
        { "GPIO_39", "undefined", 39, -1, -1}, /* BCM 39 */
        { "GPIO_40", "undefined", 40, -1, -1}, /* BCM 40 */
        { "GPIO_41", "undefined", 41, -1, -1}, /* BCM 41 */
        { "GPIO_42", "undefined", 42, -1, -1}, /* BCM 42 */
        { "GPIO_43", "undefined", 43, -1, -1}, /* BCM 43 */
        { "GPIO_44", "undefined", 44, -1, -1}, /* BCM 44 */
        { "GPIO_45", "undefined", 44, -1, -1}, /* BCM 45 */
        { "GPIO_46", "undefined", 46, -1, -1}, /* BCM 46 */
        { "GPIO_47", "undefined", 47, -1, -1}, /* BCM 47 */
        { "GPIO_48", "undefined", 48, -1, -1}, /* BCM 48 */
        { "GPIO_49", "undefined", 49, -1, -1}, /* BCM 49 */
        { "GPIO_50", "undefined", 50, -1, -1}, /* BCM 50 */
        { "GPIO_51", "undefined", 51, -1, -1}, /* BCM 51 */
        { "GPIO_52", "undefined", 52, -1, -1}, /* BCM 52 */
        { "GPIO_53", "undefined", 53, -1, -1}, /* BCM 53 */
        { "GPIO_54", "undefined", 54, -1, -1}  /* BCM 54 */
    };
    QMap<string, int> pin_table_map;
    pin_table_map["bbb"] = 1;
    pin_table_map["pi-zero"] = 2;

    switch (pin_table_map[table_name]) {
    case 1: // bbb
        pin_table_name = table_name;
        pins.insert(pins.end(), beagle_bone_black_p_list.begin(), beagle_bone_black_p_list.end());
        break;
    case 2: // pi-zero
        pin_table_name = table_name;
        pins.insert(pins.end(), pi_zero_p_list.begin(), pi_zero_p_list.end());
        break;
    default: // error.
        abort();
        break;
    }
} // pins_table common init function.

pins_table::~pins_table() {

} // pins_table destructor

int pins_table::gpio_by_key(const string &key) {
    list<pins_entry_t>::iterator p_i;
    p_i = find_if(pins.begin(), pins.end(), [&](pins_entry &p){return p.key == key;});
    if (p_i != pins.end()) {
        return p_i->gpio;
    } else {
        return -1;
    } /* endif */
}

int pins_table::gpio_by_name(const string &name) {
    list<pins_entry_t>::iterator p_i;
    p_i = find_if(pins.begin(), pins.end(), [&](pins_entry &p){return p.name == name;});
    if (p_i != pins.end()) {
        return p_i->gpio;
    } else {
        return -1;
    } /* endif */
}

string pins_table::gpio_key_by_name(const string &name) {
    list<pins_entry_t>::iterator p_i;
    p_i = find_if(pins.begin(), pins.end(), [&](pins_entry &p){return p.name == name;});
    if (p_i != pins.end()) {
        return p_i->key;
    } else {
        return "";
    } /* endif */
}

string pins_table::ain_by_key(const string &key) {
    list<pins_entry_t>::iterator p_i;
    p_i = find_if(pins.begin(), pins.end(), [&](pins_entry &p){return p.key == key;});
    if (p_i != pins.end()) {
        return p_i->name;
    } else {
        return "";
    } /* endif */
}

string pins_table::ain_by_name(const string &name) {
    list<pins_entry_t>::iterator p_i;
    p_i = find_if(pins.begin(), pins.end(), [&](pins_entry &p){return p.name == name;});
    if (p_i != pins.end()) {
        return p_i->name;
    } else {
        return "";
    } /* endif */
}

