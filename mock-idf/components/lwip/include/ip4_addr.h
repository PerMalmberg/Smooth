#pragma once

/** This is the aligned version of ip4_addr_t,
   used as local variable, on the stack, etc. */
struct ip4_addr {
    uint32_t addr;
};

/** ip4_addr_t uses a struct for convenience only, so that the same defines can
 * operate both on ip4_addr_t as well as on ip4_addr_p_t. */
typedef struct ip4_addr ip4_addr_t;