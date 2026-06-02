#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <gpiod.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "bit_bang_loader.h"


#define INIT 23
#define PROG 24
#define DONE 25

#define STRINGIFY(x) #x
#define GPIO_NAME(n) "GPIO" STRINGIFY(n)
#define GPIO_CHIP_PATH "/dev/gpiochip0"
#define BB_INIT_GPIO_NAME GPIO_NAME(INIT)
#define CONSUMER "logi_loader"

typedef struct gpio_line {
    const int line_id;
    const char *name;
    const char *line_name;
    int is_output;
    struct gpiod_line *line;
} gpio_line_type;

#define GPIO_SETTINGS(which, out) {which, STRINGIFY(which), GPIO_NAME(which), out, NULL}

gpio_line_type gpios[] = {
    //{INIT, "INIT", GPIO_NAME(INIT), 0, NULL},
    GPIO_SETTINGS(INIT, 0),
    {DONE, "DONE", GPIO_NAME(DONE), 0, NULL},
    {DONE, "PROG", GPIO_NAME(PROG), 1, NULL},
};

static void setup_gpio(struct gpiod_chip *chip, gpio_line_type *gpio)
{
    char err_buff[1000];
    int result;

    gpio->line = gpiod_chip_find_line(chip, gpio->line_name);
    if (gpio->line == NULL) {
        snprintf(err_buff, sizeof(err_buff), "Can't open GPIO line '%s'", gpio->name);
        perror(err_buff);
        exit(EXIT_FAILURE);
    }

    if (gpio->is_output) {
        result = gpiod_line_request_output(gpio->line, CONSUMER, 0);
    } else {
        result = gpiod_line_request_input(gpio->line, CONSUMER);
    }
    if (result < 0) {
        snprintf(err_buff, sizeof(err_buff), "Can't reserve GPIO line '%s'", gpio->name);
        perror(err_buff);
        exit(EXIT_FAILURE);
    }
}

static void cleanup_gpio(struct gpiod_chip *chip, gpio_line_type *gpio)
{
    (void)gpiod_line_release(gpio->line);
    gpio->line = NULL;

    (void)gpiod_chip_close(chip);
    chip = NULL;
}

static void gpio_set(gpio_line_type *gpio, int value)
{
    char err_buff[1000];
    int result = gpiod_line_set_value(gpio->line, value);
    if (result < 0) {
        snprintf(err_buff, sizeof(err_buff), "Can't set output value of line '%s'", gpio->name);
        perror(err_buff);
        exit(EXIT_FAILURE);

    }
}

static int gpio_get(gpio_line_type *gpio)
{
    char err_buff[1000];
    int result = gpiod_line_get_value(gpio->line);
    if (result < 0) {
        snprintf(err_buff, sizeof(err_buff), "Can't get input value of line '%s'", gpio->name);
        perror(err_buff);
        exit(EXIT_FAILURE);

    }
    return result;
}


void *gpio_map;

// I/O access
volatile unsigned *gpio;
unsigned cfg_save[3] ;

void clear_bb_progb(){
	gpio_set(gpios + PROG, 0);
}
void set_bb_progb(){
	gpio_set(gpios + PROG, 1);
}
char get_bb_done(){
	return (gpio_get(gpios + DONE)) ;
}
char get_bb_init(){
	return (gpio_get(gpios + INIT)) ;
}


static struct gpiod_chip *chip= NULL;

int init_bb_loader(){
    chip = gpiod_chip_open(GPIO_CHIP_PATH);
    if (chip == NULL) {
        perror("Can't open GPIO chip device " GPIO_CHIP_PATH);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < sizeof(gpios)/sizeof(gpio_line_type); i++) {
        setup_gpio(chip, gpios + i);
        printf("Set up GPIO line %i\n", i);
    }
	return 0 ;
}


void close_bb_loader(){
    for (int i = 0; i < sizeof(gpios)/sizeof(gpio_line_type); i++) {
        cleanup_gpio(chip, gpios + i);
        printf("Cleaned up GPIO line %i\n", i);
    }
}


