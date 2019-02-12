#include <sys/reboot.h>

#include "include/nodectrl/reboot.h"

void reboot_onmessage(MosquittoClient* client,
		const struct mosquitto_message* msg, gboolean safemode) {
	char** splittopic;
	int count;
	mosquitto_sub_topic_tokenise(msg->topic, &splittopic, &count);

	if (strcmp(splittopic[count - 2], SUBTOPIC_CTRL) == 0) {
		if (strcmp(splittopic[count - 1], "reboot") == 0) {
			g_message("reboot requested");
			if (safemode)
				g_message("in safemode, not actually rebooting");
			else
				reboot(RB_AUTOBOOT);
		}
	}

	mosquitto_sub_topic_tokens_free(&splittopic, count);
}
