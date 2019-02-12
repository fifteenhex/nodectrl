# nodectrl


## Source validation/replay prevention for controls

MQTT itself doesn't support signatures for publishes to verify where a publish came from.
Even with client certificates and ACLs on your broker you probably still want to know that
a publish for an action like `reboot` came from someone/somewhere that you trust.

Fortunately an MQTT topic can be up to 64K in length so we have enough space to put some
sort of signature in there instead of coming up with a custom payload format. This means
any payload could be signed in theory. So if at some point the serialisation is changed
from JSON to protobuf or something the same verification works without modifications.

So that you don't need to configure all of the trusted senders on the node an x509 certificate
encoded into the topic along with a trusted CA certificate on the node is used to evaluate whether
a sender is trusted. This does mean that the size of the MQTT message is inflated by a good amount.
In the future maybe there could be an option for sending the finger print of a pre-trusted cert
instead.

To avoid an attacker being able to capture a signed publish and replay it later the signature for 
the publish includes a one use cookie from the node heartbeat. As soon as the cookie is used the 
node will change the cookie and any captured publishes will no longer pass signature validation.

# Signed MQTT topic format

Signed topics are just the target topic with two extra elements for the certificate and the
signature. Note that the base64 encoding is slightly modified so that `+` and `/` become 
`@` and `!`.

```
nodectrl/<nodeid>/ctrl/<action>/<base64 encoded DER certificate>/<base64 encoded signature>

```

## Signature calculation

The following fields are hashed with SHA256 and then the result is signed with RSA-PSS using
the sender's private key.

```
- the one use cookie from the node heartbeat
- the original target topic without the cert and signature elements
- the publish payload if there is one
```
