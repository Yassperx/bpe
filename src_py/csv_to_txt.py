import sys
import csv
import re

def csv_to_txt(csv_path : str):
    messages = []
    with open(csv_path, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        first = True
        index = -1
        for row in reader:
            if first:
                index = row.index("content")
                first = False
            else:
                messages.append(row[index])
    return '\n'.join(messages)

def filter(content : str, output_path: str):
    content = content.encode("ascii", errors="ignore").decode() 
    content = re.sub(r"[,()\[\]]", "", content)
    content = content.lower()
    with open(output_path, 'w') as f:
        f.write(content)

def main(args: list[str]):
    if len(args) != 3:
        print("USAGE: %d csv_path txt_path" % args[0])
    csv_path = args[1]
    txt_path = args[2]
    content = csv_to_txt(csv_path)
    filter(content, txt_path)

if __name__ == "__main__":
    main(sys.argv)

