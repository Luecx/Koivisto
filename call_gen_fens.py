import subprocess
import time
import os
import sys


print(f"Launches `num_proc` koivistos in parallel.")
print(f"Usage: python3 {sys.argv[0]} <num_proc> <args>")
print(f"__proc_idx__ gets translated to the instance number")
print(f"Examples: python3 {sys.argv[0]} koivisto.exe 92 gen positions.__proc_idx__.txt")
print(f"          python3 {sys.argv[0]} koivisto.exe 92 scorefens positions.__proc_idx__.txt evals.__proc_idx__.txt")
print()
print()
print()

N_PROC = int(sys.argv[1])
path = [x for x in sys.argv if "__proc_idx__" in x][-1]
while True:

    procs = []
    for i in range(N_PROC):
        print(f"Created proc {i + 1} out of {N_PROC}")
        procs.append(subprocess.Popen([x.replace("__proc_idx__", str(i)) for x in sys.argv[2:]], stdout = subprocess.DEVNULL))
        time.sleep(0.1)

    os.system(f"cat {path.replace('__proc_idx__','*')} | wc -l > n_pos")
    npos = int(open("n_pos").read().strip())
    t = time.time()
    for i in range(3600):
        while True:
            time.sleep(1)
            if time.time() - t > 20:
                break

        os.system(f"cat {path.replace('__proc_idx__','*')} | wc -l > n_pos")
        new_npos = int(open("n_pos").read().strip())
        pps = (new_npos - npos) / (time.time() - t)
        npos = new_npos
        print("TOTAL POSITIONS: {:.3f}M, {}pps".format(npos / 1000 / 1000, int(pps)))
        t = time.time()