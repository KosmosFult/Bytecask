import os
import sys
import random
import string

def generate_random_string(length):
    letters = string.ascii_letters + string.digits
    return ''.join(random.choice(letters) for _ in range(length))


if __name__ == "__main__":
    dir = os.path.dirname(os.path.abspath(sys.argv[0]))
    trace_name = os.path.join(dir, "trace", "trace_compact.txt")
    ans_name = os.path.join(dir, "trace", "trace_compact_ans.txt")
    kv_name = os.path.join(dir, "trace", "trace_compact_kvs.txt")
    trace_file = open(trace_name, mode="w+")
    ans_file = open(ans_name, mode="w+")
    kv_file = open(kv_name, mode="w+")
    operator = ["set", "get", "del"]
    weights = [0.475, 0.475, 0.05]

    kvmap = dict()
    for i in range(5000):
        op = random.choices(operator, weights)[0]
        if op == "set":
            if random.random() < 0.5 and len(kvmap) > 0:
                key = random.choice(list(kvmap.keys()))
            else:
                key = generate_random_string(random.randint(5,32))
            val = generate_random_string(random.randint(8, 128))
            kvmap[key] = val
            trace_file.write(f"{i},{op},{key},{val}\n")
        elif op == "get":
            if len(kvmap) == 0:
                continue
            key = random.choice(list(kvmap.keys()))
            val = kvmap.get(key, False)
            if val:
                trace_file.write(f"{i},{op},{key}\n")
                ans_file.write(f"{i},{val}\n")
        else:
            if len(kvmap) == 0:
                continue
            key = random.choice(list(kvmap.keys()))
            trace_file.write(f"{i},{op},{key}\n")
            del kvmap[key]
    
    for k,v in kvmap.items():
        kv_file.write(f"{k},{v}\n")

    kv_file.close()
    trace_file.close()
    ans_file.close()