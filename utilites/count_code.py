#encoding=UTF-8

import os
import argparse


class LineCounter:
    def __init__(self, path):
        self.lines_count = 0
        self.files_count = 0
        self.files_lines = {}

        self.ignore_folders = [
            "build",
            "glfw-3.4",
            "glfw-build",
            "include",
            "lib",
            "MacAppPack",
            "output",
        ]

        self.ignore_files = [
        ]

        self.include_file_types = (
            ".java",
            ".c",
            ".cpp",
            ".h",
            ".hpp",
        )

        self.path = path

        if not os.path.isdir(path):
            raise FileNotFoundError(path)

    def is_dirname_in_ignore(self, dir_name):
        for ignore_folder in self.ignore_folders:
            if dir_name.find(ignore_folder) >= 0:
                return True
        return False

    def is_filename_in_ignore(self, file_name):
        for ignore_folder in self.ignore_folders:
            if file_name.find(ignore_folder) >= 0:
                return True
        for ignore_file in self.ignore_files:
            if file_name.find(ignore_file) >= 0:
                return True
        return False

    def scan_folder(self):
        for dirr_name, dirns, file_names in os.walk(self.path):
            if self.is_dirname_in_ignore(dirr_name):
                continue

            for file_name in file_names:
                if self.is_filename_in_ignore(file_name):
                    continue

                if file_name.endswith(self.include_file_types):
                    self.files_count += 1

                    file_path = os.path.join(dirr_name, file_name)
                    self.files_lines[file_path] = self.scan_file(file_path)

                    self.lines_count += self.files_lines[file_path]

    def scan_file(self, file_path):
        try:
            with open(file_path) as file:
                line_count = len(file.readlines())
            return line_count
        except Exception as e:
            return 0
        # file_lines = file_lines.replace(" ", "")
        # file_lines = file_lines.replace("\t", "")
        # file_lines = file_lines.split("\n")

        # line_count = 0

        # for line in file_lines:
        #     if line.startswith("#") or line.startswith("//") or
        # line.startswith("/*") or line.startswith("<!--"):
        #         continue

        #     if line != "":
        # line_count += 1

    def print_result(self, print_indivisual_files_size=True, print_folder_collective_size=True):
        if print_indivisual_files_size:
            order = list(self.files_lines)
            order.sort(reverse=True)
            bumber_len = len(str(order[0]))

            for linesc in order:
                diffrent = 10 + (bumber_len - len(str(linesc)))
                diffrent = diffrent * " "
                print(f"{linesc}{diffrent}| {self.files_lines[linesc]}")
            
            print("-" * 20, "\n")

        
        if print_folder_collective_size:
            dir_lines = {}
            for file_path, line_count in self.files_lines.items():
                dirname = os.path.dirname(file_path)
                if dirname not in dir_lines:
                    dir_lines[dirname] = 0
                dir_lines[dirname] += line_count
        
            for dirname, line_count in dir_lines.items():
                print(f"{dirname}          {line_count}")

            print("-" * 20, "\n")

        print("File Count", self.files_count)
        print("Line Count", self.lines_count)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("folder")

    args = parser.parse_args()

    counter = LineCounter(args.folder)
    counter.scan_folder()
    counter.print_result()
