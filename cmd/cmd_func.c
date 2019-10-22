#include "cmd_func.h"
#include "can_utils.h"
#include "can_func.h"
#include "flags.h"
#include "kick.h"

void cmd_magnet_toggle(int argc, char *argv[]);
void cmd_magnet_set(int argc, char *argv[]);
void cmd_magnet_free(int argc, char *argv[]);

void cmd_func_init(void) {
    cmd_add("hello", "just", cmd_hello_func);

    cmd_add("magtog", "magnet toggle", cmd_magnet_toggle);
    cmd_add("magset", "magnet set", cmd_magnet_set);
    cmd_add("magfree", "magnet free", cmd_magnet_free);

    #ifdef SL_DEBUG
    cmd_add("can_test", "test can", cmd_can_test);
    cmd_add("test_wave", "", cmd_wave_test);
    cmd_add("rocker", "rocker", cmd_show_rocker);
    cmd_add("can_stop", "s", cmd_stop_rocker);
    #endif
}

void cmd_hello_func(int argc, char *argv[]) {
    uprintf("hello world\r\n");
}

void cmd_magnet_toggle(int argc, char *argv[]) {
    MAGNET_STATE state = magnet_toggle();
    uprintf("magnet state: %d\r\n", state);
}

void cmd_magnet_set(int argc, char *argv[]) {
    MAGNET_STATE state = magnet_set();
    uprintf("magnet state: %d\r\n", state);
}

void cmd_magnet_free(int argc, char *argv[]) {
    MAGNET_STATE state = magnet_free();
    uprintf("magnet state: %d\r\n", state);
}

#ifdef SL_DEBUG
void cmd_can_test(int argc, char *argv[]) {
    uprintf("can send test\r\n");
    can_send_test();
}

void cmd_show_rocker(int argc, char *argv[]) {
    uprintf("show rocker\r\n");
    can_data_show_flag = 1;
}

void cmd_stop_rocker(int argc, char *argv[]) {
    can_data_show_flag = 0;
    uprintf("sotp show rocker\r\n");
}

void cmd_wave_test(int argc, char *argv[]) {
    send_wave_flag ^= 1;
    uprintf(send_wave_flag? "Wave Start\r\n":"Wave Stop\r\n");
}
#endif // SL_DEBUG