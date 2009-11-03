#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mcfly/config.h>
#include <mcfly/error.h>
#include <mcfly/type.h>
#include <mcfly/util.h>


#define MIN(_v1, _v2) (((_v1) < (_v2)) ? (_v1) : (_v2))


/* 
 * Forwards
 */

static mcfly_err_t load_config(
    const char            *config,
    mcfly_cfg_t           *configs,
    const mcfly_cfg_def_t *defs,
    int                    n_defs);

static mcfly_cfg_t *parse_entry(FILE *fp);

static mcfly_err_t add_entry(
    mcfly_cfg_t           *head,
    mcfly_cfg_t           *entry,
    const mcfly_cfg_def_t *defs,
    int                    n_defs);


/*
 * Defined
 */

mcfly_cfg_t *mcfly_cfg_load(
    const mcfly_t          mcfly,
    const mcfly_cfg_def_t *defs,
    int                    n_defs,
    const char            *config,
    mcfly_err_t           *err)
{
    mcfly_err_t  whoops;
    mcfly_cfg_t *cfg;

    if (err)
      *err = MCFLY_SUCCESS;

    if (!(cfg = calloc(1, sizeof(mcfly_cfg_t))))
    {
        if (err)
          *err = MCFLY_ERR_NOMEM;
        return NULL;
    }

    /* If no config is specified use the one in the handle */
    if (!config)
      config = mcfly->base_config_path;

    if ((whoops = load_config(config, cfg, defs, n_defs)) != MCFLY_SUCCESS)
    {
        if (err)
          *err = whoops;
        return NULL;
    }
    
    return cfg; 
}


static mcfly_err_t load_config(
    const char            *config,
    mcfly_cfg_t           *configs,
    const mcfly_cfg_def_t *defs,
    int                    n_defs)
{
    char         c;
    FILE        *fp;
    mcfly_cfg_t *cfg;
    mcfly_err_t  err;

    if (!(fp = fopen(config, "r")))
      return MCFLY_ERR_OFILE;

    /* Read each character skipping over comments */
    while ((c = tolower(fgetc(fp))) && !feof(fp))
    {
        /* Skip whitespace */
        if (isspace(c))
          continue;

        /* Skip comments */
        else if (c == '#')
        {
            while (!feof(fp) && (c!='\n' && c!='\r'))
              c = fgetc(fp);
        }

        /* We found a key and value */
        else
        {
            ungetc(c, fp);
            cfg = parse_entry(fp);
            err = add_entry(configs, cfg, defs, n_defs);

            /* If we don't have a config entry that is normal, it might be from
             * a module that added their own special sauce
             */
            if (err != MCFLY_SUCCESS)
            {
                free(cfg->def);
                free(cfg);
                if (err != MCFLY_ERR_NOCONFIG)
                  return err;
            }
        }
    }

    return MCFLY_SUCCESS;
}


static mcfly_cfg_t *parse_entry(FILE *fp)
{
    int          is_real, is_string, key_idx, val_idx;
    char         c, key[MCFLY_MAX_STRING_LEN], val[MCFLY_MAX_STRING_LEN];
    mcfly_cfg_t *cfg;

    /* Skip leading whitespace */
    c = fgetc(fp);
    while (!feof(fp) && isspace(c))
      c = fgetc(fp);
    ungetc(c, fp);

    /* Get the key */
    key_idx = 0;
    memset(key, 0, sizeof(key));
    while (!feof(fp) && (c = fgetc(fp)) && c!='=')
      if (isspace(c))
        break;
      else if (c!='#')
        key[key_idx++] = c;
      else
        return NULL;

    /* Skip next set of whitespace */
    while ((c = fgetc(fp)) && isspace(c))
        /* Iterate */ ;

    /* Possibly more whitespace, skip it */
    if (c == '=')
      while ((c = fgetc(fp)) && isspace(c))
        /* Iterate */ ;

    /* Get the value */
    val_idx = 0;
    memset(val, 0, sizeof(val));
    is_real = is_string = 0;
    ungetc(c, fp);
    while ((c = fgetc(fp)) && c!='#' && (c!='\n' || c!='\r'))
    {
        /* Get string */
        if (isalpha(c) || (c == '\'') || (c == '"'))
        {
            if (isalpha(c))
              ungetc(c, fp);

            is_string = 1;
            while ((c = fgetc(fp)) && c!='\'' && c!='"' && c!='\n' && c!='\r')
              val[val_idx++] = c;
            break;
        }
        else if (c == '.')
          is_real = 1;

        /* I guess we have a valid number */
        val[val_idx++] = c;
    }

    /* Construct the config object */
    if (!(cfg = calloc(1, sizeof(mcfly_cfg_t))) ||
        !(cfg->def = calloc(1, sizeof(mcfly_cfg_def_t))))
      return NULL;
            
    /* Config entry name */
    strncpy(cfg->def->name, key, MCFLY_MAX_STRING_LEN);

    /* Value */
    if (is_string)
    {
        cfg->def->type = MCFLY_CFG_VALUE_STRING;
        strncpy(cfg->value.s, val, MIN(val_idx, MCFLY_MAX_STRING_LEN-1));
    }
    else if (is_real)
    {
        cfg->def->type = MCFLY_CFG_VALUE_REAL;
        cfg->value.d = atof(val);
    }
    else
    {
        cfg->def->type = MCFLY_CFG_VALUE_INT;
        cfg->value.i = atoi(val);
    }

    return cfg;
}


/* Find the definition for the discovered entry and then add the entry to the
 * allocated list of configs.
 */
static mcfly_err_t add_entry(
    mcfly_cfg_t           *head,
    mcfly_cfg_t           *entry,
    const mcfly_cfg_def_t *defs,
    int                    n_defs)
{
    int i;

    /* Find the definition */
    for (i=0; i<n_defs; ++i)
      if (strncmp(entry->def->name, defs[i].name, MCFLY_MAX_STRING_LEN) == 0)
      {
          /* If we have a def already. It was allocated, free that and just
           * point to static memory
           */
          free(entry->def);
          entry->def = (mcfly_cfg_def_t *)&defs[i];
          break;
      }

    /* If we do not have the definition, don't add it! */
    if (i == n_defs)
      return MCFLY_ERR_NOCONFIG;

    /* If we have yet to truly set the head node, make use of it */
    if (!head->def)
    {
        memcpy(head, entry, sizeof(mcfly_cfg_t));
        free(entry);
    }
    else
      mcfly_util_list_add(&head->list, &entry->list);

    return MCFLY_SUCCESS;
}


const void *mcfly_cfg_get_from_key(const mcfly_cfg_t *cfg, int key)
{
    const mcfly_cfg_t       *ele;
    const mcfly_list_node_t *itr;

    for (itr=&cfg->list; itr; itr=itr->next)
    {
        ele = mcfly_util_list_get(itr, mcfly_cfg_t, list);
        if (ele->def && (ele->def->key == key))
          return &ele->value;
    }

    return NULL;
}


const void *mcfly_cfg_get_from_name(
    const mcfly_cfg_t *cfg,
    const char        *cfg_name)
{
    const mcfly_cfg_t       *ele;
    const mcfly_list_node_t *itr;

    if (!cfg)
      return NULL;

    for (itr=&cfg->list; itr; itr=itr->next)
    {
        ele = mcfly_util_list_get(itr, mcfly_cfg_t, list);
        if (ele->def &&
            (strncmp(cfg->def->name, cfg_name, MCFLY_MAX_STRING_LEN) == 0))
        {
            return &ele->value;
        }
    }

    return NULL;
}


void mcfly_cfg_shutdown(mcfly_cfg_t *cfg)
{
    mcfly_cfg_t       *ele;
    mcfly_list_node_t *itr;

    if (!cfg)
      return;

    itr = &cfg->list;
    while (itr)
    {
        ele = mcfly_util_list_get(itr, mcfly_cfg_t, list);
        itr = mcfly_util_list_remove(itr);
        free(ele);
    }
}
