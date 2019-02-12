from Crypto.PublicKey import RSA
from Crypto.Signature import PKCS1_PSS
from Crypto.Hash import SHA256
import ssl
import base64


def signmsg(cert, key, cookie: str, topic: str, payload: bytes):
    key = RSA.importKey(key.read())

    digest = SHA256.new()
    digest.update(cookie.encode('utf-8'))
    digest.update(topic.encode('utf-8'))
    if payload is not None:
        digest.update(payload)

    signer = PKCS1_PSS.new(key)
    signature = signer.sign(digest)

    cert = base64.b64encode(ssl.PEM_cert_to_DER_cert(cert.read()), b'@!').decode('ascii')
    signature = base64.b64encode(signature, b'@!').decode('ascii')

    return '%s/%s/%s' % (topic, cert, signature)
