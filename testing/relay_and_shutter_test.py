# -*- coding: utf-8 -*-
"""
Created on Tue Aug 24 12:44:39 2021
@author: Rodrigo Delgado
Notes for PEP:
    Package and modules names: all lowercase, underscores when neccesary
    Functions: lowercase
    Class names: CapWords
    Variables: CapWords, preferred lowercase
    Exceptions: CapWords, with distinct prefix (e.g. ErrorWhatever)
    Constants: Uppercase
    
    
TODO
- Implement the reading of serial connection to find the state of every channel all the time
"""

# =============================================================================
# Import additional modules, if fail, raise an error
# =============================================================================
try:
    import os
    import numpy as np
    import matplotlib.pyplot as plt
    import tkinter as tk
    from tkinter import ttk
    from tkinter import filedialog
    from tkinter import messagebox
    import serial
    

except ModuleNotFoundError as imp_error:
    print("Import Error: {0}\n".format(imp_error))
    print("Install the missing modules using: pip install modulename")
    
# =============================================================================
# Your code start here
# =============================================================================

# =============================================================================
# Create some global variables to use through the program
# =============================================================================
baudrate = 38400
# port_list = ["COM0", "COM2", "COM3", "COM4", "COM5"]
port_list = ["/dev/ttyUSB0", "/dev/ttyUSB1"]
port_list.insert(0, "-- select --")
status_colors = {"ON":"green", "OFF":"red"}
connection_status = "OFF"


# =============================================================================
# Define different functions for the menu buttons
# =============================================================================

# Dummy function for the buttons
def donothing():
    filewin = tk.Toplevel(window)
    button = tk.Button(filewin, text="Do nothing button")
    button.pack()


def connectToArduino():
    # =============================================================================
    # Receive and send instructions to Arduino
    # =============================================================================
    try:
        try:
            global ArduinoSerial
            ArduinoSerial = serial.Serial(selected_port, baudrate)
            if ArduinoSerial.readline():
                global connection_status
                connection_status = "ON"
                status_led.create_oval(coord, fill=status_colors[connection_status])
            else:
                messagebox.showwarning("Warning", "Connection can not be stablished")
        except serial.serialutil.SerialException:
            pass
    except NameError:
        messagebox.showwarning("Warning","No port has been selected")
  
def closeConnection():
    ArduinoSerial.close()
    global connection_status
    connection_status = "OFF"
    status_led.create_oval(coord, fill=status_colors[connection_status])
    
def ComboboxPort(event):
    global selected_port
    selected_port = port_select.get()
    
    
def send_data(channel, relay, shutter):
    if connection_status == "OFF":
       messagebox.showerror("Error", "No connection found")
    else:
         text_to_send = "<"+str(channel)+","+str(relay)+","+str(shutter)+">"
         print(text_to_send)
         ArduinoSerial.write(text_to_send.encode("utf-8"))

# =============================================================================
# Set up GUI
# =============================================================================
window = tk.Tk()

window.geometry('500x650')
window.title("Multiplexer Control")
window.resizable(False, False)

# =============================================================================
# Create the drop-down menu and the drop-down buttons
# =============================================================================
menubar = tk.Menu(window)
# Create "File" menu button and all sub-buttons
filemenu = tk.Menu(menubar, tearoff=0)
filemenu.add_command(label="Close connection", command=closeConnection)
menubar.add_cascade(label="File", menu=filemenu)
# Create "Help" menu button and all sub-buttons
helpmenu = tk.Menu(menubar, tearoff=0)
helpmenu.add_command(label="Help Index", command=donothing)
helpmenu.add_command(label="About...", command=donothing)
menubar.add_cascade(label="Help", menu=donothing)

window.config(menu=menubar)

# =============================================================================
# Create main frame to place the window widgets
# =============================================================================
mainframe = tk.Frame(window)
mainframe.grid(column=0, row=0, sticky=("N", "W", "E", "S"))

window.columnconfigure(0, weight=1)
window.rowconfigure(0, weight=1)	

## Create Label frame containing the connection setup
connection_setup = tk.LabelFrame(mainframe, text="Connection Setup")
connection_setup.grid(column=0, row=0, pady=10, padx=10, columnspan=3)

port_desc = tk.Label(connection_setup, text="Port")
port_desc.grid(column=0, row=0, sticky=("N","W"), pady=10, padx=10)

port_select = ttk.Combobox(connection_setup, values=port_list)
port_select.grid(column=0, row=1, sticky=("N", "W"), pady=10, padx=10)
port_select.bind("<<ComboboxSelected>>", ComboboxPort)

baud_desc = tk.Label(connection_setup, text="Baudrate")
baud_desc.grid(column=1, row=0, sticky=("N","W"), pady=10, padx=10)

baud_var = tk.StringVar()
baud_value = tk.Entry(connection_setup, width=12,textvariable=baud_var, state="readonly")
baud_var.set(str(baudrate))
baud_value.grid(column=1, row=1, sticky=("N", "W"), pady=10, padx=10)

connect_button = tk.Button(connection_setup, text="Connect", command=connectToArduino)
connect_button.grid(column=2, row=1, sticky="W")

status_label = tk.Label(connection_setup, text="Status")
status_label.grid(column=3, row=0, sticky=("N","E"), pady=10, padx=65)


# Create connection status LED
status_led= tk.Canvas(connection_setup, height=20, width=20)
status_led.grid(column=3, row=1, sticky=("N","W"), pady=10, padx=75)

coord = 5,5,15,15
status_led.create_oval(coord, fill=status_colors[connection_status])




class Channel():
    def __init__(self, number, col, row):
        self.n = number
        self.col = col
        self.row = row
        self.status = {"relay": "OFF", "shutter": "OFF"}
        self.bit_equivalent = {"OFF": 0, "ON": 1}
        self.frame_label = tk.LabelFrame(mainframe, text="Channel {}".format(number))
        self.frame_label.grid(column=self.col, row=self.row, sticky=("N","W"), pady=10, padx=10)
        
        self.relay_value = tk.IntVar()
        self.shutter_value = tk.IntVar()
        self.relay_check = tk.Checkbutton(self.frame_label, text="Relay", variable=self.relay_value)
        self.relay_check.grid(column=0, row=0, sticky=("W"), pady=10, padx=10)
        
        self.shutter_check = tk.Checkbutton(self.frame_label, text="Shutter", variable=self.shutter_value)
        self.shutter_check.grid(column=1, row=0, sticky=("W"), pady=10, padx=10)
        
        self.open_button = tk.Button(self.frame_label, text="Open", 
                             command=self.open_and_send)
        self.open_button.grid(column=2, row=0, sticky=("W"), pady=10, padx=10)
        
        self.close_button = tk.Button(self.frame_label, text="Close", 
                             command=self.close_and_send)
        self.close_button.grid(column=2, row=1, sticky=("W"), pady=10, padx=10)
        
        self.status_led_relay = tk.Canvas(self.frame_label, height=15, width=15)
        self.status_led_relay.grid(column=0, row=1, sticky=("N","W"), pady=10, padx=10)
        self.status_led_shutter = tk.Canvas(self.frame_label, height=15, width=15)
        self.status_led_shutter.grid(column=1, row=1, sticky=("N","W"), pady=10, padx=10)
        
        coord = 5,5,15,15
        self.status_led_relay.create_oval(coord, fill=status_colors[self.status["relay"]])
        self.status_led_shutter.create_oval(coord, fill=status_colors[self.status["shutter"]])
        
    def print_values(self):
        print("Relay: {}; Shutter: {}".format(self.relay_value.get(), self.shutter_value.get()))
        
    # def open_and_send_debug(self):
    #     if self.relay_value.get() == 1:
    #         self.status["relay"] = "ON"
    #         self.status_led_relay.create_oval(coord, fill=status_colors[self.status["relay"]])
    #     if self.shutter_value.get() == 1:
    #         self.status["shutter"] = "ON"
    #         self.status_led_shutter.create_oval(coord, fill=status_colors[self.status["shutter"]])
    #     text_to_send = "<"+str(self.n)+","+str(self.bit_equivalent[self.status["relay"]])+","+str(self.bit_equivalent[self.status["shutter"]])+">"
    #     print(text_to_send)
        
    # def close_and_send_debug(self):
    #     if self.relay_value.get() == 1:
    #         self.status["relay"] = "OFF"
    #         self.status_led_relay.create_oval(coord, fill=status_colors[self.status["relay"]])
    #     if self.shutter_value.get() == 1:
    #         self.status["shutter"] = "OFF"
    #         self.status_led_shutter.create_oval(coord, fill=status_colors[self.status["shutter"]])
    #     text_to_send = "<"+str(self.n)+","+str(self.bit_equivalent[self.status["relay"]])+","+str(self.bit_equivalent[self.status["shutter"]])+">"
    #     print(text_to_send)
        
    def open_and_send(self):
        if connection_status == "OFF":
            messagebox.showerror("Error", "No connection found")
        else:
            if self.relay_value.get() == 1:
                self.status["relay"] = "ON"
                self.status_led_relay.create_oval(coord, fill=status_colors[self.status["relay"]])
            if self.shutter_value.get() == 1:
                self.status["shutter"] = "ON"
                self.status_led_shutter.create_oval(coord, fill=status_colors[self.status["shutter"]])
            text_to_send = "<"+str(self.n)+","+str(self.bit_equivalent[self.status["relay"]])+","+str(self.bit_equivalent[self.status["shutter"]])+">"
            print(text_to_send)
            ArduinoSerial.write(text_to_send.encode("utf-8"))
            
            
    def close_and_send(self):
        if connection_status == "OFF":
            messagebox.showerror("Error", "No connection found")
        else:
            if self.relay_value.get() == 1:
                self.status["relay"] = "OFF"
                self.status_led_relay.create_oval(coord, fill=status_colors[self.status["relay"]])
            if self.shutter_value.get() == 1:
                self.status["shutter"] = "OFF"
                self.status_led_shutter.create_oval(coord, fill=status_colors[self.status["shutter"]])
            text_to_send = "<"+str(self.n)+","+str(self.bit_equivalent[self.status["relay"]])+","+str(self.bit_equivalent[self.status["shutter"]])+">"
            print(text_to_send)
            ArduinoSerial.write(text_to_send.encode("utf-8"))


channel1 = Channel(0, 0, 1)
channel2 = Channel(1, 0, 2)
channel3 = Channel(2, 0, 3)
channel4 = Channel(3, 0, 4)

channel5 = Channel(4, 1, 1)
channel6 = Channel(5, 1, 2)
channel7 = Channel(6, 1, 3)
channel8 = Channel(7, 1, 4)










window.mainloop()