#include <mosquitto_client.h>
#include "include/nodectrl/nodectrl.h"
#include "json-glib-macros/jsonbuilderutils.h"

struct nodectrl {
	MosquittoClient* mosqclient;
	const gchar* topicroot;
	const gchar* id;
	GPtrArray* heartbeats;
	GPtrArray* controls;
	gchar* cookie;
};

struct nodectrl_heartbeat_instance {
	const struct nodectrl_heartbeat* heartbeat;
	gpointer context;
};

struct nodectrl_control_instance {
	const struct nodectrl_control* control;
	gpointer context;
};

struct nodectrl_control_onmsgcontext {
	MosquittoClient* client;
	const struct mosquitto_message* msg;
};

static void nodectrl_heartbeat_call(gpointer data, gpointer user_data) {
	struct nodectrl_heartbeat_instance* heartbeat = data;
	JsonBuilder* jsonbuilder = user_data;
	heartbeat->heartbeat->heartbeat(heartbeat->context, jsonbuilder);
}

static gboolean nodectrl_heartbeat(gpointer data) {
	struct nodectrl* cntx = data;

	if (mosquitto_client_isconnected(cntx->mosqclient)) {
		JsonBuilder* jsonbuilder = json_builder_new_immutable();
		json_builder_begin_object(jsonbuilder);
		g_ptr_array_foreach(cntx->heartbeats, nodectrl_heartbeat_call,
				jsonbuilder);

		JSONBUILDER_ADD_STRING(jsonbuilder, "cookie", cntx->cookie);

		json_builder_end_object(jsonbuilder);

		gsize jsonlen;
		gchar* json = jsonbuilder_freetostring(jsonbuilder, &jsonlen, TRUE);

		GString* topicstr = g_string_new(cntx->topicroot);
		g_string_append(topicstr, "/");
		g_string_append(topicstr, cntx->id);
		g_string_append(topicstr, "/");
		g_string_append(topicstr, SUBTOPIC_HEARTBEAT);

		gchar* topic = g_string_free(topicstr, FALSE);

		mosquitto_publish(
				mosquitto_client_getmosquittoinstance(cntx->mosqclient), NULL,
				topic, jsonlen, json, 0, FALSE);

		g_free(topic);
		g_free(json);
	}

	return TRUE;
}

static gboolean nodectrl_connectedcallback(MosquittoClient* client,
		void* something, gpointer user_data) {
	struct nodectrl* cntx = user_data;
	GString* topicstr = g_string_new(cntx->topicroot);
	g_string_append(topicstr, "/");
	g_string_append(topicstr, cntx->id);
	g_string_append(topicstr, "/");
	g_string_append(topicstr, SUBTOPIC_CTRL);
	g_string_append(topicstr, "/#");
	gchar* topic = g_string_free(topicstr, FALSE);
	mosquitto_subscribe(mosquitto_client_getmosquittoinstance(client),
	NULL, topic, 0);
	g_free(topic);
	return nodectrl_heartbeat(user_data);
}

static void nodectrl_messagecallback_call(gpointer data, gpointer user_data) {
	struct nodectrl_control* ctrl = data;
	struct nodectrl_control_onmsgcontext* callcntx = user_data;
	ctrl->onmsg(callcntx->client, callcntx->msg);
}

static gboolean nodectrl_messagecallback(MosquittoClient* client,
		const struct mosquitto_message* msg, gpointer user_data) {
	struct nodectrl* cntx = user_data;
	struct nodectrl_control_onmsgcontext callcntx = { .client = client, .msg =
			msg };
	g_ptr_array_foreach(cntx->heartbeats, nodectrl_messagecallback_call,
			&callcntx);
	return TRUE;
}

static void nodectrl_mainloop_refreshcookie(struct nodectrl* nodectrl) {
	if (nodectrl->cookie != NULL)
		g_free(nodectrl->cookie);
	nodectrl->cookie = g_uuid_string_random();
}

struct nodectrl* nodectrl_mainloop_new(const gchar* topicroot, const gchar* id,
		const gchar* mqttid, const gchar* mqtthost, unsigned mqttport) {
	struct nodectrl* nodectrl = g_malloc(sizeof(*nodectrl));
	nodectrl->topicroot = topicroot;
	nodectrl->id = id;
	nodectrl->heartbeats = g_ptr_array_new();
	nodectrl->controls = g_ptr_array_new();
	nodectrl->mosqclient = mosquitto_client_new_plaintext(mqttid, mqtthost,
			mqttport);

	nodectrl_mainloop_refreshcookie(nodectrl);

	g_timeout_add(30 * 1000, nodectrl_heartbeat, nodectrl);

	g_signal_connect(nodectrl->mosqclient, MOSQUITTO_CLIENT_SIGNAL_CONNECTED,
			(GCallback ) nodectrl_connectedcallback, nodectrl);
	g_signal_connect(nodectrl->mosqclient, MOSQUITTO_CLIENT_SIGNAL_MESSAGE,
			(GCallback ) nodectrl_messagecallback, nodectrl);

	return nodectrl;
}

void nodectrl_mainloop_heartbeat_add(const struct nodectrl* nodectrl,
		const struct nodectrl_heartbeat* heartbeat) {
	struct nodectrl_heartbeat_instance* hbinstance = g_malloc(
			sizeof(*hbinstance));
	hbinstance->heartbeat = heartbeat;
	hbinstance->context = NULL;
	if (heartbeat->contextsz != 0)
		hbinstance->context = g_malloc(heartbeat->contextsz);

	if (heartbeat->init != NULL)
		heartbeat->init(hbinstance->context);

	g_ptr_array_add(nodectrl->heartbeats, hbinstance);
}

void nodectrl_mainloop_control_add(const struct nodectrl* nodectrl,
		const struct nodectrl_control* control) {
	struct nodectrl_control_instance* ctrlinstance = g_malloc(
			sizeof(*ctrlinstance));
	ctrlinstance->control = control;
	ctrlinstance->context = NULL;
	if (control->contextsz != 0)
		ctrlinstance->context = g_malloc(control->contextsz);

	if (control->init != NULL)
		control->init(ctrlinstance->context);

	g_ptr_array_add(nodectrl->controls, ctrlinstance);
}

void nodectrl_mainloop_run(struct nodectrl* nodectrl) {
	GMainLoop* mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);
}
