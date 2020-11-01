import subprocess
import psutil
import time
import os
import sys

# Aggressively kill stray processes we're not correctly cleaning up.
def killall():
    for proc in psutil.process_iter():
        if "koivisto" in proc.name().lower():
            proc.kill()

print(f"Launches `num_proc` koivistos in parallel.")
print(f"Usage: python3 {sys.argv[0]} <num_proc> <args>")
print(f"__proc_idx__ gets translated to the instance number")
print(f"Examples: python3 {sys.argv[0]} koivisto.exe 92 gen positions.__proc_idx__.txt")
print(f"          python3 {sys.argv[0]} koivisto.exe 92 scorefens positions.__proc_idx__.txt evals.__proc_idx__.txt")
print()
print()
print()

N_PROC = int(sys.argv[1])
while True:
    killall()

    procs = []
    for i in range(N_PROC):
        print(f"Created proc {i + 1} out of {N_PROC}")
        procs.append(subprocess.Popen([x.replace("__proc_idx__", str(i)) for x in sys.argv[2:]], stdout = subprocess.DEVNULL))
        time.sleep(0.1)

    os.system("cat positions.*.txt | wc -l > n_pos")
    os.system("cat evals.*.txt | wc -l > n_evs")
    npos = int(open("n_pos").read().strip())
    nevs = int(open("n_evs").read().strip())
    t = time.time()
    for i in range(3600):
        while True:
            time.sleep(1)
            if time.time() - t > 20:
                break

        os.system("cat positions.*.txt | wc -l > n_pos")
        os.system("cat evals.*.txt | wc -l > n_evs")
        new_npos = int(open("n_pos").read().strip())
        new_nevs = int(open("n_evs").read().strip())
        pps = (new_npos - npos) / (time.time() - t)
        eps = (new_nevs - nevs) / (time.time() - t)
        npos = new_npos
        nevs = new_nevs
        print("TOTAL POSITIONS: {:.3f}M, {}pps".format(npos / 1000 / 1000, int(pps)))
        print("TOTAL EVALS:     {:.3f}M, {}eps".format(nevs / 1000 / 1000, int(eps)))
        t = time.time()