#define GETTEXT_PACKAGE "gtk20"

#include <nodectrl/nodectrl.h>
#include <nodectrl/sysinfo.h>
#include <nodectrl/reboot.h>
#include <mosquitto_client.h>

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

	gboolean nodectrl_safemode = FALSE;
	gchar* nodectrl_controlca = NULL;

	GOptionEntry entries[] = { MQTTOPTS, NODECTRL_OPTS, { "nodeid", 'i', 0,
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
			mqtthost, mqttport, nodectrl_safemode);

	nodectrl_mainloop_heartbeat_add(nodectrl, &sysinfo_hb);
	nodectrl_mainloop_control_add(nodectrl, &reboot_ctrl);

	nodectrl_mainloop_run(nodectrl);

	out: return ret;
}

