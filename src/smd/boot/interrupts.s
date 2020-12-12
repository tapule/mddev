/**
 * @file exceptions.s
 * @author Tapule
 * @brief Interrupts handlers
 */
 .section .text

.global _int_external
 _int_external:
    rte

.global _int_hblank
 _int_hblank:
    rte

.global _int_vblank
 _int_vblank:
    rte

.global _int_unhandled
 _int_unhandled:
    rte
