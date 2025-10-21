import argparse
import os
import subprocess
import re

from dataclasses import dataclass
from typing import List
from general import Terminal


CRASH_LOG_FILENAME = "crash_log.log"
CRASH_LOG_SPERATOR = "======================="
EXECUTABLE_PATH = "MichaelHandMadeGame"


@dataclass
class CrashLog:
    timestamp: int
    atos_line: str

    def run_atos(self, executable_path: str):
        parts = self.atos_line.split()
        for i in range(len(parts)):
            if parts[i] == "<your_binary_path>":
                parts[i] = executable_path
                break
        pipe = subprocess.run(parts, capture_output=True, text=True)
        pretty_print_crash_trace(pipe.stdout)


def get_pretty_function_name(function_part: str) -> str:
    return Terminal.YELLOW + function_part + Terminal.END

def get_pretty_file_name(file_part: str) -> str:
    column_find = file_part.rfind(":")
    if column_find != -1:
        line_number = file_part[column_find + 1:-1]
        file_name = file_part[1:column_find]
        return Terminal.LIGHT_BLUE + file_name + Terminal.END + Terminal.GREY + ":" + line_number + Terminal.END

    return Terminal.LIGHT_BLUE + file_part + Terminal.END

def pretty_print_crash_trace(trace: str):
    for line in trace.splitlines():
        match = re.search(r'0x[0-9a-fA-F]+', line)
        if match:
            print(f"{Terminal.GREY}{line}{Terminal.END}")
            continue

        first_space_index = line.find(" ")
        function_part = line[:first_space_index]

        second_space_index = line.find(") (", first_space_index + 1)
        file_part = line[second_space_index + 2:]

        print(f"{get_pretty_function_name(function_part)} {get_pretty_file_name(file_part)}")


def parse_log_content(content: str) -> CrashLog:
    timestamp_index = content.find("Timestamp(")
    atos_index = content.find("atos -o ")

    if timestamp_index == -1 or atos_index == -1:
        return None

    timestamp = int(content[timestamp_index + len("Timestamp("):content.find(")")])
    atos_line = content[atos_index:].strip()

    return CrashLog(timestamp=timestamp, atos_line=atos_line)

def parse_logs(log_file_path) -> List[CrashLog]:
    crash_logs = []

    with open (log_file_path, "r") as log_file:
        content = log_file.read().split(CRASH_LOG_SPERATOR)

    for log_entry in content:
        log = parse_log_content(log_entry)
        if log:
            crash_logs.append(log)

    return crash_logs


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Check error logs")
    parser.add_argument("execute_folder", help="Path to the execute folder")
    args = parser.parse_args()

    log_file_path = os.path.join(args.execute_folder, CRASH_LOG_FILENAME)
    if not os.path.isfile(log_file_path):
        exit(0)
    
    crash_logs = parse_logs(log_file_path)
    if len(crash_logs) > 0:
        crash_logs[-1].run_atos(os.path.join(args.execute_folder, EXECUTABLE_PATH))
