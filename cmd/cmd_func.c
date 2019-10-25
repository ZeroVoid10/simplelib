#include "cmd_func.h"
#include "can_utils.h"
#include "can_func.h"
#include "flags.h"
#include "kick.h"
#include "mtr_driver.h"
#include <stdlib.h>

void cmd_kick_spr_free(int argc, char *argv[]);

void cmd_magnet_toggle(int argc, char *argv[]);
void cmd_magnet_set(int argc, char *argv[]);
void cmd_magnet_free(int argc, char *argv[]);

void cmd_mag_mtr_free(int argc, char *argv[]);
void cmd_mag_mtr_up_duty(int argc, char *argv[]);
void cmd_mag_mtr_up_speed(int argc, char *argv[]);
void cmd_mag_mtr_up_pos(int argc, char *argv[]);

void cmd_spr_mtr_back(int argc, char *argv[]);

void cmd_kick_spr_free(int argc, char *argv[]) {
    uprintf("spring free\r\n");
    kick_spring_free();
}

void cmd_mag_mtr_up_duty(int argc, char *argv[]) {
    if (argc == 2) {
        int arg = atoi(argv[1]);
        uprintf("mag mtr up test: duty %d\r\n", arg);
        mag_mtr_up_test(MMTR_DUTY, arg);
    }
}

void cmd_mag_mtr_up_speed(int argc, char *argv[]) {
    if (argc == 2) {
        int arg = atoi(argv[1]);
        uprintf("mag mtr up test: speed %d\r\n", arg);
        mag_mtr_up_test(MMTR_SPEED, arg);
    }
}

void cmd_mag_mtr_up_pos(int argc, char *argv[]) {
    if (argc == 2) {
        int arg = atoi(argv[1]);
        uprintf("mag mtr up test: pos %d\r\n", arg);
        mag_mtr_up_test(MMTR_POS, arg);
    }
}

void cmd_mag_mtr_free(int argc, char *argv[]) {
    md_set_duty(MAG_MTR_SID, 0);
    magnet_free();
    uprintf("mag mtr free\r\n");
}

void cmd_spr_mtr_back(int argc, char *argv[]) {
    if (argc == 2) {
        uint32_t arg = (uint32_t)atoi(argv[1]);
        uprintf("spring mtr back: time %d\r\n", arg);
        spr_mtr_back(arg);
    }
}

#ifdef SL_MOTOR_DRIVER
#ifdef EN_VESC_MOTOR_DRIVER
void cmd_vesc_set_duty(int argc, char *argv[]);
void cmd_vesc_set_rpm(int argc, char *argv[]);
void cmd_vesc_set_pos(int argc, char *argv[]);
void cmd_vesc_set_handbrake(int argc, char *argv[]);
void cmd_vesc_set_current(int argc, char *argv[]);
void cmd_vesc_set_current_brake(int argc, char *argv[]);

void cmd_vesc_set_duty(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        uprintf("id %d set duty %f\r\n", id, arg);
        vesc_set_duty(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_rpm(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        uprintf("id %d set rpm %f\r\n", id, arg);
        vesc_set_rpm(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_pos(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        uprintf("id %d set pos %f\r\n", id ,arg);
        vesc_set_pos(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_handbrake(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        vesc_set_handbrake(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_current(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        vesc_set_current(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_current_brake(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        vesc_set_current_brake(id, arg);
    } else {
        uprintf("Error Args");
    }
}
#endif // EN_VESC_MOTOR_DRIVER

#ifdef EN_MOTOR_DRIVER
void cmd_md_set_duty(int argc, char *argv[]);
void cmd_md_set_speed(int argc, char *argv[]);
void cmd_md_set_position(int argc, char *argv[]);

void cmd_md_set_duty(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        int arg = atoi(argv[2]);
        md_set_duty(id, arg);
        uprintf("id %d set duty %d\r\n", id, arg);
    } else {
        uprintf("Error Args\r\n");
    }
}

void cmd_md_set_speed(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        int arg = atoi(argv[2]);
        md_set_speed(id, arg);
        uprintf("id %d speed duty %d\r\n", id, arg);
    } else {
        uprintf("Error Args\r\n");
    }
}

void cmd_md_set_position(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        int arg = atoi(argv[2]);
        md_set_position(id, arg);
        uprintf("id %d position duty %d\r\n", id, arg);
    } else {
        uprintf("Error Args\r\n");
    }
}

#endif // EN_MOTOR_DRIVER
#endif // SL_MOTOR_DRIVER

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

void cmd_func_init(void) {
    cmd_add("hello", "just", cmd_hello_func);

    cmd_add("magtog", "magnet toggle", cmd_magnet_toggle);
    cmd_add("magset", "magnet set", cmd_magnet_set);
    cmd_add("magfree", "magnet free", cmd_magnet_free);

    cmd_add("mmup_free", "", cmd_mag_mtr_free);
    cmd_add("mmup_duty", "mag mtr up duty", cmd_mag_mtr_up_duty);
    cmd_add("mmup_speed", "", cmd_mag_mtr_up_speed);
    cmd_add("mmup_pos", "", cmd_mag_mtr_up_pos);

    cmd_add("smback", "spring mtr back", cmd_spr_mtr_back);

    #ifdef SL_MOTOR_DRIVER
    #ifdef EN_MOTOR_DRIVER
    cmd_add("m_duty", "", cmd_md_set_duty);
    cmd_add("m_speed", "", cmd_md_set_speed);
    cmd_add("m_pos", "", cmd_md_set_position);
    #endif // EN_MOTOR_DRIVER
    
    #ifdef EN_VESC_MOTOR_DRIVER
    cmd_add("v_duty", "vesc set duty", cmd_vesc_set_duty);
    cmd_add("v_rpm", "vesc set rpm", cmd_vesc_set_rpm);
    cmd_add("v_pos", "vesc set pos", cmd_vesc_set_pos);
    #endif // EN_VESC_MOTOR_DRIVER
    #endif // SL_MOTOR_DRIVER

    #ifdef SL_DEBUG
    cmd_add("can_test", "test can", cmd_can_test);
    cmd_add("test_wave", "", cmd_wave_test);
    cmd_add("rocker", "rocker", cmd_show_rocker);
    cmd_add("can_stop", "s", cmd_stop_rocker);
    #endif
}