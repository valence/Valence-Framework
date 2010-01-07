#ifndef _MCFLY_MODULES_TYPES_H
#define _MCFLY_MODULES_TYPES_H

/** These are the various types of modules for the base Mcfly system */
typedef enum _mcfly_mod_type_t
{
    MCFLY_MOD_TYPE_DUMMY,
    MCFLY_MOD_TYPE_OBD,
} mcfly_mod_type_t;


/** This is the data structure each module can return data in */
typedef struct _mcfly_mod_data_t
{
    /* Data can either be a numeric value, or an arbitray length blob  */
    double value;

    size_t binary_size;
    unsigned char *binary;
} mcfly_mod_data_t;

#endif /* _MCFLY_MODULES_TYPES_H */
