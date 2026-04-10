import os
import platform
import subprocess
import sys
import shutil

def main():
    system = platform.system()
    print(f"Detected OS: {system}")

    script_dir = os.path.dirname(os.path.abspath(__file__))
    premake_exe = "premake5.exe" if system == "Windows" else "premake5"
    premake_path = os.path.normpath(os.path.join(script_dir, "../vendor/Premake", premake_exe))

    if not os.path.isfile(premake_path):
        print(f"Error: premake not found at {premake_path}")
        sys.exit(1)

    if system == "Windows":
        action = "vs2026"
    elif system == "Linux":
        if shutil.which("ninja"):
            action = "ninja"
        else:
            action = "gmake2"
    else:
        print(f"Unsupported OS: {system}")
        sys.exit(1)

    print(f"Running premake with action: {action}...")

    try:
        subprocess.run([premake_path, action], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Premake failed with error: {e}")
        sys.exit(e.returncode)


if __name__ == "__main__":
    main()