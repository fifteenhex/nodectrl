import pytest
from nodectrl import Node
from subprocess import Popen
import time
import random
import uuid


def pytest_addoption(parser):
    parser.addoption("--mosquitto_path", action="store")
    parser.addoption("--nodectrl_path", action="store")
    parser.addoption("--nodectrl_ca_path", action="store")
    parser.addoption('--valgrind_path', action='store')


@pytest.fixture(scope='session')
def mosquitto_port():
    return random.randint(6000, 8000)


@pytest.fixture(scope="session")
def mosquitto_path(request):
    path = request.config.getoption("--mosquitto_path")
    if path is None:
        path = "mosquitto"
    return path


@pytest.fixture(scope="session")
def nodectrl_path(request):
    path = request.config.getoption("--nodectrl_path")
    if path is None:
        path = "nodectrl"
    return path


@pytest.fixture(scope="session")
def nodectrl_ca_path(request):
    return request.config.getoption("--nodectrl_ca_path")


@pytest.fixture(scope="session")
def mosquitto_process(mosquitto_path, mosquitto_port):
    process = Popen([mosquitto_path, '-v', '-p', str(mosquitto_port)])
    time.sleep(2)
    yield process
    process.terminate()
    process.wait()


@pytest.fixture(scope='session')
def nodectrl_id():
    return 'node-%s' % uuid.uuid4()


@pytest.fixture(scope="session")
def nodectrl_process(mosquitto_port, nodectrl_path, nodectrl_id, nodectrl_ca_path, valgrind_path):
    args = [nodectrl_path, '-h', 'localhost', '-p', str(mosquitto_port), '-i', nodectrl_id, '--controlca',
            nodectrl_ca_path, '--safemode']
    if valgrind_path is not None:
        args[0:0] = [valgrind_path]
    process = Popen(args)
    time.sleep(2)
    assert process.poll() is None
    yield process
    process.terminate()
    process.wait()


@pytest.fixture()
async def node(mosquitto_process, mosquitto_port, nodectrl_process, nodectrl_id):
    node = Node('localhost', id=nodectrl_id, port=mosquitto_port)
    await node.wait_for_connection()
    return node


@pytest.fixture(scope="session")
def valgrind_path(request):
    return request.config.getoption("--valgrind_path")
