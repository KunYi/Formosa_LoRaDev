http
http://chirpstack.uwingstech.com/

account: user
password: user123


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


chripstack
user:admin
password:Formosa123

ssh pi@jp-tyo-dvm.sakurafrp.com -p 10262
gateway, SSh
user:pi
password:Formosa123



