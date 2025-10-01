Import("env")
import os, subprocess, pathlib
APP_NAME    = os.environ.get("APP_NAME",    "HYBRIDSPOT")
APP_VERSION = os.environ.get("APP_VERSION", "0.6.3-ci")
GIT_SHA     = os.environ.get("GITHUB_SHA",  "")
GIT_REF     = os.environ.get("GITHUB_REF_NAME", "")
PIOENV_NAME = os.environ.get("PIOENV", env.get("PIOENV") or "")

def git(cmd):
    try: return subprocess.check_output(cmd, stderr=subprocess.DEVNULL).decode().strip()
    except Exception: return ""
if not GIT_SHA: GIT_SHA = git(["git","rev-parse","--short","HEAD"]) or "local"
if not GIT_REF: GIT_REF = git(["git","rev-parse","--abbrev-ref","HEAD"]) or "local"
content = f