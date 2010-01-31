#ifndef _MCFLY_UTIL_H
#define _MCFLY_UTIL_H

#include <stddef.h>
#include <mcfly/type.h>


/** 
 * Note: The list functionality here is implemented similiar as to how Linux
 * implements their kernel lists: include/linux/list.h (in the kernel)
 */


/**
 * Add an entry to the end of the list
 *
 * @param head  Start of the list
 * @param entry Element to add
 */
extern void mcfly_util_list_add(
    mcfly_list_node_t *head,
    mcfly_list_node_t *entry);


/**
 * Remove the entry from the list.  This routine does not deallocate any
 * memory.
 *
 *
 * @param entry Element to remove
 *
 * @return The next element in the list after itr.
 *         In other words, this routine returns itr->next.
 */
extern mcfly_list_node_t *mcfly_util_list_remove(mcfly_list_node_t *entry);


/**
 * Returns a pointer to the entry in the list
 *
 * @param _entry The mcfly_list_node_t instance
 * @param _type  Actual data type being stored
 * @param _next  Member/field name in the structure of '_type' that is
 *               the mcfly_list_node_t object
 *
 * @return The entry casted to '_type'
 */
#define mcfly_util_list_get(_entry, _type, _next) \
    ((_type *)((char *)_entry - offsetof(_type, _next)))

#endif /* _MCFLY_UTIL_H */
