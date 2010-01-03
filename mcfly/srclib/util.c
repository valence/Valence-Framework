#include <mcfly/type.h>


void mcfly_util_list_add(mcfly_list_node_t *head, mcfly_list_node_t *entry)
{
    if (!head->next)
    {
        head->next = entry;
        head->prev = entry;
        entry->prev = head;
        return;
    }

    head->prev->next = entry;
    entry->prev = head->prev;
    head->prev = entry;
}


mcfly_list_node_t *mcfly_util_list_remove(mcfly_list_node_t *entry)
{
    /* Head? */
    if ((entry == entry->prev) && (entry == entry->next))
      return NULL;

    /* End of the list? */
    if (!entry->next)
      return NULL;
    
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    return entry->next;
}
