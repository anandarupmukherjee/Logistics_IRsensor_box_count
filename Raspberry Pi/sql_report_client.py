# -*- coding: utf-8 -*-
"""
Created on Tue Oct 27 12:33:25 2020

@author: Anand
"""
import mysql.connector
import os
from datetime import datetime
import time as t

# mydb = mysql.connector.connect(host="localhost",user="pi",password="admin",database="hospital")  #--> for local development server
mydb = mysql.connector.connect(host="172.18.0.5",user="root",password="admin",database="anand_logistics")  #--->for rpi deployment version

records=[]
##################SELECT ALL######################################
mycursor = mydb.cursor()
mycursor.execute("SELECT * FROM unload_dock WHERE (inp LIKE '%1%') AND (outp LIKE '%1%')")
myresult = mycursor.fetchall()



def idb_push(loc,mesg):
    dev="unloader"
    loc=loc
    mesg=mesg
    var="curl -i -XPOST 'http://172.18.0.2:8086/write?db=logistics' --data-binary '"+dev+" "+loc+"="+mesg+"'"
    os.system(var)


box_count=0
total_box=2000


while (((box_count*100)/total_box)<=100):
    for x in myresult:
    #    print(x)
        records.append(x)
        box_count+=1


    print("GENERATED UNLOAD REPORT")
    print("---------------------------------------") 
    #first objective: count number of unloaded boxes
    print("No. of boxes unloaded = " + str(box_count))
    idb_push("box_count",str(box_count))
    print("---------------------------------------")
    #second objective: count how many left assuming 20 boxes
    print("No. of boxes left = " + str(total_box-box_count))
    idb_push("box_left",str(total_box-box_count))
    print("---------------------------------------")
    print("% completion = " + str((box_count*100)/total_box)+"%")
    idb_push("completion",str((box_count*100)/total_box))
    print("---------------------------------------")
    #third objective: time elapsed per box unload
    ts1=int(records[0][0])
    ts2=int(records[box_count-1][0])
    

    #print(ts1)
    #print(ts2)
    #
    #print("---------------------------------------")
    #
    #converted_d1 = datetime.fromtimestamp(round(ts1 / 1000))
    #current_time_utc = datetime.fromtimestamp(round(ts2 / 1000))

    print(datetime.utcfromtimestamp(ts1).strftime('%Y-%m-%d %H:%M:%S'))
    print(datetime.utcfromtimestamp(ts2).strftime('%Y-%m-%d %H:%M:%S'))
    print('Total time taken to unload boxes (minutes):')
    print((ts2-ts1)/(60))
    idb_push("time_taken",str((ts2-ts1)/(60)))
    print("---------------------------------------")
    t.sleep(1)



