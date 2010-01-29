#ifndef _MCFLY_MODULES_COMMANDS_H
#define _MCFLY_MODULES_COMMANDS_H

/* These are commands supported by the base Mcfly system modules */
typedef enum _mcfly_mod_cmd_t
{
    /* Default */
    MCFLY_MOD_CMD_NONE,

    /* Testing */
    MCFLY_MOD_CMD_DUMMY_TEST,

    /* OBD Commands */
    MCFLY_MOD_CMD_OBD_SPEED,
    MCFLY_MOD_CMD_OBD_RPM,
    MCFLY_MOD_CMD_OBD_THROTTLE_POS,
    MCFLY_MOD_CMD_OBD_STANDARDS,
    MCFLY_MOD_CMD_OBD_AMBIENT_AIR,
    MCFLY_MOD_CMD_OBD_VIN
} mcfly_mod_cmd_t;

#endif /* _MCFLY_MODULES_COMMANDS_H */
