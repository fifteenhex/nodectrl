#include <sys/sysinfo.h>

#include "include/nodectrl/sysinfo.h"
#include "json-glib-macros/jsonbuilderutils.h"

void sysinfo_heartbeat(void* nothing, JsonBuilder* jsonbuilder) {
	struct sysinfo si;
	if (sysinfo(&si) == 0) {
		JSONBUILDER_START_OBJECT(jsonbuilder, "sysinfo");
		JSONBUILDER_ADD_INT(jsonbuilder, "uptime", si.uptime);
		json_builder_end_object(jsonbuilder);
	}
}
