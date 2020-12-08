/*
 * Copyright 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

import subprocess
import os
from multiprocessing import Process
from multiprocessing import Manager
import time
import threading
import sys


def timer_set(wait, string):
    wait[string] = False


def run_cf_analyzer(i, size, wait):

    timer = threading.Timer(2.0, timer_set, [wait, 'ad_network'])
    timer.start()
    analyzer_run_cmd = "./network_analyzer"
    analyzer_run_cmd = analyzer_run_cmd.split()

    output, err = subprocess.Popen(analyzer_run_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()


    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "bind error" in err or "bind error" in output:
        timer.cancel()
        return run_cf_analyzer(i, size, wait)
    if "Signal 11" in err or "Signal 11" in output:
        timer.cancel()
        return run_cf_analyzer(i, size, wait)


    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/cf_base_result/" + size + "/cf_analyzer_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return


def run_cf_ad_network(i, size, wait):

    while True:
        if not wait['ad_network']:
            break

    timer = threading.Timer(2.0, timer_set, [wait, 'client'])
    timer.start()
    cf_ad_network_run_cmd = "./cf_ad_network"
    cf_ad_network_run_cmd = cf_ad_network_run_cmd.split()

    output, err = subprocess.Popen(cf_ad_network_run_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "bind error" in err or "bind error" in output:
        timer.cancel()
        return run_cf_ad_network(i, size, wait)
    if "Signal 11" in err or "Signal 11" in output:
        timer.cancel()
        return run_cf_ad_network(i, size, wait)


    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/cf_base_result/" + size + "/cf_ad_network_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return


def run_cf_client(i, size, wait):
    
   
    while True:
        if not wait['ad_network'] and not wait['client']:
            break

    cf_client_cmd = "/home/mkpark/test_program/clients/src_base/cf_client 127.0.0.1 5000"
    cf_client_cmd = cf_client_cmd.split()

    output, err = subprocess.Popen(cf_client_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "Connection failed" in output:
        return run_cf_client(i, size, wait)

    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/cf_base_result/" + size + "/cf_client_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return


    
def run(i, size, manager):

    wait = manager.dict()
    wait['ad_network'] = True
    wait['client'] = True

    procs = []

    proc = Process(target=run_cf_analyzer, args=(i, size, wait,))
    procs.append(proc)
    proc.start()

    proc = Process(target=run_cf_ad_network, args=(i, size, wait,))
    procs.append(proc)
    proc.start()

    proc = Process(target=run_cf_client, args=(i, size, wait,))
    procs.append(proc)
    proc.start()

    for proc in procs:
        proc.join()


if __name__ == "__main__":

    size = sys.argv[1]

    manager = Manager()
    for i in range(10):
        run(i, size, manager)

