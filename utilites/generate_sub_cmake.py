import os
import argparse


def generate_all_directory_cmake_files(root_directory: str):
    for dirpath, _, _ in os.walk(root_directory):
        generate_single_directory_cmake_file(dirpath)

    for dirpath, dirnames, _ in os.walk(root_directory, topdown=False):
        print(dirpath)
        directories_that_have_cmake = [d for d in dirnames if os.path.exists(os.path.join(dirpath, d, "CMakeLists.txt"))]
        if len(directories_that_have_cmake) == 0:
            continue

        cmake_include_sub_folder = ""
        for directory in directories_that_have_cmake:
            cmake_include_sub_folder += "include(${CMAKE_CURRENT_LIST_DIR}/" + directory + "/CMakeLists.txt)\n"
        
        if os.path.exists(os.path.join(dirpath, "CMakeLists.txt")):
            with open(os.path.join(dirpath, "CMakeLists.txt"), "a") as cmake_file:
                cmake_file.write("\n# Include sub-directory CMakeLists\n")
                cmake_file.write(cmake_include_sub_folder)

        else:
            with open(os.path.join(dirpath, "CMakeLists.txt"), "w") as cmake_file:
                cmake_file.write("# Include sub-directory CMakeLists\n")
                cmake_file.write(cmake_include_sub_folder)

def generate_single_directory_cmake_file(directory: str):
    files = [file for file in os.listdir(directory) if os.path.isfile(os.path.join(directory, file))]

    cpp_files = []

    for file in files:
        if file.endswith(".cpp") or file.endswith(".c"):
            cpp_files.append(file)
        elif file.endswith(".hpp") or file.endswith(".h"):
            cpp_files.append(file)
    
    if len(cpp_files) == 0:
        return False

    cpp_files.sort()
    
    cmake_content = "list(APPEND SOURCES\n"
    for cpp_file in cpp_files:
        cmake_content += "    ${CMAKE_CURRENT_LIST_DIR}/" + f"{cpp_file}\n"
    cmake_content += ")\n"

    with open(os.path.join(directory, "CMakeLists.txt"), "w") as cmake_file:
        cmake_file.write(cmake_content)

    return True


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate sub CMakeLists.txt files.")
    parser.add_argument("path", type=str, help="Path to the directory to generate CMakeLists.txt for.")
    args = parser.parse_args()

    generate_all_directory_cmake_files(args.path)