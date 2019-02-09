#pragma once

#include <json-glib/json-glib.h>

#define SUBTOPIC_HEARTBEAT	"heartbeat"
#define SUBTOPIC_CTRL		"ctrl"

struct nodectrl;

struct nodectrl_heartbeat {
	void (*init)(gpointer context);
	void (*heartbeat)(gpointer context, JsonBuilder* jsonbuilder);
	gpointer context;
};

struct nodectrl* nodectrl_mainloop_new(const gchar* topicroot, const gchar* id,
		const gchar* mqttid, const gchar* mqtthost, unsigned mqttport);
void nodectrl_mainloop_heartbeat_add(struct nodectrl* nodectrl,
		struct nodectrl_heartbeat* heartbeat);
void nodectrl_mainloop_run(struct nodectrl* nodectrl);
