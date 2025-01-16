
def get_port():
    from os import remove
    from os.path import dirname, abspath

    port_info = dirname(abspath(__file__)) + "/port.info"

    try:
        f = open(port_info, "r")
        port_num = int(f.read())
        
        f.close()
        remove(port_info)

        print(f"Using Port '{port_num}'")
        return port_num

    except:
        print("Port Not Parsed. Using '39595'")
        return 39595

trick.frame_log_on()
trick.real_time_enable()
trick.exec_set_software_frame(0.1)
trick.itimer_enable()

trick.exec_set_enable_freeze(True)
trick.exec_set_freeze_command(True)

portnum = get_port()

trick.var_server_set_port(portnum)