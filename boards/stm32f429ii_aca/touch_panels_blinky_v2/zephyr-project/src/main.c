#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

#include <inttypes.h>
#include <stdint.h>

#include "cap_but.h"

#define SLEEP_TIME_MS	1

#define LEN(arr) (sizeof(arr) / sizeof(arr)[0])

/* Loop through a list of related GPIOs */
#define GPIO_LIST_LOOP(list_name) for (i = 0; i < LEN(list_name); i++)

#define CAP_BUT1_BIT    (1 << CAP_BUT1)
#define CAP_BUT2_BIT    (1 << CAP_BUT2)

/* cap buttons relative threshold value (lower - increase sensitivity) */
#define CAP_BUT_THRESHOLD 20

struct gpio_dt_spec cap_buttons[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw7), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw8), gpios, {0}),
};

static struct gpio_dt_spec traffic_leds[] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0}), /* red LED */
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios, {0}), /* yellow LED */
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios, {0}), /* green LED */
};

static struct gpio_callback cap_but_cb_data;

typedef enum {
    TRAFFIC_RED_LED = 0,
    TRAFFIC_YELLOW_LED,
    TRAFFIC_GREEN_LED
} traffic_leds_typedef;

// struct k_timer cap_buttons_timer;
// volatile int64_t timer1_int, timer2_int;
volatile uint16_t timer1_int = 0, timer2_int = 0;

void
cap_but_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    // if (timer1_int == 0) {
    if (pins & BIT(cap_buttons[CAP_BUT1].pin)) {
        // timer1_int = k_timer_status_get(&cap_buttons_timer);
        timer1_int = k_uptime_get();
        printk("Cap_button 1 pressed lolllllllllllllll\n");
    }
    // }

    // if (timer2_int == 0) {
    if (pins & BIT(cap_buttons[CAP_BUT2].pin)) {
        timer2_int = k_uptime_get();
        printk("Cap_button 2 pressed lolllllllllllllll\n");
    }
    // }
}

void
configure_led(traffic_leds_typedef traffic_led)
{
    int ret;

    if (traffic_leds[traffic_led].port && !gpio_is_ready_dt(&traffic_leds[traffic_led])) {
        printk("Error %d: LED device %s is not ready; ignoring it\n",
            ret, traffic_leds[traffic_led].port->name);
        traffic_leds[traffic_led].port = NULL;
    }

    ret = gpio_pin_configure_dt(&traffic_leds[traffic_led], GPIO_OUTPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure LED device %s pin %d\n",
            ret, traffic_leds[traffic_led].port->name, traffic_leds[traffic_led].pin);
        traffic_leds[traffic_led].port = NULL;
    } else {
        printk("Set up LED at %s pin %d\n", traffic_leds[traffic_led].port->name, traffic_leds[traffic_led].pin);
    }
}

void
cap_buttons_looophandler(uint32_t *cap_buttons_bit)
{
    static cap_buttons_state_typedef state;
    // static int64_t uptime_hold;
    static uint16_t uptime_hold;
    // static int cap_but1_zero = 0x7FFF, cap_but2_zero = 0x7FFF;
    static uint16_t cap_but1_zero = 0x7FFF, cap_but2_zero = 0x7FFF;
    // int64_t cap_but1_value, cap_but2_value;
    uint16_t cap_but1_value, cap_but2_value;

    switch(state) {
        /* Discharges the first capacitive button */
        case cap_but1_discharge:
            // cap_but_discharge(CAP_BUT1);
            state = cap_but1_prepare_and_start;
            break;

        /* Configures GPIOs needed for measurement, and start charging */
        case cap_but1_prepare_and_start:
            timer1_int = 0;
            cap_but_prepare(CAP_BUT1);
            uptime_hold = k_uptime_get();
            cap_but_charge(CAP_BUT1);
            state = cap_but1_charging;
            break;
        
        /* Charging state, looks for charging completion */
        case cap_but1_charging:
            if (timer1_int) {
                /* Calculates actual timer cycles needed for charging */
                cap_but1_value = timer1_int - uptime_hold;
                if ( cap_but1_value > ( cap_but1_zero + ( cap_but1_zero * CAP_BUT_THRESHOLD / 100 ) ) ) {
                    *cap_buttons_bit |= CAP_BUT1_BIT;
                }
                else {
                    *cap_buttons_bit &= ~CAP_BUT1_BIT;
                }

                /* Updates calibration value if needed, assumes that not touched button have lower charging time */
                if (cap_but1_zero > cap_but1_value)
                    cap_but1_zero = cap_but1_value;
                state = cap_but2_discharge;
            }
            break;

        case cap_but2_discharge:
            // cap_but_discharge(CAP_BUT2);
            state = cap_but2_prepare_and_start;
            break;

        case cap_but2_prepare_and_start:
            timer2_int = 0;
            cap_but_prepare(CAP_BUT2);
            uptime_hold = k_uptime_get();
            cap_but_charge(CAP_BUT2);
            state = cap_but2_charging;
            break;

        case cap_but2_charging:
            if (timer2_int) {
                /* Calculates actual timer cycles needed for charging */
                cap_but2_value = timer2_int - uptime_hold;
                if ( cap_but2_value > ( cap_but2_zero + ( cap_but2_zero * CAP_BUT_THRESHOLD / 100 ) ) ) {
                    *cap_buttons_bit |= CAP_BUT2_BIT;
                }
                else {
                    *cap_buttons_bit &= ~CAP_BUT2_BIT;
                }

                /* Updates calibration value if needed, assumes that not touched button have lower charging time */
                if( cap_but2_zero > cap_but2_value )
                    cap_but2_zero = cap_but2_value;
                state = cap_but1_discharge;
            }
            break;
    }
}

int
main(void)
{
    int val, i;
    uint32_t cap_buttons_bit;

    configure_led(TRAFFIC_RED_LED);
    configure_led(TRAFFIC_GREEN_LED);

    /* Turn off LEDs */
    gpio_pin_set_dt(&traffic_leds[TRAFFIC_RED_LED],   0);
    gpio_pin_set_dt(&traffic_leds[TRAFFIC_GREEN_LED], 0);

    cap_but_setup_interrupt(CAP_BUT1);
    cap_but_setup_interrupt(CAP_BUT2);

    gpio_init_callback( &cap_but_cb_data, cap_but_pressed,
            BIT(cap_buttons[0].pin) | BIT(cap_buttons[1].pin) );
    gpio_add_callback(cap_buttons[0].port, &cap_but_cb_data);

    while (1) {
        cap_buttons_looophandler(&cap_buttons_bit);

        if (cap_buttons_bit & CAP_BUT1_BIT)
            gpio_pin_set_dt(&traffic_leds[TRAFFIC_GREEN_LED], 1);
        else
            gpio_pin_set_dt(&traffic_leds[TRAFFIC_GREEN_LED], 0);

        if (cap_buttons_bit & CAP_BUT2_BIT)
            gpio_pin_set_dt(&traffic_leds[TRAFFIC_RED_LED], 1);
        else
            gpio_pin_set_dt(&traffic_leds[TRAFFIC_RED_LED], 0);

        // k_msleep(SLEEP_TIME_MS);
    }

    //Onårbar xdxdxdxd
    return 0;
}
