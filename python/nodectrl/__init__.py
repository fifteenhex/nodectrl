import logging
import tlwpy.mqttbase
from tlwpy.mqttbase import MqttBase
from asyncio import Queue
import asyncio
import json

HEARTBEAT = 'heartbeat'
CTRL_REBOOT = 'reboot'


class Node(MqttBase):
    __slots__ = ['topic_root', 'id', '__logger', '__last_uptime', '__reboot_queue']

    def __on_heartbeat(self, client, userdata, msg):
        self.__logger.debug("saw gateway heartbeat")
        heartbeat_json = json.loads(msg.payload)

        uptime = heartbeat_json['sysinfo']['uptime']
        if uptime < self.__last_uptime:
            self.__logger.debug('node uptime went backwards, probably rebooted')
            self.event_loop.call_soon_threadsafe(self.__reboot_queue.put_nowait, True)
        self.__last_uptime = uptime

    def __init__(self, host: str, id: str, port: int = None, topic_root: str = 'nodectrl'):
        self.__last_uptime = -1
        self.__reboot_queue = Queue()
        self.topic_root = topic_root
        self.id = id
        heartbeat_topic = '%s/%s/%s' % (self.topic_root, id, HEARTBEAT)
        super().__init__(host, port=port, id=tlwpy.mqttbase.create_client_id('nodectrl'), topics=[heartbeat_topic])
        self.__logger = logging.getLogger('nodectrl')
        self.mqtt_client.message_callback_add(heartbeat_topic, self.__on_heartbeat)

    async def reboot(self, safemode: False):
        self.__logger.debug('triggering node reboot')
        self.mqtt_client.publish('%s/%s/ctrl/%s' % (self.topic_root, self.id, CTRL_REBOOT))
        if not safemode:
            return await asyncio.wait_for(self.__reboot_queue.get(), 5 * 60)
