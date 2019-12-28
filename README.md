# NVMP-X: Native
The native NV:MP master branch, where all native and SDK development is happening.

NVMPX (NVMP10) is an AFX rewrite of the original modification of NVMP, now open source. This project is entirely unstable at the moment, view the status section for each project state.

In brief, if you came here with the intention of downloading and running NV:MP, you're out of luck for now until things are re-implemented. You can however get involved if you got any 
programming knowledge to contribute!

## Compiling and Building

Use CMake on the source directory and point your build directory somewhere of choice, no magic needed at the moment - should build in your chosen IDE.

No 64-bit support right now, Fallout: New Vegas is x86 so the projects are aligned to support it. In future it may be possible to target the server to 64-bit specifically, depending on the requirements of lib_net.

Compilation may be flakey on Linux as it's unsupported at the moment, but on the agenda - some Linux implementaitons are needed, which should flag up if you try to build on non Win32.

## Legacy Repository

The legacy project is still closed source whilst it's unshittified and moved over to the AFX / NVMP-X redo, so the main game client build is missing for the time being.

## Primary Project Statuses
- app_client 
  :: Not compilable, code is outdated, scheduled for rewrite

- app\_client\_headless :: Should be compilable in Debug, this is a headless connection to the server for testing and developing lib_net

- app_server :: Should be compilable in Debug, code is being stripped and rewritten

\* in the progress of migrating from legacy repository

Project is not compilable in Release as we have hand coded errors for important features or security checks that need to be addressed before making a public release.

## Wikipedia & Discord

We have a couple of channels to discuss development on this repro over at [our Discord](https://discordapp.com/invite/vvA4yAu),
and we have a [small wikipedia](https://wiki.nv-mp.com/) being made slowly in the background to support SDK documentation.
