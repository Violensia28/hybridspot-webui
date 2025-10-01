# PATCH CI — hybridspot-webui

Patch ini **menimpa tiga file** agar build di Codespaces/Actions **langsung hijau**:

1) `scripts/gen_buildinfo.py` — versi **robust** (tanpa f-strings/triple-quote) → mencegah `SyntaxError`/`NameError`.
2) `platformio.ini` — **relax** `platform_packages` (dikomentari) → biarkan PlatformIO ambil Arduino-ESP32 yang cocok agar `WiFi.h` tersedia.
3) `.github/workflows/build.yml` — tambah step **Prepare PlatformIO platform & framework**, cache pip/PIO, build **single-thread** dan upload artifacts.

## Cara apply (di root repo)

```bash
unzip -o patch-ci-full.zip -d .

git add scripts/gen_buildinfo.py platformio.ini .github/workflows/build.yml
git commit -m "ci: robust gen_buildinfo + relax platform_packages + prepare platform"
# Disarankan push ke branch baru lalu PR
git switch -c feat/ci-fix
git push -u origin feat/ci-fix
```

Setelah PR terbuka → tab **Actions** → jalankan **Build (manual & tags)**.
Jika ingin rilis: `git tag v0.6.4 && git push origin v0.6.4`.

## Catatan
- Jika kamu memang butuh pin versi framework/toolchain, aktifkan kembali blok `platform_packages` **setelah** Actions stabil, atau install eksplisit via step `pio pkg install`.
- Build lokal/Codespaces cepat: `pio run -e ci_build -j 1` (setelah `pipx install platformio`).
