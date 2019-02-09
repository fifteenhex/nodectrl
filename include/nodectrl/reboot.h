#pragma once

#include <mosquitto_client.h>
#include "nodectrl.h"

void reboot_onmessage(MosquittoClient* client,
		const struct mosquitto_message* msg);

struct nodectrl_control reboot_ctrl = { .onmsg = reboot_onmessage };
