#ifndef _MCFLY_MODULES_COMMANDS_H
#define _MCFLY_MODULES_COMMANDS_H

/* These are commands supported by the base Mcfly system modules */
typedef enum _mcfly_mod_commands_t mcfly_mod_commands_t;
enum mcfly_mod_commands_t
{
    /* Default */
    MCFLY_MOD_CMD_NONE,

    /* OBD Commands */
    MCFLY_MOD_CMD_OBD_SPEED,
};

#endif /* _MCFLY_MODULES_COMMANDS_H */
