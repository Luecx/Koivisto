import multiprocessing
import os
import chess

N_THREADS = 16
BUF_SIZE = 32 * 1024 * 1024
FNAME = "2016-07.pgn"

def work(thread_n):
    progress_fname = "workcache/{}_progress.txt".format(thread_n)
    
    f = open(FNAME, "rb")
    f.seek(0, 2)
    total_bytes = f.tell()
    f.seek(0)

    os.makedirs("output", exist_ok = True)
    output_f = open("output/{}.txt".format(thread_n), "a")

    while True:
        try:
            os.makedirs("workcache", exist_ok = True)
            cur = int(open(progress_fname).read().strip())
        except:
            cur = total_bytes // N_THREADS * thread_n
            open(progress_fname, "w").write(str(cur))

        if cur + BUF_SIZE >= total_bytes // N_THREADS * (thread_n + 1):
            return

        f.seek(cur)
        buf = f.read(BUF_SIZE).decode("utf8")

        events = buf.split("[Event")[1:-1]
        events = [e for e in events if "[%eval" in e]

        for event in events:
            for line in event.split("\n"):
                if line[:2] != "1.":
                    continue

                moves_tokens = line.split("}")
                moves = []
                for tok in moves_tokens:
                    if "eval" not in tok:
                        break

                    move = tok.split("{")[0].split(".")[-1].strip().replace("!", "").replace("?", "")
                    e = tok.split("[%eval ")[1].split("]")[0]
                    moves.append((move, e))

                b = chess.Board()
                fen_data = []
                for m in moves:
                    b.push_san(m[0])
                    fen_data.append((b.fen(), m[1]))

                for d in fen_data:
                    output_f.write(d[0] + ";" + d[1] + "\n")

                break


        cur += BUF_SIZE
        open(progress_fname, "w").write(str(cur))

if __name__ == "__main__":
    p = multiprocessing.Pool(N_THREADS)
    p.map(work, range(N_THREADS))
