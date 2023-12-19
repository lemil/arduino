import requests
import json
import getopt
import sys
import time



base_url = "http://192.168.0.119"
hello_url = base_url + "/"
cmd_url = base_url + "/cmd"

setup_file = "setup_machine2"

retry_sleep = 5
retry_max_attemps = 5

def send_command(url,cmd):

    success = False
    attemps = 0
    while not success:
        headers = {'Content-type': 'application/json', 'Accept': 'text/plain'}

        body = { 
            "cmd": cmd
        }

        print(json.dumps(body))

        try:
            resp = requests.post(url,headers=headers,data=json.dumps(body),timeout=8)

            print(resp.text)
        except ex:
            setup_machine()


        if resp.status_code == 200 or attemps > retry_max_attemps:
            success = True
        else:
            attemps +=1
            time.sleep(retry_sleep)

    return success

def process_file(cmd_file):
    my_file = open(cmd_file,'r')
    lines = my_file.readlines()
    my_file.close()

    for line in lines:
        cmd = line.replace("\r","").replace("\n","")

        if cmd[0] != "C":
            continue

        send_command(cmd_url,cmd)
        

def check_is_up():
    is_up = False

    while not is_up:
        resp = requests.get(hello_url)

        is_up = resp.status_code == 200

        if is_up:
            print("Connected!")
            return
        
        print("Service down, sleep and retry")
        
        time.sleep(retry_sleep*5)


def setup_machine(setup_file):
    process_file(setup_file)
    time.sleep(2)


def check_args(argv):

    if len(argv) == 1:
        return argv[0]
    else:
        # Print something useful
        print ('usage: polargraph.py <filename> ')
        sys.exit(2)

if __name__ == "__main__":

    filename = check_args(sys.argv[1:])

    print(f"Opening \"{filename}\"")

    check_is_up()

    setup_machine(setup_file)

    process_file(filename)