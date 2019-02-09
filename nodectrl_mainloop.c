#include <mosquitto_client.h>
#include "include/nodectrl.h"
#include "json-glib-macros/jsonbuilderutils.h"

struct nodectrl {
	MosquittoClient* mosqclient;
	const gchar* topicroot;
	const gchar* id;
	GPtrArray* heartbeats;
};

static void nodectrl_heartbeat_call(gpointer data, gpointer user_data) {
	struct nodectrl_heartbeat* heartbeat = data;
	JsonBuilder* jsonbuilder = user_data;
	heartbeat->heartbeat(heartbeat->context, jsonbuilder);
}

static gboolean nodectrl_heartbeat(gpointer data) {
	struct nodectrl* cntx = data;

	if (mosquitto_client_isconnected(cntx->mosqclient)) {
		JsonBuilder* jsonbuilder = json_builder_new_immutable();
		json_builder_begin_object(jsonbuilder);
		g_ptr_array_foreach(cntx->heartbeats, nodectrl_heartbeat_call,
				jsonbuilder);
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
	//ctrl_onconnected(cntx->id, cntx->mosqclient);
	return nodectrl_heartbeat(user_data);
}

static gboolean nodectrl_messagecallback(MosquittoClient* client,
		const struct mosquitto_message* msg, gpointer user_data) {
	//ctrl_onmessage(client, msg);
	return TRUE;
}

struct nodectrl* nodectrl_mainloop_new(const gchar* topicroot, const gchar* id,
		const gchar* mqttid, const gchar* mqtthost, unsigned mqttport) {
	struct nodectrl* nodectrl = g_malloc(sizeof(*nodectrl));
	nodectrl->topicroot = topicroot;
	nodectrl->id = id;
	nodectrl->heartbeats = g_ptr_array_new();
	nodectrl->mosqclient = mosquitto_client_new_plaintext(mqttid, mqtthost,
			mqttport);

	g_timeout_add(30 * 1000, nodectrl_heartbeat, nodectrl);

	g_signal_connect(nodectrl->mosqclient, MOSQUITTO_CLIENT_SIGNAL_CONNECTED,
			(GCallback ) nodectrl_connectedcallback, nodectrl);
	g_signal_connect(nodectrl->mosqclient, MOSQUITTO_CLIENT_SIGNAL_MESSAGE,
			(GCallback ) nodectrl_messagecallback, nodectrl);

	return nodectrl;
}

void nodectrl_mainloop_heartbeat_add(struct nodectrl* nodectrl,
		struct nodectrl_heartbeat* heartbeat) {
	if (heartbeat->init != NULL)
		heartbeat->init(NULL);
	g_ptr_array_add(nodectrl->heartbeats, heartbeat);
}

void nodectrl_mainloop_run(struct nodectrl* nodectrl) {
	GMainLoop* mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);
}
