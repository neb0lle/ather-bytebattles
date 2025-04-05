from __future__ import print_function
import os
import sys
import shutil
import platform
import argparse

# global variables

# environment variable names

g_env_asdk_cmake_root = "ASDK_CMAKE_ROOT"
g_env_asdk_ninja_root = "ASDK_NINJA_ROOT"

# initialized variables

g_os_type = platform.system().lower()
g_os_env = os.environ
g_current_src_path = os.path.dirname(os.path.abspath(sys.argv[0]))
g_asdk_install_script = os.path.join(
    g_current_src_path, "asdk-gen2", "setup", "setup.py")
g_arm_toolchain_dir = os.path.join(
    g_current_src_path, "asdk-gen2", "cmake", "devel_arm_eabi_toolchain.cmake")
g_build_choices = ["release", "debug"]
g_rtos = "uCOS3"

# default variables, gets overriden later

g_cmake = str()
g_ninja = str()
g_users_build_dir = os.path.join(g_current_src_path, "build")

g_parsed_args = object()
g_arg_parser = object()
g_mcu_choices = ["cyt2b75_m0plus", "cyt2b75_m4", "c2000"]


def _parse_args():
    global g_parsed_args
    global g_arg_parser
    global g_users_build_dir

    # add cli arguments
    g_arg_parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    # command: build.py --build <build_dir> --platform <mcu> --clean

    g_arg_parser.add_argument("--build",
                              help="Output build artificats under 'build' directory from the given location",
                              metavar="<build_dir>",
                              required=False,
                              default="Current working directory")

    g_arg_parser.add_argument("--platform",
                              help="Target microcontroller. Allowed values: [" +
                              ", ".join(g_mcu_choices) + "]",
                              metavar="<mcu>",
                              choices=g_mcu_choices,
                              required=False)

    g_arg_parser.add_argument("--clean",
                              help="Do a clean build.",
                              action="store_true",
                              required=False,
                              default=False)
    
    g_arg_parser.add_argument("--type",
                            help="Build type. Allowed values: [" +
                            ", ".join(g_build_choices) + "]",
                            metavar="<build_type>",
                            choices=g_build_choices,
                            required=True)

    if (len((sys.argv)) == 1):
        g_arg_parser.print_help()
        sys.exit(1)
    else:
        g_parsed_args = g_arg_parser.parse_args()

    if ("Current working directory" not in g_parsed_args.build):
        g_users_build_dir = os.path.join(g_parsed_args.build, g_parsed_args.type.lower())
    else:
        g_users_build_dir = os.path.join(g_users_build_dir, g_parsed_args.type.lower())


def _set_asdk_env():
    global g_cmake
    global g_ninja

    cmake_exists = False
    ninja_exists = False

    # check if cmake is available
    if (g_env_asdk_cmake_root in g_os_env):
        g_cmake = os.path.join(g_os_env[g_env_asdk_cmake_root], "bin", "cmake")
        if (g_os_type == "windows"):
            g_cmake = g_cmake + ".exe"
        if not os.path.exists(g_cmake):
            print("Error: ASDK-CMake is not available.")
            print("\nRun 'python {0}' to install it.".format(
                g_asdk_install_script))
            sys.exit(1)
        else:
            cmake_exists = True

    # check if ninja is available
    if (g_env_asdk_ninja_root in g_os_env):
        g_ninja = os.path.join(g_os_env[g_env_asdk_ninja_root], "ninja")
        if (g_os_type == "windows"):
            g_ninja = g_ninja + ".exe"
        if not os.path.exists(g_ninja):
            print("Error: ASDK-ninja is not available.")
            print("\nRun 'python {0}' to install it.".format(
                g_asdk_install_script))
            sys.exit(1)
        else:
            ninja_exists = True

    # ASDK build requirements are not met
    if ((ninja_exists and cmake_exists) == False):
        print("ASDK environment is not avaiable on your system.")
        print("\nRun 'python {0}' to set it up.".format(g_asdk_install_script))
        sys.exit(1)


def _clean():
    clean_cmd = "{0} --build {1} --target clean".format(
        g_cmake, g_users_build_dir)
    os.system(clean_cmd)


def _build():
    build_cmd = "{0} -S . -B {1} -DCMAKE_TOOLCHAIN_FILE={2} -G Ninja -DCMAKE_MAKE_PROGRAM={3} -DTARGET_PLATFORM={4} -DTARGET_RTOS={5} -DCMAKE_BUILD_TYPE={6}".format(
        g_cmake, g_users_build_dir, g_arm_toolchain_dir, g_ninja, g_parsed_args.platform.upper(), g_rtos.upper(), g_parsed_args.type.upper())
    # print(build_cmd)
    if (os.system(build_cmd) == 0):
        build_cmd = "{0} --build {1}".format(g_cmake, g_users_build_dir)
        if (os.system(build_cmd) == 0):
            print("build completed succesfully!")
        else:
            print("build failed.")
    else:
        print("build failed.")


def main():
    _parse_args()

    _set_asdk_env()

    if g_parsed_args.clean and os.path.exists(g_users_build_dir):
        _clean()

    if g_parsed_args.platform:
        _build()


# entry point

if __name__ == "__main__":
    main()
