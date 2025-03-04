#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/slist.h>

#include <zephyr/display/mb_display.h>

/* The micro:bit has a 5x5 LED display. Using (x, y) notation the top-left
 * corner has coordinates (0, 0) and the bottom-right has (4, 4).
 */

#define LED_MAX            4    /* LEDs maximum x and y position */

#define SCROLL_SPEED       800  /* Text scrolling speed */

#define START_SNAKE_LENGTH 2    /* Initial snake length */
#define X_START            2    /* Snake starting x position */
#define MOVEMENT_DELAY     400  /* Time between each movement */

// Define buttons A (0) and B (1).
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET_OR(
                                    DT_ALIAS(sw0), gpios, {0});
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(
                                    DT_ALIAS(sw1), gpios, {0});

static struct gpio_callback button_cb_data;

enum directions {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

struct x_y {
    int x;
    int y;
};

/* Struct representing a snake body segment */
struct snake_segment {
    sys_snode_t node;    /* Node pointing to next node in snake.body list */
    struct x_y position;
};

struct snake {
    sys_slist_t body; /* Single-linked list representing the body segments */
    int length;
};

/* The direction the snake head is going in */
static enum directions head_direction = UP;

static int score = 0;

/* Snake head position */
static struct x_y head_position = {X_START, LED_MAX-START_SNAKE_LENGTH+1};

static struct snake snake;

/* Snake food item position */
static struct x_y food_position;

static void
generate_food_position(void)
{
    struct snake_segment *segment_ptr, *tmp;
    int is_unique_position = 0;

    /* Continuously generate random x-y coordinates for the food item
     * until the position differs from each of the snake body segments.
     */
    while (!is_unique_position) {
        food_position.x = sys_rand16_get() % LED_MAX + 1;
        food_position.y = sys_rand16_get() % LED_MAX + 1;

        SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&snake.body, segment_ptr, tmp, node) {
            if (food_position.x == segment_ptr->position.x &&
                food_position.y == segment_ptr->position.y) {
                    is_unique_position = 0;
                    break;
            }
            else {
                is_unique_position = 1;
            }
        }
    }
}

static void
button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	if (pins & BIT(button0.pin)) {
		printk("A pressed\n");
        head_direction = (head_direction + LEFT) % 4;
	}
    else {
		printk("B pressed\n");
        head_direction = (head_direction + RIGHT) % 4;
	}
}

static void
configure_buttons(void)
{
    int ret;

    /* Check if button is ready (button0.port == button1.port) */
    if (!gpio_is_ready_dt(&button0)) {
        printk("Error: button device %s is not ready\n",
            button0.port->name);
        return;
    }

    /* Configure button0 as input */
    ret = gpio_pin_configure_dt(&button0, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
            ret, button0.port->name, button0.pin);
        return;
    }

    /* Configure button1 as input */
    ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
            ret, button1.port->name, button1.pin);
        return;
    }

    /* Configure interrupt for button0 */
    ret = gpio_pin_interrupt_configure_dt(&button0,
             GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error %d: failed to configure interrupt on %s pin %d\n",
            ret, button0.port->name, button0.pin);
        return;
    }

    /* Configure interrupt for button1 */
    ret = gpio_pin_interrupt_configure_dt(&button1,
             GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error %d: failed to configure interrupt on %s pin %d\n",
            ret, button1.port->name, button1.pin);
        return;
    }

    /* Add callback function button_pressed to run when pressing either button */
    gpio_init_callback(&button_cb_data, button_pressed,
                  BIT(button0.pin) | BIT(button1.pin));
    gpio_add_callback(button0.port, &button_cb_data);
}

static void
game_end(void)
{
    struct mb_display *disp = mb_display_get();

    printk("Score: %d. Snake length: %d.", score, snake.length);

    /* Continuously display the score as a scrolling image if it is at least
     * a two-digit number. Otherwise, display it as a single image.
     */
    if (score >= 10)
        mb_display_print(disp, MB_DISPLAY_MODE_SCROLL | MB_DISPLAY_FLAG_LOOP,
                 SCROLL_SPEED, "%d", score);
    else
        mb_display_print(disp, MB_DISPLAY_MODE_SINGLE, SYS_FOREVER_MS,
                 "%d", score);
}

static void
expand_snake(int x, int y)
{
    struct snake_segment *new_segment = (struct snake_segment*)
                  k_malloc(sizeof(struct snake_segment));

    new_segment->position.x = x;
    new_segment->position.y = y;

    sys_slist_append(&snake.body, &new_segment->node);
    snake.length++;
}

static void
init_snake(void)
{
    int i;

    /* Initialize single-linked list representing the snake body */
    sys_slist_init(&snake.body);
    snake.length = 0;

    /* Generate initial snake body segments with x-y coordinates */
    for (i = head_position.y; i <= LED_MAX; i++)
        expand_snake(X_START, i);
}

int
main(void)
{
    struct mb_display *disp = mb_display_get();

    /* Create initial snake body */
    init_snake();

    struct snake_segment *segment_ptr, *tmp;
    struct snake_segment *head_segment = SYS_SLIST_PEEK_HEAD_CONTAINER(&snake.body,
                                     segment_ptr, node);

    configure_buttons();

    /* Generate initial item position */
    generate_food_position();

    int alive = 1;

    while (1) {
        struct mb_image img = {};
        struct snake_segment *prev_tail_segment = SYS_SLIST_PEEK_TAIL_CONTAINER(&snake.body,
                    segment_ptr, node);
        /* Store the tail position to be used as next tail position if food item is eaten */
        struct x_y prev_tail_position = prev_tail_segment->position;

        /* Generate the LED image with the snake body and food item LED positions */
        SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&snake.body, segment_ptr, tmp, node) {
            if (segment_ptr->position.y == food_position.y) {
                img.row[segment_ptr->position.y] = (img.row[segment_ptr->position.y] | (BIT(segment_ptr->position.x) | BIT(food_position.x)));
            }
            else {
                img.row[food_position.y] = (img.row[food_position.y] | BIT(food_position.x));
                img.row[segment_ptr->position.y]  = (img.row[segment_ptr->position.y] | BIT(segment_ptr->position.x));
            }
        }

        mb_display_image(disp, MB_DISPLAY_MODE_DEFAULT, SYS_FOREVER_MS, &img, 1);

        k_msleep(MOVEMENT_DELAY);

        /* Go through and update each of the body segments excluding the head by
         * setting each of the segment's positions to its preceding segment.
         */
        struct x_y prev_segment_position = head_segment->position;
        SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&snake.body, segment_ptr, tmp, node) {
            if (segment_ptr != head_segment) {
                struct x_y temp_position = segment_ptr->position;

                segment_ptr->position.x = prev_segment_position.x;
                segment_ptr->position.y = prev_segment_position.y;

                prev_segment_position = temp_position;
            }
        }

        /* Update the snake head position based on its direction */
        switch(head_direction) {
            case UP:
                head_segment->position.y--;
                break;
            case RIGHT:
                head_segment->position.x++;
                break;
            case DOWN:
                head_segment->position.y++;
                break;
            case LEFT:
                head_segment->position.x--;
                break;
        }

        /* Game ends if snake head position is out of bounds */
        if (head_segment->position.x < 0 || head_segment->position.x > LED_MAX ||
              head_segment->position.y < 0 || head_segment->position.y > LED_MAX) {
                alive = 0;
        }
        else {
            /* Check for collision between head and body segments */
            SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&snake.body, segment_ptr, tmp, node) {
                if (segment_ptr != head_segment) {
                    /* Game ends if snake head has collided with a body segment */
                    if (head_segment->position.x == segment_ptr->position.x &&
                        head_segment->position.y == segment_ptr->position.y) {
                            alive = 0;
                            break;
                    }
                }
            }
        }

        if (!alive)
            break; // Dead ;)

        /* Check if food is eaten */
        if (head_segment->position.x == food_position.x &&
              head_segment->position.y == food_position.y) {
                /* Use the previous tail position before the last movement for the new segment */
                expand_snake(prev_tail_position.x, prev_tail_position.y);

                score++;
                generate_food_position();
        }
    }

    game_end();
    return 0;
}
