# Compiling and Installing Mako Server with Ansible

This guide walks you through using an Ansible playbook to **compile** and **optionally install** and launch the Lua tutorials for the Mako Server, either on your **local computer** or on a **target computer** such as a **Raspberry Pi** or any Linux target computer where Python is installed. **Ansible** is an automation tool that simplifies software provisioning. You do not need Ansible experience for using our ready to run playbook.

- **Windows Users:** [Use WSL](https://learn.microsoft.com/en-us/windows/wsl/install)

One of the main benefits of this approach is how easy it makes it to **update and recompile** the binary whenever any of the GitHub repositories are updated. Simply re-run the playbook to rebuild the server with the latest changes.

This is especially **important** if you're using the Mako Server to **generate ECC certificates** or if you're relying on the built-in **softTPM** - whether implicitly or explicitly. In these cases, it's crucial to preserve the auto-generated pre-master secret. This secret is stored in a generated C header file the first time the server is compiled:

- BAS/examples/MakoServer/src/NewEncryptionKey.h

Be sure **not to delete or overwrite** this file when recompiling, as it plays a key role in securing your application. For more information, refer to the [soft Trusted Platform Module documentation](https://realtimelogic.com/ba/examples/MakoServer/readme.html#TPM).

## Prerequisites

Before you start, ensure you're on a Debian-based system and have Ansible installed and that you have download the Mako Server Ansible script:

``` bash
sudo apt update && sudo apt install ansible -y
wget https://raw.githubusercontent.com/RealTimeLogic/BAS/refs/heads/main/mako.yaml
```

## Running the Playbook

### Install on a Local Machine

To compile, install, and run tutorials locally:

``` bash
ansible-playbook -i localhost, -c local mako.yaml -e install=true -e tutorials=true
```

**Note:**
- You can customize the playbook run with [optional flags](#optional-playbook-flags) like `-e install=true`.
- The Playbook requires [passwordless sudo access](https://gcore.com/learning/how-to-disable-password-for-sudo-command). If you do not want to enable this feature, you can add the option `--ask-become-pass`

#### Cross-Compiling (for experts)
To cross-compile, set the CROSS_COMPILE environment variable to the compiler's prefix before running the playbook. For example, if the compiler is not in the path and the GCC compiler name is arm-linux-gnueabihf-gcc, set an environment variable similar to:

``` bash
export CROSS_COMPILE=/opt/gcc-linaro-7.1.1-2017.08-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
```

### Install on a Remote Machine (e.g. embedded Linux)

Ansible makes it easy to install on a device such as Raspberry Pi. You run Ansible on your host computer, not the target computer.

To target a remote device (for example, a NanoPi):

``` bash
ssh-copy-id debian@192.168.1.146
ansible-playbook -i 192.168.1.146, -u debian mako.yaml -e install=true -e tutorials=true
```

Replace 192.168.1.146 with your target machine's IP address (or hostname/domain name), and debian with the appropriate SSH user.

**Note:**
- The `ssh-copy-id` command is necesarry if you have not copied your public SSH key to the remote computer. Ansible requires passwordless login via SSH.
- When remote compiling directly on a device, the required build tools consume approximately 250 MB of disk space.

## Optional Playbook Flags

You can customize the behavior with these variables:

- install=true     - Installs the compiled binaries to /usr/local/bin
- tutorials=true   - Launches the Lua tutorial project after install

## 🌍 Accessing the Tutorials

After the playbook runs (with tutorials=true), open a browser and go to:

http://localhost           (for local runs)  
http:/target-ip         (for remote runs)


## Notes

- Git repositories are updated each time the playbook runs.

- With option `tutorials=true` and if the server cannot open port 80, it will be listening on port 9357; thus, you will need to navigate to http://localhost:9357

- Running the tutorials will stop any existing Mako server instance (using killall mako), then launch a new one using your current user.

Feel free to extend this with additional features like systemd integration or logging flags if needed.

### What This Playbook Does

The playbook performs the following tasks:

1. Ensures required system packages are installed (git, gcc, etc.).
2. Clones the necessary Git repositories:
   - BAS
   - BAS-Resources
   - LPeg, lua-protobuf, and CBOR libraries
3. Downloads and extracts the SQLite amalgamation (if missing). Note that the SQLite version and year are in the two variables sqlver and sqldir.
4. Compiles the Mako Server.
5. Optionally installs the mako binary and mako.zip to /usr/local/bin.
6. Optionally starts the Lua tutorial launcher using the built server.

#### Example

The following shows Ansible compiling, installing, and running the Mako Server tutorials.


```bash
ansible-playbook -i localhost, -c local mako.yaml -e install=true -e tutorials=true

PLAY [Build and optionally install Mako Server] ************************************************************************

TASK [Gathering Facts] *************************************************************************************************
ok: [localhost]

TASK [Ensure base directory exists] ************************************************************************************
changed: [localhost]

TASK [Ensure required packages are installed] **************************************************************************
ok: [localhost]

TASK [Clone BAS repository if not present] *****************************************************************************
changed: [localhost]

TASK [Clone BAS-Resources repository if not present] *******************************************************************
changed: [localhost]

TASK [Make build scripts in BAS-Resources executable] ******************************************************************
ok: [localhost]

TASK [Clone LPeg if not present] ***************************************************************************************
changed: [localhost]

TASK [Clone lua-protobuf if not present] *******************************************************************************
changed: [localhost]

TASK [Clone CBOR if not present] ***************************************************************************************
changed: [localhost]

TASK [Check if sqlite3.c exists] ***************************************************************************************
ok: [localhost]

TASK [Download SQLite amalgamation] ************************************************************************************
ok: [localhost]

TASK [Unzip SQLite] ****************************************************************************************************
changed: [localhost]

TASK [Move SQLite files to BAS/src] ************************************************************************************
changed: [localhost]

TASK [Compile Mako Server] *********************************************************************************************
changed: [localhost]

TASK [Install Mako Server binary] **************************************************************************************
changed: [localhost]

TASK [Install Mako zip file] *******************************************************************************************
changed: [localhost]

TASK [Run Lua tutorials] ***********************************************************************************************
changed: [localhost]

PLAY RECAP *************************************************************************************************************
localhost                  : ok=17   changed=12   unreachable=0    failed=0    skipped=0    rescued=0    ignored=0
```
