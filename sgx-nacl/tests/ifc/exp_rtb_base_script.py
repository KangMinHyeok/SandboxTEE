# Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
# Use of this source code is governed by a GPLv2 license that can be
# found in the LICENSE file.

import subprocess
import os
from multiprocessing import Process
from multiprocessing import Manager
import time
import threading
import sys

delay = 2.0

def timer_set(wait, string):
    wait[string] = False


def run_rtb_analyzer1(i, size, wait):

    timer = threading.Timer(delay, timer_set, [wait, 'dsp_a'])
    timer.start()
    analyzer_run_cmd = "./rtb_analyzer1"
    analyzer_run_cmd = analyzer_run_cmd.split()

    output, err = subprocess.Popen(analyzer_run_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()


    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "bind error" in err or "bind error" in output:
        timer.cancel()
        return run_rtb_analyzer1(i, size, wait)
    if "Signal 11" in err or "Signal 11" in output:
        timer.cancel()
        return run_rtb_analyzer1(i, size, wait)


    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/rtb_base_result/" + size + "/rtb_analyzer1_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return


def run_rtb_analyzer2(i, size, wait):

    timer = threading.Timer(delay, timer_set, [wait, 'dsp_b'])
    timer.start()
    analyzer_run_cmd = "./rtb_analyzer2"
    analyzer_run_cmd = analyzer_run_cmd.split()

    output, err = subprocess.Popen(analyzer_run_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()


    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "bind error" in err or "bind error" in output:
        timer.cancel()
        return run_rtb_analyzer2(i, size, wait)
    if "Signal 11" in err or "Signal 11" in output:
        timer.cancel()
        return run_rtb_analyzer2(i, size, wait)


    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/rtb_base_result/" + size + "/rtb_analyzer2_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return


def run_rtb_dsp1(i, size, wait):

    while True:
        if not wait['dsp_a'] and not wait['dsp_b']: break
    
    timer = threading.Timer(delay, timer_set, [wait, 'ad_exchange_a'])
    timer.start()
    dsp_run_cmd = "./rtb_dsp1"
    dsp_run_cmd = dsp_run_cmd.split()

    output, err = subprocess.Popen(dsp_run_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()


    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "bind error" in err or "bind error" in output:
        timer.cancel()
        return run_rtb_dsp1(i, size, wait)
    if "Signal 11" in err or "Signal 11" in output:
        timer.cancel()
        return run_rtb_dsp1(i, size, wait)


    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/rtb_base_result/" + size + "/rtb_dsp1_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return


def run_rtb_dsp2(i, size, wait):

    while True:
        if not wait['dsp_a'] and not wait['dsp_b']: break

    timer = threading.Timer(delay, timer_set, [wait, 'ad_exchange_b'])
    timer.start()
    dsp_run_cmd = "./rtb_dsp2"
    dsp_run_cmd = dsp_run_cmd.split()

    output, err = subprocess.Popen(dsp_run_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()


    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "bind error" in err or "bind error" in output:
        timer.cancel()
        return run_rtb_dsp2(i, size, wait)
    if "Signal 11" in err or "Signal 11" in output:
        timer.cancel()
        return run_rtb_dsp2(i, size, wait)


    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/rtb_base_result/" + size + "/rtb_dsp2_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return


def run_rtb_ad_exchange(i, size, wait):

    while True:
        if not wait['ad_exchange_a'] and not wait['ad_exchange_b']: break
    
    timer = threading.Timer(delay, timer_set, [wait, 'ssp'])
    timer.start()
    ad_exchange_run_cmd = "./rtb_ad_exchange"
    ad_exchange_run_cmd = ad_exchange_run_cmd.split()

    output, err = subprocess.Popen(ad_exchange_run_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()


    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "bind error" in err or "bind error" in output:
        timer.cancel()
        return run_rtb_ad_exchange(i, size, wait)
    if "Signal 11" in err or "Signal 11" in output:
        timer.cancel()
        return run_rtb_ad_exchange(i, size, wait)


    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/rtb_base_result/" + size + "/rtb_ad_exchange_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return


def run_rtb_ssp(i, size, wait):

    while True:
        if not wait['ssp']: break
    
    timer = threading.Timer(delay, timer_set, [wait, 'client'])
    timer.start()
    ssp_run_cmd = "./rtb_ssp"
    ssp_run_cmd = ssp_run_cmd.split()

    output, err = subprocess.Popen(ssp_run_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()


    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "bind error" in err or "bind error" in output:
        timer.cancel()
        return run_rtb_ssp(i, size, wait)
    if "Signal 11" in err or "Signal 11" in output:
        timer.cancel()
        return run_rtb_ssp(i, size, wait)


    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/rtb_base_result/" + size + "/rtb_ssp_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return



def run_rtb_client(i, size, wait):
    
   
    while True:
        if not wait['client']: break

    client_cmd = "/home/mkpark/test_program/clients/src_base/rtb_client 127.0.0.1 5000"
    client_cmd = client_cmd.split()

    output, err = subprocess.Popen(client_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

    output = output.decode("utf-8")
    err = err.decode("utf-8")

    if "Connection failed" in output:
        return run_rtb_client(i, size, wait)

    f = open("/home/mkpark/pave/depot_tools/native_client/experiment/rtb_base_result/" + size + "/rtb_client_" + str(i) + ".txt", "w")
    
    f.write(output + "\n\n")
    f.write(err + "\n")
    f.close()

    return


    
def run(i, size, manager):

    wait = manager.dict()
    
    wait['dsp_a'] = True
    wait['dsp_b'] = True
    wait['ad_exchange_a'] = True
    wait['ad_exchange_b'] = True
    wait['ssp'] = True
    wait['client'] = True

    procs = []

    proc = Process(target=run_rtb_analyzer1, args=(i, size, wait,))
    procs.append(proc)
    proc.start()
    proc = Process(target=run_rtb_analyzer2, args=(i, size, wait,))
    procs.append(proc)
    proc.start()

    proc = Process(target=run_rtb_dsp1, args=(i, size, wait,))
    procs.append(proc)
    proc.start()
    proc = Process(target=run_rtb_dsp2, args=(i, size, wait,))
    procs.append(proc)
    proc.start()

    proc = Process(target=run_rtb_ad_exchange, args=(i, size, wait,))
    procs.append(proc)
    proc.start()

    proc = Process(target=run_rtb_ssp, args=(i, size, wait,))
    procs.append(proc)
    proc.start()

    proc = Process(target=run_rtb_client, args=(i, size, wait,))
    procs.append(proc)
    proc.start()

    for proc in procs:
        proc.join()


if __name__ == "__main__":
    size = sys.argv[1]

    manager = Manager()
    for i in range(10):
        run(i, size, manager)


