#pragma once

#include <glib.h>
#include <json-glib/json-glib.h>
#include "nodectrl.h"

struct sensor {
	gchar* name;
	guint32 millidegrees;
};

struct thermal {
	GSList* sensors;
};

void thermal_init(struct thermal* t);
void thermal_heartbeat(struct thermal* t, JsonBuilder* jsonbuilder);

struct nodectrl_heartbeat thermal_hb = { .init = thermal_init, .heartbeat =
		thermal_heartbeat, .contextsz = sizeof(struct thermal) };
