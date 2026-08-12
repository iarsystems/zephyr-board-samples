#include "interior_light_switch.h"
#include "zephyr/drivers/gpio.h"

/* Onboard car interior light on/auto/off switch states. */
static struct gpio_dt_spec interior_light_switch[] = { 
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw4), gpios, {0}), /* on   */
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw5), gpios, {0}), /* auto */
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw6), gpios, {0}), /* off  */
};

void
configure_interior_light_switch(void)
{
    int i, ret;
    unsigned int pin_mask_sum = 0;

    static struct gpio_callback switch_cb_data;

    for (i = 0; i < LEN(interior_light_switch); i++) {
    // GPIO_ARRAY_LOOP(interior_light_switch, i) {
        if (!gpio_is_ready_dt(&interior_light_switch[i])) {
            printk("Error: button device %s is not ready\n",
                interior_light_switch[i].port->name);
            return;
        }

        ret = gpio_pin_configure_dt(&interior_light_switch[i], GPIO_INPUT);
        if (ret != 0) {
            printk("Error %d: failed to configure %s pin %d\n",
                ret, interior_light_switch[i].port->name, interior_light_switch[i].pin);
            return;
        }

        ret = gpio_pin_interrupt_configure_dt(&interior_light_switch[i],
                        GPIO_INT_EDGE_TO_ACTIVE);
        if (ret != 0) {
            printk("Error %d: failed to configure interrupt on %s pin %d\n",
                ret, interior_light_switch[i].port->name, interior_light_switch[i].pin);
            return;
        }
        pin_mask_sum |= BIT(interior_light_switch[i].pin);
    }

    // gpio_init_callback(&switch_cb_data, switch_toggled, pin_mask_sum);
    // gpio_add_callback(interior_light_switch[0].port, &switch_cb_data);
}

interior_light_state_typedef
get_interior_light_state(void)
{
    /* Get car interior light switch states */
    int interior_light_on_state = gpio_pin_get_dt(&interior_light_switch[0]);
    int interior_light_auto_state = gpio_pin_get_dt(&interior_light_switch[1]);
    int interior_light_off_state = gpio_pin_get_dt(&interior_light_switch[2]);

    if (interior_light_on_state) {
        return ON_STATE;
    }
    else if (interior_light_auto_state) {
        return AUTO_STATE;
    }
    else if (interior_light_off_state) {
        return OFF_STATE;
    }
}
