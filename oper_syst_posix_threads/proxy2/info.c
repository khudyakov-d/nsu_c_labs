#include <malloc.h>
#include "info.h"

void free_info(request_info_t *info) {
    free(info->hostname);
    free(info->method);
    free(info->url);
    free(info);
}
