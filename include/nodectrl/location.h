#pragma once

#include <json-glib/json-glib.h>
#include "nodectrl.h"

struct location {
	gboolean valid;
	double lat, lon;
	guint64 timestamp;
};

void location_init(struct location* location);
void location_heartbeat(struct location* location, JsonBuilder* jsonbuilder);

struct nodectrl_heartbeat location_hb = { .init = location_init, .heartbeat =
		location_heartbeat, .contextsz = sizeof(struct location) };
