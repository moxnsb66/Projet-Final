#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "gpio.h"

#define PERIPH_BASE        0x3F000000UL
#define INTC_BASE         (PERIPH_BASE + 0x00B200)

#define IRQ_BASIC_PENDING (*(volatile uint32_t*)(INTC_BASE + 0x00))
#define IRQ_PENDING1      (*(volatile uint32_t*)(INTC_BASE + 0x04))
#define IRQ_PENDING2      (*(volatile uint32_t*)(INTC_BASE + 0x08))
#define IRQ_ENABLE1       (*(volatile uint32_t*)(INTC_BASE + 0x10))
#define IRQ_ENABLE2       (*(volatile uint32_t*)(INTC_BASE + 0x14))
#define IRQ_ENABLE_BASIC  (*(volatile uint32_t*)(INTC_BASE + 0x18))
#define IRQ_DISABLE1      (*(volatile uint32_t*)(INTC_BASE + 0x1C))

#define SYS_TIMER_MATCH1_IRQ (1u << 1)

#define SYS_TIMER_BASE    (PERIPH_BASE + 0x003000)
#define TIMER_CS          (*(volatile uint32_t*)(SYS_TIMER_BASE + 0x00))
#define CS_M1             (1u << 1)

/**
 * Compteur global de "ticks" utilisé par main.c
 * 1 tick = 10 ms (voir timer.c)
 */
volatile unsigned int g_ticks = 0;

void interrupts_init(void) {
    // Autorise les interruptions du timer système (match 1)
    IRQ_ENABLE1 = SYS_TIMER_MATCH1_IRQ;
}

/**
 * Gestionnaire d’interruptions (appelé depuis vectors.S)
 * Ici on ne fait que :
 *  - reconnaître l’IRQ du timer système
 *  - incrémenter g_ticks
 *  - programmer le prochain tick avec timer_schedule_next()
 */
void irq_handler(void) {
    // IRQ du timer système (match 1) en attente ?
    if (IRQ_PENDING1 & SYS_TIMER_MATCH1_IRQ) {
        // Vérifie que c’est bien le match 1 qui a déclenché
        if (TIMER_CS & CS_M1) {
            // Acquitte l’interruption (efface le bit)
            TIMER_CS = CS_M1;

            // On avance le compteur global de ticks
            g_ticks++;

            // Programme la prochaine interruption
            timer_schedule_next();
        }
        return;
    }

    // Ici, tu pourras gérer d’autres sources d’IRQ plus tard si nécessaire
}
