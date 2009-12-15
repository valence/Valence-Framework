#ifndef _MCFLY_MODULES_COMMANDS_H
#define _MCFLY_MODULES_COMMANDS_H

/* These are commands supported by the base Mcfly system modules */
typedef enum _mcfly_mod_cmd_t mcfly_mod_cmd_t;
enum _mcfly_mod_cmd_t
{
    /* Default */
    MCFLY_MOD_CMD_NONE,

    /* OBD Commands */
    MCFLY_MOD_CMD_OBD_SPEED,
    MCFLY_MOD_CMD_OBD_RPM,
};

#endif /* _MCFLY_MODULES_COMMANDS_H */
