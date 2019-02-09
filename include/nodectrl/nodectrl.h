#pragma once

#include <json-glib/json-glib.h>
#include <mosquitto_client.h>

#define SUBTOPIC_HEARTBEAT	"heartbeat"
#define SUBTOPIC_CTRL		"ctrl"

struct nodectrl;

struct nodectrl_heartbeat {
	void (*init)(gpointer context);
	void (*heartbeat)(gpointer context, JsonBuilder* jsonbuilder);
	gsize contextsz;
};

struct nodectrl_control {
	void (*init)(gpointer context);
	void (*onmsg)(MosquittoClient* client, const struct mosquitto_message* msg);
	gsize contextsz;
};

struct nodectrl* nodectrl_mainloop_new(const gchar* topicroot, const gchar* id,
		const gchar* mqttid, const gchar* mqtthost, unsigned mqttport);
void nodectrl_mainloop_heartbeat_add(const struct nodectrl* nodectrl,
		const struct nodectrl_heartbeat* heartbeat);
void nodectrl_mainloop_control_add(const struct nodectrl* nodectrl,
		const struct nodectrl_control* control);
void nodectrl_mainloop_run(struct nodectrl* nodectrl);
