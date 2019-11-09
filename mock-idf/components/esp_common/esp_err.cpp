#include <esp_err.h>

const char *esp_err_to_name(esp_err_t code)
{
    // We're expected to always give back an actual
    // string as per the original implementation.
    return "UNKNOWN ERROR";
}