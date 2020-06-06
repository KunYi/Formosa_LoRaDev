DEBUG
===

1. use ssh to login gateway first
user:pi
password:raspberry

2. observer MQTT message
run "mosquitto_sub -t "application/2/device/#" -d"

3. open new terminal to login gateway, use step 1 account infomration

4. send MQTT message for test
run "mosquitto_pub -t "application/2/device/007032fffe010000/tx" -d -f per.json"

   per.json context like the below, show how to change update peridiod
   ---
   '''
   {
    "confirmed":true,
    "fPort": 12,
    "object": { "Control": "Peridiod", "Second": 120 }
   }
   '''





