#ifndef _MCFLY_ERROR_H
#define _MCFLY_ERROR_H

#include <stdlib.h>


/**
 * Error codes in McFly are actually structures of the value and its
 * corresponding description.  When error codes are compared, since they are
 * static, just the addresses get compared.  When you print an error code or
 * actually want to use one, the proper macros should be used: MCFLY_ERR_VAL()
 * and MCFLY_ERR_STR()
 */
struct _mcfly_err_code_t {int val; const char *str;};
typedef struct _mcfly_err_code_t *mcfly_err_t;


#define _new_err_code(_val, _name, _str)                           \
    static const struct _mcfly_err_code_t _##_name = {_val, _str}; \
    static const mcfly_err_t _name = (mcfly_err_t)&_##_name;


/** Error codes */
/* Error codes */
#define MCFLY_SUCCESS NULL
_new_err_code(1,  MCFLY_ERR_UNKNOWN,     "Unknown")
_new_err_code(2,  MCFLY_ERR_NOMEM,       "Out of Memory")
_new_err_code(3,  MCFLY_ERR_INVALID_ARG, "Invalid Argument")
_new_err_code(4,  MCFLY_ERR_LIST,        "List")
_new_err_code(5,  MCFLY_ERR_ODEV,        "Opening Device")
_new_err_code(6,  MCFLY_ERR_ODIR,        "Opening Directory")
_new_err_code(7,  MCFLY_ERR_OFILE,       "Opening File")
_new_err_code(8,  MCFLY_ERR_WRITE,       "Writing Data")
_new_err_code(9,  MCFLY_ERR_NOCONFIG,    "No Configuration Entry Found")
_new_err_code(10, MCFLY_ERR_NOMOD,       "No Module Found")
_new_err_code(11, MCFLY_ERR_MODINIT,     "Module Initialization")
_new_err_code(12, MCFLY_ERR_MODRECV,     "Receiving Data from Module")
_new_err_code(13, MCFLY_ERR_MODTHREAD,   "Threading Module")
_new_err_code(14, MCFLY_ERR_NOCMD,       "Command Not Supported")
_new_err_code(15, MCFLY_ERR_CMDSEND,     "Error Sending Command")


/** 
 * MCFLY_ERR_VAL
 * MCFLY_ERR_STR
 *
 * @param _err Error object
 *
 * @return Either an integer or the string representation of the error.
 */
#define MCFLY_ERR_VAL(_err) ((_err == MCFLY_SUCCESS) ? 0 : _err->val)
#define MCFLY_ERR_STR(_err) ((_err == MCFLY_SUCCESS) ? "Success" : _err->str)

#endif /* _MCFLY_ERROR_H */
