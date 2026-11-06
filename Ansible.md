# Build and install Mako Server with Ansible

The included [Ansible playbook](mako.yaml) downloads the sources and builds Mako Server on a Debian-based Linux system. You can run it on your own computer or build directly on a remote device, such as a Raspberry Pi. It can also install the server and start the Lua tutorials.

Ansible runs the setup steps for you. You don't need to write a playbook to use this one. On Windows, run it in [Windows Subsystem for Linux (WSL)](https://learn.microsoft.com/en-us/windows/wsl/install).

## Before you start

Install Ansible on the computer that will run the playbook, then download `mako.yaml`:

```sh
# Run on your Debian/Ubuntu development computer.
sudo apt update && sudo apt install ansible -y
wget https://raw.githubusercontent.com/RealTimeLogic/BAS/refs/heads/main/mako.yaml
```

The build machine needs Python and internet access to download the sources. The playbook installs the build packages on Debian-based systems. Package installation and copying the finished server to `/usr/local/bin` require sudo access. Add `--ask-become-pass` to the commands below if sudo requires a password.

## Build on your computer

Run these commands from the directory containing `mako.yaml`.

To download and compile without installing:

```sh
# The trailing comma identifies localhost as the inventory, rather than a file.
ansible-playbook -i localhost, -c local mako.yaml
```

To compile, install, and start the tutorials:

```sh
# Install mako and mako.zip in /usr/local/bin, then launch the tutorials.
ansible-playbook -i localhost, -c local mako.yaml -e install=true -e tutorials=true
```

Starting the tutorials stops existing `mako` processes with `killall mako`. Use this option when you're ready to replace a running Mako instance.

## Build on a remote device

Run Ansible on your development computer. The compilation runs on the remote device, which needs Python and enough disk space for the sources and build tools.

Set up SSH key access if you haven't already, then run the playbook:

```sh
# Replace the address and user with those of your device.
ssh-copy-id debian@192.168.1.146
ansible-playbook -i 192.168.1.146, -u debian mako.yaml -e install=true -e tutorials=true
```

## Open the tutorials

After a run with `tutorials=true`, open [http://localhost](http://localhost) for a local installation, or `http://<target-ip>` for a remote device. If port 80 was unavailable, use port 9357, for example [http://localhost:9357](http://localhost:9357).

## Playbook options

Pass options with `-e`, as shown above.

| Variable | Type and default | Effect |
| --- | --- | --- |
| `install` | Boolean, `false` | Copies `mako` and `mako.zip` to `/usr/local/bin`. |
| `tutorials` | Boolean, `false` | Starts the tutorial launcher using the installed server. Use with `install=true` on the first run. |
| `builddir` | Directory path, `makobuild` | Selects the directory for the source checkouts on the build machine. |
| `mako_install_path` | Directory path, `/usr/local/bin` | Selects the installation directory. |

## Updating an existing build

Rerun the playbook to update its Git checkouts. The compile task runs when the **BAS checkout changes**. Changes to BAS-Resources or another dependency alone do not trigger compilation; rebuild those manually from the BAS build directory:

```sh
# Use your configured builddir if it differs from makobuild.
cd makobuild/BAS
make -f mako.mk clean
make -f mako.mk
```

Keep `BAS/examples/MakoServer/src/NewEncryptionKey.h` when updating or moving an existing build. This generated header contains the pre-master secret used by native softTPM key derivation, including ECC certificate keys. Replacing it changes the secret available to the application. See the [softTPM documentation](https://realtimelogic.com/ba/examples/MakoServer/readme.html#TPM).

The playbook manages its own source checkouts. Its BAS-Resources update uses `force: yes`, so keep edits you want to retain outside that managed checkout or save them before running it.

## Cross-compiling

For a local cross-build, set `CROSS_COMPILE` to the toolchain prefix before running the playbook. The makefile appends `gcc`:

```sh
# Example prefix for an installed ARM Linux toolchain.
export CROSS_COMPILE=/opt/toolchain/bin/arm-linux-gnueabihf-
ansible-playbook -i localhost, -c local mako.yaml
```

Copy the resulting executable and `mako.zip` to the target. Leave `install` and `tutorials` disabled on the build computer when it cannot run the target executable. The remote-device procedure above compiles on the device itself.

## What gets downloaded

The playbook fetches BAS, BAS-Resources, LPeg, lua-protobuf, and CBOR. It also downloads the SQLite amalgamation when `sqlite3.c` is absent; `sqldir` and `sqlver` in `mako.yaml` select its release. The output is a `mako` executable and its `mako.zip` resource archive.
