Import("env")
import os, subprocess, pathlib

def _git(args):
    try:
        return subprocess.check_output(args, stderr=subprocess.DEVNULL).decode().strip()
    except Exception:
        return ""

APP_NAME    = os.environ.get("APP_NAME",    "HYBRIDSPOT")
APP_VERSION = os.environ.get("APP_VERSION", "0.6.3-ci")
GIT_SHA     = os.environ.get("GITHUB_SHA",  "")
GIT_REF     = os.environ.get("GITHUB_REF_NAME", "")
PIOENV_NAME = os.environ.get("PIOENV", env.get("PIOENV") or "")

if not GIT_SHA:
    GIT_SHA = _git(["git", "rev-parse", "--short", "HEAD"]) or "local"
if not GIT_REF:
    GIT_REF = _git(["git", "rev-parse", "--abbrev-ref", "HEAD"]) or "local"

lines = [
    "#pragma once",
    "#ifndef BUILD_INFO_H",
    "#define BUILD_INFO_H",
    '#define APP_NAME        "{0}"'.format(APP_NAME),
    '#define APP_VERSION     "{0}"'.format(APP_VERSION),
    '#define GIT_SHA         "{0}"'.format(GIT_SHA),
    '#define GIT_REF         "{0}"'.format(GIT_REF),
    '#define PIOENV_NAME     "{0}"'.format(PIOENV_NAME),
    "#endif",
]

out = pathlib.Path("include") / "BuildInfo.h"
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text("
".join(lines) + "
", encoding="utf-8")

print("[gen_buildinfo] Wrote {} -> {{APP_NAME: {}, APP_VERSION: {}, GIT_REF: {}, GIT_SHA: {}}}".format(
    out, APP_NAME, APP_VERSION, GIT_REF, GIT_SHA
))
