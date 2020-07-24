

### apt source
pi@rak-gateway:/etc/apt/sources.list.d $ ls
chirpstack.list  mosquitto-buster.list  raspi.list
pi@rak-gateway:/etc/apt/sources.list.d $ cat mosquitto-buster.list 
deb https://repo.mosquitto.org/debian buster main
pi@rak-gateway:/etc/apt/sources.list.d $ cat chirpstack.list 
deb https://artifacts.chirpstack.io/packages/3.x/deb stable main
pi@rak-gateway:/etc/apt/sources.list.d $ cat raspi.list 
deb http://archive.raspberrypi.org/debian/ buster main
# Uncomment line below then 'apt-get update' to enable 'apt-get source'
#deb-src http://archive.raspberrypi.org/debian/ buster main

---
### setup MQTT bridge (use v1.6.10)

pi@rak-gateway:/etc/mosquitto/conf.d $ cat formosa.conf 

# 長庚大學
# Broker IP/Server IP:
#   120.126.23.14:1883
# 彩原
#   118.163.94.171:1883
#
# connection dash-brige

connection dash-brige
address 118.163.94.171:1883
cleansession false
notifications false
keepalive_interval 300
try_private false
clientid Formosa0001
start_type automatic
bridge_protocol_version mqttv311
topic #  both 0 application/2/device/ none/FORMOSA00001/

### setup MQTT topic for dashboard in chirpstack-application-server.toml
---
root@rak-gateway:/etc/chirpstack-application-server# cat chirpstack-application-server.toml 
# This configuration sets the required settings and configures an integration
# with a MQTT broker. Many options and defaults have been omitted for
# simplicity.
#
# See https://www.chirpstack.io/application-server/install/config/ for a full
# configuration example and documentation.


# PostgreSQL settings.
#
# Please note that PostgreSQL 9.5+ is required.
[postgresql]
# PostgreSQL dsn (e.g.: postgres://user:password@hostname/database?sslmode=disable).
#
# Besides using an URL (e.g. 'postgres://user:password@hostname/database?sslmode=disable')
# it is also possible to use the following format:
# 'user=chirpstack_as dbname=chirpstack_as sslmode=disable'.
#
# The following connection parameters are supported:
#
# * dbname - The name of the database to connect to
# * user - The user to sign in as
# * password - The user's password
# * host - The host to connect to. Values that start with / are for unix domain sockets. (default is localhost)
# * port - The port to bind to. (default is 5432)
# * sslmode - Whether or not to use SSL (default is require, this is not the default for libpq)
# * fallback_application_name - An application_name to fall back to if one isn't provided.
# * connect_timeout - Maximum wait for connection, in seconds. Zero or not specified means wait indefinitely.
# * sslcert - Cert file location. The file must contain PEM encoded data.
# * sslkey - Key file location. The file must contain PEM encoded data.
# * sslrootcert - The location of the root certificate file. The file must contain PEM encoded data.
#
# Valid values for sslmode are:
#
# * disable - No SSL
# * require - Always SSL (skip verification)
# * verify-ca - Always SSL (verify that the certificate presented by the server was signed by a trusted CA)
# * verify-full - Always SSL (verify that the certification presented by the server was signed by a trusted CA and the server host name matches the one in the certificate)
# dsn="postgres://localhost/chirpstack_as?sslmode=disable"
dsn="postgres://chirpstack_as:dbpassword@localhost/chirpstack_as?sslmode=disable"


# Redis settings
#
# Please note that Redis 2.6.0+ is required.
[redis]
# Redis url (e.g. redis://user:password@hostname/0)
#
# For more information about the Redis URL format, see:
# https://www.iana.org/assignments/uri-schemes/prov/redis
url="redis://localhost:6379"


# Application-server settings.
[application_server]

  # Integration configures the data integration.
  #
  # This is the data integration which is available for all applications,
  # besides the extra integrations that can be added on a per-application
  # basis.
  [application_server.integration]
  # Enabled integrations.
  enabled=["mqtt"]

    # MQTT integration backend.
    [application_server.integration.mqtt]
    # MQTT topic templates for the different MQTT topics.
    #
    # The meaning of these topics are documented at:
    # https://www.chirpstack.io/application-server/integrate/data/
    #
    # The following substitutions can be used:
    # * "{{ .ApplicationID }}" for the application id.
    # * "{{ .DevEUI }}" for the DevEUI of the device.
    #
    # Note: the downlink_topic_template must contain both the application id and
    # DevEUI substitution!
    uplink_topic_template="application/{{ .ApplicationID }}/device/{{ .DevEUI }}/t"
    downlink_topic_template="application/{{ .ApplicationID }}/device/{{ .DevEUI }}/tx"
    join_topic_template="application/{{ .ApplicationID }}/device/{{ .DevEUI }}/join"
    ack_topic_template="application/{{ .ApplicationID }}/device/{{ .DevEUI }}/ack"
    error_topic_template="application/{{ .ApplicationID }}/device/{{ .DevEUI }}/error"
    status_topic_template="application/{{ .ApplicationID }}/device/{{ .DevEUI }}/status"
    location_topic_template="application/{{ .ApplicationID }}/device/{{ .DevEUI }}/location"

    # MQTT server (e.g. scheme://host:port where scheme is tcp, ssl or ws)
    server="tcp://localhost:1883"

    # Connect with the given username (optional)
    username=""

    # Connect with the given password (optional)
    password=""


    # Settings for the "internal api"
    #
    # This is the API used by ChirpStack Network Server to communicate with ChirpStack Application Server
    # and should not be exposed to the end-user.
    [application_server.api]
    # ip:port to bind the api server
    bind="0.0.0.0:8001"

    # Public ip:port of the application-server API.
    #
    # This is used by ChirpStack Network Server to connect to ChirpStack Application Server. When running
    # ChirpStack Application Server on a different host than ChirpStack Network Server, make sure to set
    # this to the host:ip on which ChirpStack Network Server can reach ChirpStack Application Server.
    # The port must be equal to the port configured by the 'bind' flag
    # above.
    public_host="localhost:8001"


    # Settings for the "external api"
    #
    # This is the API and web-interface exposed to the end-user.
    [application_server.external_api]
    # ip:port to bind the (user facing) http server to (web-interface and REST / gRPC api)
    bind="0.0.0.0:8080"

    # http server TLS certificate (optional)
    tls_cert=""

    # http server TLS key (optional)
    tls_key=""

    # JWT secret used for api authentication / authorization
    # You could generate this by executing 'openssl rand -base64 32' for example
    jwt_secret="verysecret"


# Join-server configuration.
#
# ChirpStack Application Server implements a (subset) of the join-api specified by the
# LoRaWAN Backend Interfaces specification. This API is used by ChirpStack Network Server
# to handle join-requests.
[join_server]
# ip:port to bind the join-server api interface to
bind="0.0.0.0:8003"

### setup LoRaWAN channel in chirpstack-network-server.toml (base on chirpstack-network-server.us_902_928.toml)
root@rak-gateway:/etc/chirpstack-network-server# cat chirpstack-network-server.toml
# This configuration configures ChirpStack Network Server for the EU868 band using a MQTT
# broker to communicate with the gateways. Many options and defaults have been
# omitted for simplicity.
# 
# For other bands, see the ./examples/ sub-directory.
#
# See https://www.chirpstack.io/network-server/install/config/ for a full
# configuration example and documentation.


# PostgreSQL settings.
#
# Please note that PostgreSQL 9.5+ is required.
[postgresql]
# PostgreSQL dsn (e.g.: postgres://user:password@hostname/database?sslmode=disable).
#
# Besides using an URL (e.g. 'postgres://user:password@hostname/database?sslmode=disable')
# it is also possible to use the following format:
# 'user=chirpstack_ns dbname=chirpstack_ns sslmode=disable'.
#
# The following connection parameters are supported:
#
# * dbname - The name of the database to connect to
# * user - The user to sign in as
# * password - The user's password
# * host - The host to connect to. Values that start with / are for unix domain sockets. (default is localhost)
# * port - The port to bind to. (default is 5432)
# * sslmode - Whether or not to use SSL (default is require, this is not the default for libpq)
# * fallback_application_name - An application_name to fall back to if one isn't provided.
# * connect_timeout - Maximum wait for connection, in seconds. Zero or not specified means wait indefinitely.
# * sslcert - Cert file location. The file must contain PEM encoded data.
# * sslkey - Key file location. The file must contain PEM encoded data.
# * sslrootcert - The location of the root certificate file. The file must contain PEM encoded data.
#
# Valid values for sslmode are:
#
# * disable - No SSL
# * require - Always SSL (skip verification)
# * verify-ca - Always SSL (verify that the certificate presented by the server was signed by a trusted CA)
# * verify-full - Always SSL (verify that the certification presented by the server was signed by a trusted CA and the server host name matches the one in the certificate)
dsn="postgres://chirpstack_ns:dbpassword@localhost/chirpstack_ns?sslmode=disable"


# Redis settings
#
# Please note that Redis 2.6.0+ is required.
[redis]
# Redis url (e.g. redis://user:password@hostname/0)
#
# For more information about the Redis URL format, see:
# https://www.iana.org/assignments/uri-schemes/prov/redis
url="redis://localhost:6379"


# Network-server settings.
[network_server]
# Network identifier (NetID, 3 bytes) encoded as HEX (e.g. 010203)
net_id="000000"


  # LoRaWAN regional band configuration.
  #
  # Note that you might want to consult the LoRaWAN Regional Parameters
  # specification for valid values that apply to your region.
  # See: https://www.lora-alliance.org/lorawan-for-developers
  [network_server.band]
  name="US_902_928"


  # LoRaWAN network related settings.
  [network_server.network_settings]
  enabled_uplink_channels=[8,9,10,11,12,13,14,15]
  # Disable ADR
  #
  # When set, this globally disables ADR.
  disable_adr=true
  
    # Extra channel configuration.
    #
    # Use this for LoRaWAN regions where it is possible to extend the by default
    # available channels with additional channels (e.g. the EU band).
    # The first 5 channels will be configured as part of the OTAA join-response
    # (using the CFList field).
    # The other channels (or channel / data-rate changes) will be (re)configured
    # using the NewChannelReq mac-command.
    #
#    [[network_server.network_settings.extra_channels]]
#    frequency=867100000
#    min_dr=0
#    max_dr=5

#    [[network_server.network_settings.extra_channels]]
#    frequency=867300000
#    min_dr=0
#    max_dr=5

#    [[network_server.network_settings.extra_channels]]
#    frequency=867500000
#    min_dr=0
#    max_dr=5

#    [[network_server.network_settings.extra_channels]]
#    frequency=867700000
#    min_dr=0
#    max_dr=5

#    [[network_server.network_settings.extra_channels]]
#    frequency=867900000
#    min_dr=0
#    max_dr=5


    # Class B settings
    [network_server.network_settings.class_b]
    # Ping-slot data-rate.
    ping_slot_dr=0

    # Ping-slot frequency (Hz)
    #
    # Set this to 0 to use the default frequency plan for the configured region
    # (which could be frequency hopping).
    ping_slot_frequency=0


  # Network-server API
  #
  # This is the network-server API that is used by ChirpStack Application Server or other
  # custom components interacting with ChirpStack Network Server.
  [network_server.api]
  # ip:port to bind the api server
  bind="0.0.0.0:8000"


  # Backend defines the gateway backend settings.
  #
  # The gateway backend handles the communication with the gateway(s) part of
  # the LoRaWAN network.
  [network_server.gateway.backend]
  # Backend
  type="mqtt"


    # MQTT gateway backend settings.
    #
    # This is the backend communicating with the LoRa gateways over a MQTT broker.
    [network_server.gateway.backend.mqtt]
    # MQTT topic templates for the different MQTT topics.
    #
    # The meaning of these topics are documented at:
    # https://www.chirpstack.io/gateway-bridge/
    #
    # The default values match the default expected configuration of the
    # ChirpStack Gateway Bridge MQTT backend. Therefore only change these values when
    # absolutely needed.

    # Event topic template.
    event_topic="gateway/+/event/+"

    # Command topic template.
    #
    # Use:
    #   * "{{ .GatewayID }}" as an substitution for the LoRa gateway ID
    #   * "{{ .CommandType }}" as an substitution for the command type
    command_topic_template="gateway/{{ .GatewayID }}/command/{{ .CommandType }}"

    # MQTT server (e.g. scheme://host:port where scheme is tcp, ssl or ws)
    server="tcp://localhost:1883"

    # Connect with the given username (optional)
    username=""

    # Connect with the given password (optional)
    password=""


# Metrics collection settings.
[metrics]
# Timezone
#
# The timezone is used for correctly aggregating the metrics (e.g. per hour,
# day or month).
# Example: "Europe/Amsterdam" or "Local" for the the system's local time zone.
timezone="Local"


# Join-server settings.
[join_server]

  # Default join-server settings.
  #
  # This join-server will be used when resolving the JoinEUI is set to false
  # or as a fallback when resolving the JoinEUI fails.
  [join_server.default]
  # hostname:port of the default join-server
  #
  # This API is provided by ChirpStack Application Server.
  server="http://localhost:8003"


### configuration sakura frp service
---
pi@rak-gateway:~ $ cat /lib/systemd/system/sakurafrp.service 
[Unit]
Description=Sakura Frp Client
After=network.target auditd.service
ConditionPathExists=!/etc/sakurafrp/frpc_not_to_be_run

[Service]
User=root
WorkingDirectory=/opt/sakurafrp/
LimitNOFILE=4096
PIDFile=/run/sakurafrp/client.pid
ExecStart=/opt/sakurafrp/frpc_linux_arm -c /etc/sakurafrp/frpc.ini
Restart=on-failure
RuntimeDirectory=sakurafrpc
RuntimeDirectoryMode=755
StartLimitInterval=600

[Install]
WantedBy=multi-user.target
Alias=sakurafrpc.service

---

pi@rak-gateway:~ $ cat /etc/sakurafrp/frpc.ini 
[common]
server_addr = 109.166.36.56
server_port = 7000
tcp_mux = true
pool_count = 1
protocol = tcp
user = e6fd505510fc3bef
token = SakuraFrpClientToken
dns_server = 114.114.114.114
 
[formosa123]
privilege_mode = true
type = tcp
local_ip = 127.0.0.1
local_port = 22
remote_port = 10262
use_encryption = false
use_compression = true
 
[chripstack]
privilege_mode = true
type = http
local_ip = 127.0.0.1
local_port = 8080
custom_domains = chirpstack.uwingstech.com
use_encryption = false
use_compression = true
