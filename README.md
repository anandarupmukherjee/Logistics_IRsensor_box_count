# LOGISTICS_IRsensor_box_count
This is a demo solution for counting boxes moving on a linear conveyor belt during container unloading.

This solution uses a M5 stack core2 arduino board with IR proximity sensors attached to it. The M5 board connects to a wifi network and pushes data to a Raspberry Pi server over MQTT. The docker containers in the raspberry pi recieve the data over MQTT, stores it into an Influxdb timeseries DB and visualizes the live status of the conveyor belt on a Grafana dashboard.

![image](https://user-images.githubusercontent.com/11146716/221406344-a090206b-db8a-4dcb-af39-6089e5589398.png)

