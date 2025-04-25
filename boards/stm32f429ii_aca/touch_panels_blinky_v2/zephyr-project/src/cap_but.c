#include <zephyr/kernel.h>
#include "zephyr/drivers/gpio.h"
#include <zephyr/device.h>
#include "zephyr/drivers/gpio.h"

#include <inttypes.h>
#include <stdint.h>

#include "cap_but.h"
#include "zephyr/sys/printk.h"

void
cap_but_discharge(cap_button_typedef cap_but)
{
    int ret;

    ret = gpio_pin_configure_dt(&cap_buttons[cap_but], GPIO_DISCONNECTED);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
            ret, cap_buttons[cap_but].port->name, cap_buttons[cap_but].pin);
        return;
    }
}

/* Function that prepares the given capacitive
 * sensor button for measurement.
 */
void
cap_but_prepare(cap_button_typedef cap_but)
{
    int ret;
    
    if (!gpio_is_ready_dt(&cap_buttons[cap_but])) {
        printk("Error: button device %s is not ready\n",
            cap_buttons[cap_but].port->name);
        return;
    }

    /* Configure the given capactive button's pin as an input */
    ret = gpio_pin_configure_dt(&cap_buttons[cap_but], GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
            ret, cap_buttons[cap_but].port->name, cap_buttons[cap_but].pin);
        return;
    }

    // gpio_remove_callback(cap_buttons[cap_but].port, &cap_but_cb_data);
    // gpio_init_callback( &cap_but_cb_data, cap_but_pressed,
    //             BIT(cap_buttons[cap_but].pin) );
    // gpio_add_callback(cap_buttons[cap_but].port, &cap_but_cb_data);

    // gpio_init_callback( &cap_but_cb_data, cap_but_pressed,
    //         BIT(cap_buttons[0].pin) | BIT(cap_buttons[1].pin) );
    // gpio_add_callback(cap_buttons[0].port, &cap_but_cb_data);
    
    /* Configure the opposite capacitive button's pin as an output */
    switch (cap_but) {
        case CAP_BUT1:
            ret = gpio_pin_configure_dt(&cap_buttons[CAP_BUT2], GPIO_OUTPUT);
            if (ret != 0) {
                printk("Error %d: failed to configure %s pin %d\n",
                    ret, cap_buttons[CAP_BUT2].port->name, cap_buttons[CAP_BUT2].pin);
            }
            break;

        case CAP_BUT2:
            ret = gpio_pin_configure_dt(&cap_buttons[CAP_BUT1], GPIO_OUTPUT);
            if (ret != 0) {
                printk("Error %d: failed to configure %s pin %d\n",
                    ret, cap_buttons[CAP_BUT1].port->name, cap_buttons[CAP_BUT1].pin);
            }
            break;
    }
}

// void
// cap_buttons_setup(void)
// {
//     int ret;

//     if (!gpio_is_ready_dt(&cap_buttons[CAP_BUT1])) {
//         printk("Error: button device %s is not ready\n",
//             cap_buttons[CAP_BUT1].port->name);
//         return;
//     }

//     ret = gpio_pin_configure_dt(&cap_buttons[CAP_BUT1], GPIO_INPUT);
//     if (ret != 0) {
//         printk("Error %d: failed to configure %s pin %d\n",
//             ret, cap_buttons[CAP_BUT1].port->name, cap_buttons[CAP_BUT1].pin);
//         return;
//     }

//     ret = gpio_pin_configure_dt(&cap_buttons[CAP_BUT2], GPIO_INPUT);
//     if (ret != 0) {
//         printk("Error %d: failed to configure %s pin %d\n",
//             ret, cap_buttons[CAP_BUT2].port->name, cap_buttons[CAP_BUT2].pin);
//         return;
//     }

//     gpio_init_callback( &cap_but_cb_data, cap_but_pressed,
//                 BIT(cap_buttons[CAP_BUT1].pin) | BIT(cap_buttons[CAP_BUT2].pin) );
//     gpio_add_callback(cap_buttons[CAP_BUT1].port, &cap_but_cb_data);
// }

void
cap_but_charge(cap_button_typedef cap_but) {
    switch(cap_but) {
        case CAP_BUT1:
            gpio_pin_set_dt(&cap_buttons[CAP_BUT2], 1);
            break;
        case CAP_BUT2:
            gpio_pin_set_dt(&cap_buttons[CAP_BUT1], 1);
            break;
    }
}

/* Setup capactive button interrupts */
void
cap_but_setup_interrupt(cap_button_typedef cap_but)
{
    int ret;

    // IRQ_DIRECT_CONNECT(6, 1, my_isr, 0);
    // IRQ_DIRECT_CONNECT(7, 1, my_isr, 0);

    ret = gpio_pin_interrupt_configure_dt(&cap_buttons[cap_but],
                    GPIO_INT_EDGE_RISING);
                    // GPIO_INT_EDGE_BOTH);
    if (ret != 0) {
        printk("Error %d: failed to configure interrupt on %s pin %d\n",
            ret, cap_buttons[cap_but].port->name, cap_buttons[cap_but].pin);
        return;
    }
    else {
        printk("Set up interrupt on %s pin %d", cap_buttons[cap_but].port->name, cap_buttons[cap_but].pin);
    }
}
