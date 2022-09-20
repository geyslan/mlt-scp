#include <stdbool.h>
#include <stdlib.h>

#include "bitprint.h"

#define MAX_SCOPES 32

#define UID_FILTER      1 << 0
#define PID_FILTER      1 << 1
#define UTS_NS_FILTER   1 << 2
#define COMM_FILTER     1 << 3

typedef struct filter_scope {
    bool uid;
    bool pid;
    bool uts_ns;
    bool comm;
} filter_scope_t;

typedef struct config_entry {
    uint32_t uid_filter_enabled;
    uint32_t pid_filter_enabled;
    uint32_t uts_ns_filter_enabled;
    uint32_t comm_filter_enabled;

    uint32_t uid_filter_out;
    uint32_t pid_filter_out;
    uint32_t uts_ns_filter_out;
    uint32_t comm_filter_out;

    uint32_t enabled_scopes;
} config_entry_t;

void
print_filter_scopes(filter_scope_t **fscopes)
{
    if (!fscopes)
        return;

    printf(">>> Filter Scopes\n");
    for (int i = 0; i < MAX_SCOPES; i++) {
        filter_scope_t *fs = fscopes[i];
        if (!fs)
            continue;

        printf(" > [%d] %p - %u %u %u %u\n", i+1, fs,
                                           fs->uid,
                                           fs->pid,
                                           fs->uts_ns,
                                           fs->comm
        );
    }
    printf("\n");
}

void
print_config_entry(config_entry_t *ce)
{
    printf(">>> Config Entry\n");
    printf(" * enabled_scopes:%*s", 30, " ");
    printb(ce->enabled_scopes);
    printf("  > uid_filter_enabled in scope%*s", 10, "\t");
    printb(ce->uid_filter_enabled);
    printf("  > pid_filter_enabled in scope%*s", 10, "\t");
    printb(ce->pid_filter_enabled);
    printf("  > uts_ns_filter_enabled in scope%*s", 10, "\t");
    printb(ce->uts_ns_filter_enabled);
    printf("  > comm_filter_enabled in scope%*s", 10, "\t");
    printb(ce->comm_filter_enabled);
    printf("\n");
}

void
set_filter_scope(filter_scope_t *fscope, uint32_t filters)
{
    if (!fscope)
        return;
    
    fscope->uid = !!(filters & UID_FILTER);
    fscope->pid = !!(filters & PID_FILTER);
    fscope->uts_ns = !!(filters & UTS_NS_FILTER);
    fscope->comm = !!(filters & COMM_FILTER);
}

filter_scope_t *
new_filter_scope(uint32_t filters)
{
    filter_scope_t *fscope = malloc(sizeof(filter_scope_t));
    if (!fscope)
        return NULL;

    set_filter_scope(fscope, filters);

    return fscope;
}

void set_config_entry(config_entry_t *ce, filter_scope_t **fscopes)
{
    if (!ce || !fscopes)
        return;
    
    ce->enabled_scopes = 0;
    for (int i = 0; i < MAX_SCOPES; i++) {
        filter_scope_t *fs = fscopes[i];
        if (!fs)
            continue;

        uint32_t scope_enabled = fscopes[i]->uid |
                                 fscopes[i]->pid |
                                 fscopes[i]->uts_ns |
                                 fscopes[i]->comm;

        if (!scope_enabled)
            continue;

        ce->uid_filter_enabled |= (fscopes[i]->uid << i);
        ce->pid_filter_enabled |= (fscopes[i]->pid << i);
        ce->uts_ns_filter_enabled |= (fscopes[i]->uts_ns << i);
        ce->comm_filter_enabled |= (fscopes[i]->comm << i);
        ce->enabled_scopes |= (1 << i);
    }
}

int
main(void)
{
    filter_scope_t *fscopes[MAX_SCOPES] = {
        new_filter_scope(UID_FILTER),
        new_filter_scope(PID_FILTER | COMM_FILTER),
        new_filter_scope(0),
        new_filter_scope(UTS_NS_FILTER),
        new_filter_scope(PID_FILTER)
    };
    print_filter_scopes(fscopes);

    config_entry_t ce = {};
    set_config_entry(&ce, fscopes);
    print_config_entry(&ce);

    return 0;
}
