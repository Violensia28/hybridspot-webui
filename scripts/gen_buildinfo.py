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

content = (
    "#pragma once
"
    "#ifndef BUILD_INFO_H
E        "{0}"
'
    '#define APP_VERSION     "{1}"
'
    '#define GIT_SHA  ine APP_VERSION     "{1}"
'
    '#define GIT_SHA         "{2}"
'
    '#define GIT_REF         "{3}"
'
    '#define PIOENV_NAME     "{4}"
'
    "#endif
"
).format(APP_NAME, APP_VERSION, GIT_SHA, GIT_REF, PIOENV_NAME)

out = pathlib.Path("include") / "BuildInfo.h"
out.write_text(content, encoding="utf-8")
print(f"[gen_buildinfo] Wrote {out} -> {{APP_NAME: {APP_NAME}, APP_VERSION: {APP_VERSION}, GIT_REF: {GIT_REF}, GIT_SHA: {GIT_SHA}}}")
