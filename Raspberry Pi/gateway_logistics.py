#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Jul  6 19:45:52 2021

@author: anandarupmukherjee
"""
import mysql.connector
import paho.mqtt.client as mqtt
import os
import time as t
import json
from datetime import datetime

# mydb = mysql.connector.connect(host="localhost",user="pi",password="admin",database="hospital")  #--> for local development server
mydb = mysql.connector.connect(host="172.18.0.5",user="root",password="admin",database="anand_logistics")  #--->for rpi deployment version


HOST = '172.18.0.4'
PORT = 1883
MQTT_TOPIC = [("log/belt1/status",0)]

def fmtRESTapi(topc, mesg):
    dev=topc.split("/")[0]
    loc=topc.split("/")[1]
    data = "{}".format(mesg)
    message = {"message" : data}
    db_update(data,"belt1") #ideally change the 2nd args to be updated from list automatically
    
#    print("Published: '" + json.dumps(message) + "' to the topic: " + "'log/belt1/status'")
    t.sleep(0.1)
    return mesg

def rest_api(topc, load):
    msg=str(load.decode("utf-8"))
    top=str(topc)
#    print("message: ", msg)
#    print("topic: ", top)
    fmtRESTapi(top,msg)


def on_connect(client, userdata, flags, rc):
    # Subscribe to any topic starting with 'hermes/intent/'
    # client.subscribe('DIAL/192.168.1.23/Irms')
    client.subscribe(MQTT_TOPIC)
    

def on_message(client, userdata, msg):
    print("Message received on topic {0}: {1}"\
        .format(msg.topic, msg.payload))
    rest_api(msg.topic, msg.payload)


def db_update(data,loc):
    inp_sta=data.split("I:")[1].split(',')[0]
    out_sta=data.split("O:")[1].split(',')[0]
    tim_stmp=data.split("tInS:")[1].split('}')[0]
    print("play with: "+inp_sta+"-"+out_sta+"-"+tim_stmp)
    mycursor = mydb.cursor()

    sql = "INSERT INTO unload_dock (ts, inp, outp, location) VALUES (%s, %s, %s, %s)"
    val = (tim_stmp, inp_sta, out_sta,loc)
    mycursor.execute(sql, val)
    mydb.commit()
    print(mycursor.rowcount, "record inserted.")
    
     

                    
                    
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(HOST, PORT, 60)
client.loop_forever()


#print('Publish End')
#myAWSIoTMQTTClient.disconnect()
