#pragma once

#include <json-glib/json-glib.h>
#include "nodectrl.h"

void sysinfo_heartbeat(void* nothing, JsonBuilder* jsonbuilder);

struct nodectrl_heartbeat sysinfo_hb = { .heartbeat = sysinfo_heartbeat };
