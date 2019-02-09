#define GETTEXT_PACKAGE "gtk20"

#include <nodectrl.h>
#include <mosquitto_client.h>
#include "include/sysinfo.h"

#define TOPICROOT "nodectrl"

int main(int argc, char** argv) {
	int ret = 0;

	gchar* id = NULL;
	gchar* mqttid = NULL;
	gchar* mqtthost = "localhost";
	gint mqttport = 1883;
	gchar* mqttrootcert = NULL;
	gchar* mqttdevicecert = NULL;
	gchar* mqttdevicekey = NULL;

	GOptionEntry entries[] = { MQTTOPTS, { "nodeid", 'i', 0,
			G_OPTION_ARG_STRING, &id, "", "" }, { NULL } };

	GOptionContext* context = g_option_context_new("");
	GError* error = NULL;
	g_option_context_add_main_entries(context, entries, GETTEXT_PACKAGE);
	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_print("option parsing failed: %s\n", error->message);
	}

	if (id == NULL) {
		g_message("You must provide a node id");
		ret = 1;
		goto out;
	}

	struct nodectrl* nodectrl = nodectrl_mainloop_new(TOPICROOT, id, NULL,
			mqtthost, mqttport);

	struct nodectrl_heartbeat sysinfoheartbeat = { .init = NULL, .heartbeat =
			sysinfo_heartbeat };
	nodectrl_mainloop_heartbeat_add(nodectrl, &sysinfoheartbeat);

	nodectrl_mainloop_run(nodectrl);

	out: return ret;
}

