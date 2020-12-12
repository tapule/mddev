/**
 * @file exceptions.s
 * @author Tapule
 * @brief Exceptions handlers
 */
.section .text

.global _exc_bus_error
_exc_bus_error:
    rte

.global _exc_address_error
_exc_address_error:
    rte

.global _exc_illegal_instruction
_exc_illegal_instruction:
    rte

.global _exc_zero_divide
_exc_zero_divide:
    rte

.global _exc_chk_instruction
_exc_chk_instruction:
    rte

.global _exc_trapv
_exc_trapv:
    rte

.global _exc_privilege_violation
_exc_privilege_violation:
    rte

.global _exc_unhandled
_exc_unhandled:
    rte
