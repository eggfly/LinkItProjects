#ifndef GROVE_LED_BAR_H
#define	GROVE_LED_BAR_H

// Work on 8-bit mode
#define GLB_CMDMODE 0x00

#define PIN_DATA	VM_PIN_D0
#define PIN_CLOCK	VM_PIN_D1

typedef struct {
	unsigned char brightness[10];
} led_bar_state_t;

void led_bar_set_state(led_bar_state_t * state);

#endif

