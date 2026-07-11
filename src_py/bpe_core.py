from typing import Self
import sys
from random import randint, random
from math import log

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

_CURRENT_UID : int = 0

_SPLIT_TOK = ','
_OPEN_PAREN = '('
_CLOSE_PAREN = ')'
_OPEN_BRACK = '['
_CLOSE_BRACK = ']'

class Token:
    left : str | Self
    right : str | Self | None
    uid : int 
    freq : int
    
    def __init__(self, uid : int, left: str | Self, right : str | Self | None):
        self.left = left
        self.right = right
        self.uid = uid
        self.freq = 1

    def __hash__(self):
        return hash((self.left, self.right))

    def __str__(self):
        res = str(self.left)
        if self.right is not None:
            res += str(self.right)
        return res

    def __eq__(self, other):
        return self.__str__() == other.__str__()

    def __repr__(self):
        return self.__str__()

    def dump(self, level: int = 0) -> str:
        global _SPLIT_TOK, _OPEN_PAREN, _CLOSE_PAREN, _OPEN_BRACK, _CLOSE_BRACK, _CURRENT_UID
        result = ""

        result += f"{_OPEN_BRACK}{self.uid}{_CLOSE_BRACK}"

        if self.right is not None:
            result += _OPEN_PAREN

        if isinstance(self.left, str):
            result += self.left
        else:
            result += self.left.dump(level + 1)

        if self.right is not None:
            result += _SPLIT_TOK

            if isinstance(self.right, str):
                result += self.right
            else:
                result += self.right.dump(level + 1)

            result += _CLOSE_PAREN

        return result
    

def parse_token(line: str) -> Token | None:
    global _SPLIT_TOK, _OPEN_PAREN, _CLOSE_PAREN, _OPEN_BRACK, _CLOSE_BRACK, _CURRENT_UID

    if not line:
        eprint("ERROR: cannot return token from null str")
        return None

    if _CLOSE_BRACK not in line:
        eprint("ERROR: given token has no uid :", line)
        return None

    lhs, rhs = line[1:].split(_CLOSE_BRACK, maxsplit=1)

    if not lhs.isdigit():
        eprint("ERROR: given token uid not being an integer :", line)
        return None

    if not rhs:
        eprint("ERROR: empty token data:", line)
        return None
    
    if rhs[0] == _OPEN_PAREN:

        first, second = "", rhs[1:-1]
        depth = 0
        while not (depth == 0 and second[0] == _SPLIT_TOK):
            if second[0] == _OPEN_PAREN:
                depth += 1
            elif second[0] == _CLOSE_PAREN:
                depth -= 1

            first += second[0]
            second = second[1:]

        second = second[1:]
        left = parse_token(first)
        right = parse_token(second)

        if left is None or right is None:
            eprint("ERROR: could not build children from", line)
            return None

        tok = Token(int(lhs), left, right)
        return tok
    else:
        tok = Token(int(lhs), rhs, None)
        return tok

class Bpe:
    toks : list[Token]

    def __init__(self, file_path : str):
        self.toks = []
        if file_path:
            file = open(file_path, "r")
            for ch in file.read():
                if ch == '\n':
                    continue
                global _CURRENT_UID
                self.toks.append(Token(_CURRENT_UID, ch.lower(), None))
                _CURRENT_UID += 1
            file.close()

    def __str__(self) -> str:
        return ''.join([str(tok) for tok in self.toks])

    def __repr__(self) -> str:
        return self.__str__()

    def save_to_file(self, file_path: str):
        eprint("INFO: saving to file", file_path, len(self.toks), "tokens")
        global _SPLIT_TOK, _OPEN_PAREN, _CLOSE_PAREN, _OPEN_BRACK, _CLOSE_BRACK, _CURRENT_UID
        with open(file_path, 'w') as f:
            f.write(f"{_SPLIT_TOK}{_OPEN_PAREN}{_CLOSE_PAREN}{_OPEN_BRACK}{_CLOSE_BRACK}\n")
            for tok in self.toks:
                f.write(tok.dump())
                f.write('\n')

    def load_from_file(self, file_path: str):
        global _SPLIT_TOK, _OPEN_PAREN, _CLOSE_PAREN, _OPEN_BRACK, _CLOSE_BRACK, _CURRENT_UID
        self.toks = []
        with open(file_path, 'r') as f:
            last_line = ""
            first = True
            for line in f:
                if first:
                    first = False
                    _SPLIT_TOK = line[0]
                    _OPEN_PAREN = line[1]
                    _CLOSE_PAREN = line[2]
                    _OPEN_BRACK = line[3]
                    _CLOSE_BRACK = line[4]
                else:
                    tok = parse_token(line[:-1])
                    self.toks.append(tok)
        eprint("INFO: loaded from file", file_path, len(self.toks), "tokens")

    def fix_freq(self):
        mp : dict[Token, int] = {}

        for tok in self.toks:
            mp[tok] = mp.get(tok, 0) + 1

        for tok in self.toks:
            tok.freq = mp[tok]

    def step(self) -> bool:
        if len(self.toks) <= 1:
            return False

        mp : dict[tuple[Token, Token], int] = {}

        for i in range(len(self.toks) - 1):
            pr = (self.toks[i], self.toks[i+1])
            mp[pr] = mp.get(pr, 0) + 1
       
        first_k = (self.toks[0], self.toks[1])
        best_kv = (first_k, mp[first_k])
        for k, v in mp.items():
            if v > best_kv[1]:
                best_kv = (k, v)
        
        if best_kv[1] == 1:
            return False
        best_pr = best_kv[0]

        j = 0
        new_toks : list[Token] = []
        while j < len(self.toks):
            if j + 1 >= len(self.toks):
                new_toks.append(self.toks[j])
                j += 1
            else:
                cur = (self.toks[j], self.toks[j+1])
                if cur == best_pr:
                    global _CURRENT_UID
                    new_toks.append(Token(_CURRENT_UID, best_pr[0], best_pr[1]))
                    _CURRENT_UID += 1
                    j += 2
                else:
                    new_toks.append(self.toks[j])
                    j += 1
        self.toks = new_toks
        return True

    def follows(self, word: str) -> str:
        self.fix_freq()
        word = word.lower()

        candidates: dict[str, int] = {}
        for tok in self.toks:
            s = str(tok)
            s = s.strip()
            if word in s and word != s:
                candidates[s] = 0.2 * log(len(s)) + (1.0 - 0.2) * tok.freq + random()

        if not candidates:
            return "mb bpe.py failed"

        best = max(candidates.items(), key=lambda kv: (kv[1], len(kv[0])))
        return best[0]

    def fetch(self) -> str:
        n = len(self.toks)
        return str(self.toks[randint(0, n - 1)])

def usage():
    print("USAGE:")
    print("\t: save data_file outputfile")
    print("\t: load save_file word")

def __main(args: list[str]) -> int:
    if len(args) < 3:
        usage()
        return 1
    
    if args[1] != "save" and args[1] != "load" and args != "next":
        usage()
        return 1

    if args[1] == "save":
        if len(args) < 4:
            usage()
            return 1

        bpe = Bpe(args[2])

        i = 1
        while bpe.step():
            if i % 100 == 0:
                print(f"Iterated {i} times")
            i += 1

        bpe.save_to_file(args[3])

    elif args[1] == "load":
        bpe = Bpe("")
        bpe.load_from_file(args[2])
        while True:
            print('> ', end='')
            word = input()
            if word == "#quit":
                break
            print(bpe.follows(word))

    return 0

if __name__ == "__main__":
    ret = __main(sys.argv)
