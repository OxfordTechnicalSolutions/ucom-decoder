from ucompy import UcomDbu, UcomSignal, UcomMessage, UcomData
import sys

"""Get all the message and signal information from the .dbu file
"""

u = UcomDbu("dbu\\oxts.dbu")
if not u.get_valid():
    print("Error reading .dbu file")
    sys.exit()

for key,value in u.get_messages().items():
    print(f"{key}: {value}")
    for signal in value.get_signals():
        print(f"{signal.get_signal_id()}")