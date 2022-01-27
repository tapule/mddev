/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: assert.h
 * Compile time assert
 *
 * This file provides a simple assert that validates a predicate at compile time
 * without generating any code.
 * It uses a trick to typedef an array of negative size on failure. The output
 * on failure will looks like:
 *  error: size of array 'assertion_failed_main_c_31' is negative
 * 
 * More info:
 * https://stackoverflow.com/questions/807244/c-compiler-asserts-how-to-implement/809465#809465
 * http://www.pixelbeat.org/programming/gcc/static_assert.html
 */

#ifndef MDDEV_CTASSERT_H
#define MDDEV_CTASSERT_H

#define CT_ASSERT_PASTE(a,b) a##b
#define CT_ASSERT_LINE(predicate, line, file) \
    typedef char CT_ASSERT_PASTE(assertion_failed_##file##_,line)[2*!!(predicate)-1];

/**
 * @brief Checks a predicate at compile time
 * 
 * Validates a predicate at compile time without generating any code 
 * 
 * @param predicate  The predicate to validate
 * @param file  An identifier for the current source file.
 * 
 * @note The predicate must be evaluable as a C boolean.
 * @note It is recommended to use an unique file identifier, for example:
 *      main.c -> main_c
 *      entity.c -> entity_c
 */
#define ct_assert(predicate, file) CT_ASSERT_LINE(predicate,__LINE__,file)

#endif /* MDDEV_CTASSERT_H */