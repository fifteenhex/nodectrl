import pytest


@pytest.mark.asyncio
async def test_control_reboot(nodectrl_process, node):
    await node.wait_for_connection()
    await node.reboot()
