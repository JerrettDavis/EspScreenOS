#!/usr/bin/env python3
from __future__ import annotations
import argparse,hashlib,json,os,shutil,subprocess,sys,zipfile
from pathlib import Path
import boardlib
ROOT=Path(__file__).resolve().parents[1]
APPS=("dashboard","hardware_test","board_info","calculator","gpio_toolbox","recorder","paint","notes")

def die(message,code=2): print(f"error: {message}",file=sys.stderr);raise SystemExit(code)
def profile(board):
 try:return boardlib.load_board(board)
 except (FileNotFoundError,json.JSONDecodeError) as e:die(str(e))
def generate(board,check=False):
 ok,findings=boardlib.generate(board,check)
 for f in findings:print(f"{f.severity}: {f.message}")
 return ok
def idf():
 tool=shutil.which("idf.py")
 if not tool:die("idf.py is not on PATH; activate ESP-IDF 5.4.x first")
 return tool
def run(cmd): print("+",subprocess.list2cmdline([str(x) for x in cmd]));return subprocess.run(cmd,cwd=ROOT).returncode
def build_args(board,apps):
 generate(board);base=ROOT/"generated"/board;defaults=[base/"sdkconfig.defaults"]
 if apps is not None:
  selected={x.strip().replace("-","_") for x in apps.split(",") if x.strip()}
  unknown=selected-set(APPS)
  if unknown:die("unknown app(s): "+", ".join(sorted(unknown)))
  overlay=base/"apps.defaults";overlay.write_text("".join(f"CONFIG_ESPSCREEN_APP_{x.upper()}={'y' if x in selected else 'n'}\n" for x in APPS));defaults.append(overlay)
 sep=";" if os.name=="nt" else ";"
 return ["-D",f"ESPSCREEN_BOARD={board}","-D",f"SDKCONFIG={ROOT/'build'/board/'sdkconfig'}","-D",f"SDKCONFIG_DEFAULTS={sep.join(str(x) for x in defaults)}"]
def main():
 p=argparse.ArgumentParser(prog="espscreen",description="EspScreenOS board/build/flash tool")
 sub=p.add_subparsers(dest="command",required=True)
 sub.add_parser("list-boards")
 v=sub.add_parser("validate");v.add_argument("--all",action="store_true");v.add_argument("--board")
 g=sub.add_parser("generate");g.add_argument("--all",action="store_true");g.add_argument("--board");g.add_argument("--check",action="store_true")
 sub.add_parser("doctor");sub.add_parser("test")
 for name in ("build","flash","monitor"):
  q=sub.add_parser(name);q.add_argument("--board",required=True);q.add_argument("--port");q.add_argument("--apps",help="comma-separated built-in app IDs");q.add_argument("--allow-provisional",action="store_true")
 pkg=sub.add_parser("package");pkg.add_argument("--board",required=True)
 a=p.parse_args()
 if a.command=="list-boards":
  for bid in boardlib.board_ids(): print(f"{bid:34} {profile(bid)['display_name']}")
 elif a.command=="validate":
  ids=boardlib.board_ids() if a.all else [a.board or die("use --all or --board")];bad=False
  for bid in ids:
   findings=boardlib.validate_board(profile(bid));print(f"{bid}: {'ok' if not findings else 'issues'}")
   for f in findings:print(f"  {f.severity}: {f.message}");bad|=f.severity=="error"
  raise SystemExit(1 if bad else 0)
 elif a.command=="generate":
  ids=boardlib.board_ids() if a.all else [a.board or die("use --all or --board")];raise SystemExit(0 if all(generate(x,a.check) for x in ids) else 1)
 elif a.command=="doctor":
  print("Python:",sys.version.split()[0]);print("ESP-IDF:",os.environ.get("IDF_PATH","not activated"));print("idf.py:",shutil.which("idf.py") or "not found");print("CMake:",shutil.which("cmake") or "not found");raise SystemExit(0 if shutil.which("idf.py") else 1)
 elif a.command=="test":
  if run([sys.executable,"-m","unittest","discover","tests/python","-v"]):raise SystemExit(1)
  if shutil.which("cmake"):
   if run(["cmake","-S","tests/host","-B","build/host"]) or run(["cmake","--build","build/host","--config","Debug"]) or run(["ctest","--test-dir","build/host","-C","Debug","--output-on-failure"]):raise SystemExit(1)
  else: print("warning: CMake unavailable; host C++ tests skipped")
 elif a.command in ("build","flash","monitor"):
  b=profile(a.board)
  if b["maturity"]=="provisional" and a.command=="flash" and not a.allow_provisional:die("refusing provisional profile; pass --allow-provisional after checking the PCB")
  build=ROOT/"build"/a.board;args=build_args(a.board,a.apps);target=b["mcu"]["target"]
  if a.command=="build":
   if not (build/"sdkconfig").exists() and run([idf(),"-B",build,*args,"set-target",target]):raise SystemExit(1)
   raise SystemExit(run([idf(),"-B",build,*args,"build"]))
  if not a.port:die(f"--port is required for {a.command}")
  raise SystemExit(run([idf(),"-B",build,*args,"-p",a.port,a.command]))
 elif a.command=="package":
  bdir=ROOT/"build"/a.board
  flasher=bdir/"flasher_args.json"
  if not flasher.exists():die("build firmware before packaging")
  out=ROOT/"dist"/f"espscreenos-{a.board}.zip";out.parent.mkdir(exist_ok=True)
  flash_files=json.loads(flasher.read_text()).get("flash_files",{}).values()
  files=sorted([bdir/path for path in flash_files]+[flasher,ROOT/"generated"/a.board/"board.json",ROOT/"VERSION",ROOT/"RELEASE_NOTES.md",ROOT/"LICENSE",ROOT/"docs"/"build-and-flash.md"],key=lambda p:p.as_posix())
  missing=[str(p.relative_to(ROOT)) for p in files if not p.is_file()]
  if missing:die("package input missing: "+", ".join(missing))
  checksums="".join(f"{hashlib.sha256(p.read_bytes()).hexdigest()}  {p.relative_to(ROOT).as_posix()}\n" for p in files)
  timestamp=(1980,1,1,0,0,0)
  def add_bytes(archive,name,data):
   info=zipfile.ZipInfo(name,timestamp);info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o100644<<16
   archive.writestr(info,data)
  with zipfile.ZipFile(out,"w") as z:
   for f in files:add_bytes(z,f.relative_to(ROOT).as_posix(),f.read_bytes())
   add_bytes(z,"SHA256SUMS",checksums.encode())
  print(out)
if __name__=="__main__":main()
