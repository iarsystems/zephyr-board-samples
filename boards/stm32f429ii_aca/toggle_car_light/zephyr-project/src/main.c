#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

#include "interior_light_switch.h"

#define SLEEP_TIME_MS	1

/* Onboard car interior light LED. */
static struct gpio_dt_spec car_led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led3), gpios, {0});

/* Onboard car door open/closed state switch. */
static struct gpio_dt_spec open_closed_switch = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw3), gpios, {0});

void
switch_toggled(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    int car_door_state;
    interior_light_state_typedef interior_light_state;

    interior_light_state = get_interior_light_state();

    /* 0 if open, 1 if closed */
    car_door_state = gpio_pin_get_dt(&open_closed_switch);

    switch(interior_light_state) {
        case ON_STATE:
            gpio_pin_set_dt(&car_led, 1);
        case AUTO_STATE:
            if (car_door_state > 0)
                gpio_pin_set_dt(&car_led, 1);
            else
                gpio_pin_set_dt(&car_led, 0);
        case OFF_STATE:
            gpio_pin_set_dt(&car_led, 0);
    }
}

void
configure_button(void)
{
    int ret;

    if (!gpio_is_ready_dt(&open_closed_switch)) {
        printk("Error: button device %s is not ready\n",
            open_closed_switch.port->name);
        return;
    }

    ret = gpio_pin_configure_dt(&open_closed_switch, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
            ret, open_closed_switch.port->name, open_closed_switch.pin);
        return;
    }

    ret = gpio_pin_interrupt_configure_dt(&open_closed_switch,
                    GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error %d: failed to configure interrupt on %s pin %d\n",
            ret, open_closed_switch.port->name, open_closed_switch.pin);
        return;
    }
}

void
configure_led(void)
{
    int ret;
    
    if (car_led.port && !gpio_is_ready_dt(&car_led)) {
        printk("Error %d: LED device %s is not ready; ignoring it\n",
            ret, car_led.port->name);
        car_led.port = NULL;
    }

    if (car_led.port) {
        ret = gpio_pin_configure_dt(&car_led, GPIO_OUTPUT);
        if (ret != 0) {
            printk("Error %d: failed to configure LED device %s pin %d\n",
                ret, car_led.port->name, car_led.pin);
            car_led.port = NULL;
        } else {
            printk("Set up LED at %s pin %d\n", car_led.port->name, car_led.pin);
        }
    }
}

int
main(void)
{
    int car_door_state;
    interior_light_state_typedef interior_light_state;

    configure_button();
    configure_interior_light_switch();
    configure_led();

    gpio_pin_set_dt(&car_led, 0);

    while (1) {
        /* State of the interior light switch. 
         * Possible Values: ON_STATE, AUTO_STATE, OFF_STATE.
         */
        interior_light_state = get_interior_light_state();

        /* 0 if open, 1 if closed */
        car_door_state = gpio_pin_get_dt(&open_closed_switch);

        switch(interior_light_state) {
            case ON_STATE:
                gpio_pin_set_dt(&car_led, 1);
            case AUTO_STATE:
                if (car_door_state > 0)
                    gpio_pin_set_dt(&car_led, 1);
                else
                    gpio_pin_set_dt(&car_led, 0);
            case OFF_STATE:
                gpio_pin_set_dt(&car_led, 0);
        }

        // k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
