#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "timer.h"

extern volatile unsigned int g_ticks;   // défini dans interrupts.c

// 100 ticks = 1 seconde (car 10 ms par tick)
static void attendre_secondes(int secondes) {
    unsigned int start  = g_ticks;
    unsigned int target = start + (unsigned int)(secondes * 100);

    while ((int)(g_ticks - target) < 0) {
        // On attend, g_ticks avance grâce aux interruptions timer
    }
}

// Attendre un nombre de ticks (1 tick = 10 ms)
static void attendre_ticks(unsigned int ticks) {
    unsigned int start = g_ticks;
    while ((unsigned int)(g_ticks - start) < ticks) {
        // on attend
    }
}

int main(void) {
    gpio_init();     // initialise la LED (rouge)
    timer_init();    // initialise le timer (tick toutes les 10 ms)

    while (1) {

        // 🟢 "Feu VERT" → LED éteinte (on peut passer)
        gpio_set_red(false);
        attendre_secondes(10);   // 10 secondes

        // 🟡 "Feu JAUNE" → LED clignote pendant ~3 secondes
        // 1 tick = 10 ms → 10 ticks = 100 ms
        // Ici : 15 clignotements, chaque cycle = 200 ms → 15 * 0.2 s = 3 s
        for (int i = 0; i < 15; i++) {
            gpio_set_red(true);
            attendre_ticks(10);   // ~100 ms
            gpio_set_red(false);
            attendre_ticks(10);   // ~100 ms
        }

        // 🔴 "Feu ROUGE" → LED rouge allumée
        gpio_set_red(true);
        attendre_secondes(15);   // 15 secondes
    }

    return 0;
}
