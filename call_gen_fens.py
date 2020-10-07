import subprocess
import psutil
import time
import os

def isOurbenchRunning():
    for proc in psutil.process_iter():
        try:
            e = proc.exe().lower()
            if "openbench" in e and "engines" in e:
                return True
        except:
            pass

    return False

def killall():
    for proc in psutil.process_iter():
        if proc.name() == "Koivisto.exe":
            proc.kill()

N_PROC = 10
while True:
    break_due_to_ourbench_running = False
    killall()

    if isOurbenchRunning():
        print("Ourbench running!")
        time.sleep(60)
        continue

    procs = []
    for i in range(N_PROC):
        if isOurbenchRunning():
            break

        print(f"Created proc {i + 1} out of {N_PROC}")
        procs.append(subprocess.Popen(["Koivisto.exe", "gen", f"positions.{i}.txt"], stdout = subprocess.DEVNULL))
        time.sleep(0.1)

    os.system("cat positions.*.txt | wc -l > n_pos")
    npos = int(open("n_pos").read().strip())
    t = time.time()
    for i in range(20):
        while True:
            if isOurbenchRunning():
                print("Breaking due to ourbench!")
                break_due_to_ourbench_running = True
                break

            time.sleep(1)
            if time.time() - t > 20:
                break

        if break_due_to_ourbench_running:
            break

        os.system("cat positions.*.txt | wc -l > n_pos")
        new_npos = int(open("n_pos").read().strip())
        pps = (new_npos - npos) / (time.time() - t)
        npos = new_npos
        print("TOTAL POSITIONS: {:.3f}M, {}pps".format(npos / 1000 / 1000, int(pps)))
        t = time.time()