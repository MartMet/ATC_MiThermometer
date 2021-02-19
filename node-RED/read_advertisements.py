import re
import struct
import json
import textwrap
import subprocess


def execute_ble_scan():
    temp = subprocess.Popen(["sudo", "blescan", "-t" ,"15"], stdout=subprocess.PIPE)
    temp.wait()
    return temp.communicate()[0].decode('utf-8')


def regex_search(ble_string):
    # UUID 0x181A - size 19: custom - temperature x0.01C, humidity x0.01%, battery voltage in mV, battery charge
    # level 0..100%, GPIO-pin flags (mark reset) and triggers.
    pattern = r"<1a18\S{30}>"
    return re.findall(pattern, ble_string)


def convert_to_json(list_of_advertisments):
    temp_list = []
    for list_element in list_of_advertisments:
        temp_dict = {}

        # strip '<' '>'
        list_element = list_element[1:-1]

        hex_data = bytearray.fromhex(list_element)

        # battery in mV
        battery = struct.unpack('<H', hex_data[12:14])[0]
        # battery in percent
        battery_percent = hex_data[14]
        # humidity in 0.01%
        humidity = float(struct.unpack('<H', hex_data[10:12])[0]) * 0.01
        # temperature in  0.01C
        temperature = float(struct.unpack('<H', hex_data[8:10])[0]) * 0.01
        # convert mac address
        mac_address = ':'.join((list(reversed(textwrap.wrap(list_element[4:16], 2)))))

        temp_dict['mac_address'] = mac_address
        temp_dict['temperature'] = temperature
        temp_dict['humidity'] = humidity
        temp_dict['battery_percent'] = battery_percent
        temp_dict['battery'] = battery
        temp_list.append(temp_dict)

    json_string = json.dumps({"values": temp_list})
    print(json_string)
    pass


if __name__ == '__main__':
    ble_result = execute_ble_scan()
    list_of_advertisements = regex_search(ble_result)
    convert_to_json(list_of_advertisements)
