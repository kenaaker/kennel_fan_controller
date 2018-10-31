
#include <string>
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include "gpio_rotary.h"

#define ENCODER_STATES 4

static struct encoder_table {
    struct state_of_clk_and_dt table_state[ENCODER_STATES];
} encoder_table = { {
    { 1, 1 }, /* 0 */
    { 0, 1 }, /* 1 */
    { 0, 0 }, /* 2 */
    { 1, 0 }  /* 3 */
} };

static struct state_of_clk_and_dt last_clk_and_dt = { 1, 1 };

void gpio_rotary::set_initial_encoder_state(void) {
    unsigned int clk_state;
    QByteArray clk_line;
    unsigned int dt_state;
    QByteArray dt_line;

    /* Read the clk_state and dt_state from the hardware */
    clk_gpio_value.seek(0);
    clk_line = clk_gpio_value.readAll();
    if (clk_line.size() < 1) {
        qDebug() << __func__ << ":" << __LINE__ << " gpio rotary read failed.." ;
    } else {
        if (clk_line[0] == '1') {
            clk_state = 1;
        } else {
            if (clk_line[0] == '0') {
                clk_state = 0;
            } else {
                qDebug() << __func__ << ":" << __LINE__ << " Don't know what's going on. Quit.";
                abort();
            }
        }
        dt_gpio_value.seek(0);
        dt_line = dt_gpio_value.read(2);
        if (dt_line[0] == '1') {
            dt_state = 1;
        } else {
            if (dt_line[0] == '0') {
                dt_state = 0;
            } else {
                qDebug() << __func__ << ":" << __LINE__ << " Don't know what's going on. Quit.";
                abort();
            }
        }
    }


    for (int i = 0; i < ENCODER_STATES; ++i) {
        if ((encoder_table.table_state[i].clk_state  == clk_state) &&
                (encoder_table.table_state[i].dt_state == dt_state)) {
            encoder_current_state_index = i;
            break;
        }
    }
}

gpio_rotary::gpio_rotary() {

    clk_state_gpio = nullptr;
    dt_state_gpio = nullptr;
    edges_counted_value = 0;
}

gpio_rotary::gpio_rotary(QString clk_gpio, QString dt_gpio, QString in_dial_name) {

    clk_state_gpio = new lin_gpio(clk_gpio.toStdString());
    if (!clk_state_gpio) {
        qDebug() << __func__ << ":" << __LINE__ << " GPIO allocation failed.";
        QCoreApplication::quit();
    } else {
        dt_state_gpio = new lin_gpio(dt_gpio.toStdString());
        if (!dt_state_gpio) {
            qDebug() << __func__ << ":" << __LINE__ << " GPIO allocation failed.";
            QCoreApplication::quit();
        } else {
            QString clk_gpio_value_path;
            dial_name = in_dial_name;
            clk_gpio_value_path = QString::fromStdString(clk_state_gpio->gpio_get_path());
            clk_gpio_value.setFileName(clk_gpio_value_path);
            clk_state_gpio->gpio_set_direction("in");
            clk_state_gpio->gpio_set_edge("both");         /* Interrupt on rising or falling edge */
            clk_gpio_value.open(QFile::ReadOnly);
            clk_notifier = new QSocketNotifier(clk_gpio_value.handle(), QSocketNotifier::Exception);
            clk_notifier->setEnabled(true);
            connect(clk_notifier, SIGNAL(activated(int)), this, SLOT(clk_ready_read(int)));

            QString dt_gpio_value_path;
            dt_gpio_value_path = QString::fromStdString(dt_state_gpio->gpio_get_path());
            dt_gpio_value.setFileName(dt_gpio_value_path);
            dt_state_gpio->gpio_set_direction("in");
            dt_state_gpio->gpio_set_edge("both");         /* Interrupt on rising or falling edge */
            dt_gpio_value.open(QFile::ReadOnly);
            dt_notifier = new QSocketNotifier(dt_gpio_value.handle(), QSocketNotifier::Exception);
            dt_notifier->setEnabled(true);
            connect(dt_notifier, SIGNAL(activated(int)), this, SLOT(dt_ready_read(int)));

            set_initial_encoder_state();
            edges_counted_value = 0;
            limit_lower_bound = 0;
            limit_upper_bound = 100;
        } /* endif */
    } /* endif */
}

gpio_rotary::~gpio_rotary() {
    clk_gpio_value.close();
    clk_state_gpio->gpio_set_edge("none");             /* Don't generate interrupts. */
    dt_gpio_value.close();
    dt_state_gpio->gpio_set_edge("none");
    delete clk_notifier;
    delete dt_notifier;
    delete clk_state_gpio;
    delete dt_state_gpio;
}

QString const gpio_rotary::get_dial_name(void) {
    return(dial_name);
} /* get_dial_name */

void gpio_rotary::set_bounds(int lower_bound, int upper_bound) {
    limit_lower_bound = lower_bound;
    limit_upper_bound = upper_bound;
}

static unsigned int make_next_entry(unsigned int encoder_current_state_index) {
    unsigned int ret_value;

    ret_value = (++encoder_current_state_index) & 0x03;
    return ret_value;
}

static unsigned int make_previous_entry(unsigned int encoder_current_state_index) {
    unsigned int ret_value;

    ret_value = (encoder_current_state_index + 3) & 0x03;
    return ret_value;
}


void gpio_rotary::calc_current_value(const struct state_of_clk_and_dt current_table_state) {
    int old_current_value = current_value;
    unsigned int previous_table_entry;
    unsigned int next_table_entry;

    previous_table_entry = make_previous_entry(encoder_current_state_index);
    next_table_entry = make_next_entry(encoder_current_state_index);
    if ((current_table_state.clk_state == encoder_table.table_state[previous_table_entry].clk_state) &&
        (current_table_state.dt_state == encoder_table.table_state[previous_table_entry].dt_state)) {
        edges_counted_value = std::max(edges_counted_value - 1, (limit_lower_bound * 4));
        encoder_current_state_index = previous_table_entry;
    } else if ((current_table_state.clk_state == encoder_table.table_state[next_table_entry].clk_state) &&
               (current_table_state.dt_state == encoder_table.table_state[next_table_entry].dt_state))  {
        edges_counted_value = std::min(edges_counted_value + 1, (limit_upper_bound * 4));
        encoder_current_state_index = next_table_entry;
    } else {
        //qDebug() << __func__ << ":" << __LINE__ << "clk_state=" << current_table_state.clk_state <<
        //                                           "dt_state=" << current_table_state.dt_state;
        //qDebug() << __func__ << ":" << __LINE__ << " encoder_current_state_index = " << encoder_current_state_index <<
        //                                           " Didn't match a table entry, ignoring.";
        //            " Examined previous entry=" << previous_table_entry <<
        //            " previous clk_state=" << encoder_table.table_state[previous_table_entry].clk_state <<
        //            " previous dt_state=" << encoder_table.table_state[previous_table_entry].dt_state <<
        //            " and next entry=" << next_table_entry <<
        //            " next clk_state=" << encoder_table.table_state[next_table_entry].clk_state <<
        //            " next dt_state=" << encoder_table.table_state[next_table_entry].dt_state;
    }
    current_value = std::min(edges_counted_value / 4, limit_upper_bound);

    if (old_current_value != current_value) {
        // qDebug() << __func__ << ":" << __LINE__ << "dial" << dial_name << " current_value=" << current_value;
        //                        " old_current_value=" << old_current_value << " current_value=" << current_value;
        emit value_changed(current_value);
    }

}

void gpio_rotary::clk_ready_read(int) {
    QByteArray clk_line;          /* Value read from encoder clk/dt GPIO value */
    struct state_of_clk_and_dt current_table_state;

    clk_gpio_value.seek(0);
    clk_line = clk_gpio_value.read(2);
    if (clk_line.size() < 1) {
        qDebug() << __func__ << ":" << __LINE__ << " gpio rotary read failed.." ;
    } else {
        if (clk_line[0] == '1') {
            current_table_state.clk_state = 1;
        } else {
            if (clk_line[0] == '0') {
                current_table_state.clk_state = 0;
            }
        }
        last_clk_and_dt.clk_state = current_table_state.clk_state;
        current_table_state.dt_state = last_clk_and_dt.dt_state;
        calc_current_value(current_table_state);
    } /* endif */
} /* clk_ready_read */

void gpio_rotary::dt_ready_read(int) {
    QByteArray dt_line;          /* Value read from encoder clk/dt GPIO value */
    struct state_of_clk_and_dt current_table_state;

    dt_gpio_value.seek(0);
    dt_line = dt_gpio_value.read(2);
    if (dt_line.size() < 1) {
        qDebug() << __func__ << ":" << __LINE__ << " gpio rotary read failed.." ;
    } else {
        if (dt_line[0] == '1') {
            current_table_state.dt_state = 1;
        } else {
            if (dt_line[0] == '0') {
                current_table_state.dt_state = 0;
            }
        }
        last_clk_and_dt.dt_state = current_table_state.dt_state;
        current_table_state.clk_state = last_clk_and_dt.clk_state;
        calc_current_value(current_table_state);
    } /* endif */
} /* dt_ready_read */
