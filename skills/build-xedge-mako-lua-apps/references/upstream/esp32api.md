

---

# Source: ESP32/genindex.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="source/GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="source/AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="source/LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="source/xedge.html" class="reference internal">Xedge32</a>
- <a href="source/ADC.html" class="reference internal">ADC API</a>
- <a href="source/cam.html" class="reference internal">Camera API</a>
- <a href="source/GPIO.html" class="reference internal">GPIO API</a>
- <a href="source/I2C.html" class="reference internal">I2C API</a>
- <a href="source/PCNT.html" class="reference internal">PCNT API</a>
- <a href="source/PWM.html" class="reference internal">PWM API</a>
- <a href="source/RMT.html" class="reference internal">RMT API</a>
- <a href="source/UART.html" class="reference internal">UART API</a>
- <a href="source/misc.html" class="reference internal">Miscellaneous API</a>
- <a href="source/BME280.html" class="reference internal">BME280 Module</a>
- <a href="source/RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="source/tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="source/applications.html" class="reference internal">Applications</a>
- <a href="source/license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="index.html" class="icon icon-home" aria-label="Home"></a>
- Index
- 

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

# Index

<div class="genindex-jumpbox">

</div>

</div>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/index.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="#" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="source/GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="source/AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="source/LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="source/xedge.html" class="reference internal">Xedge32</a>
- <a href="source/ADC.html" class="reference internal">ADC API</a>
- <a href="source/cam.html" class="reference internal">Camera API</a>
- <a href="source/GPIO.html" class="reference internal">GPIO API</a>
- <a href="source/I2C.html" class="reference internal">I2C API</a>
- <a href="source/PCNT.html" class="reference internal">PCNT API</a>
- <a href="source/PWM.html" class="reference internal">PWM API</a>
- <a href="source/RMT.html" class="reference internal">RMT API</a>
- <a href="source/UART.html" class="reference internal">UART API</a>
- <a href="source/misc.html" class="reference internal">Miscellaneous API</a>
- <a href="source/BME280.html" class="reference internal">BME280 Module</a>
- <a href="source/RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="source/tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="source/applications.html" class="reference internal">Applications</a>
- <a href="source/license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](#)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="#" class="icon icon-home" aria-label="Home"></a>
- Xedge32 Introduction
- <a href="_sources/index.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="xedge32-introduction" class="section">

# Xedge32 Introduction<a href="#xedge32-introduction" class="headerlink" title="Link to this heading"></a>

Xedge32 is the ESP32 edition of <a href="https://realtimelogic.com/ba/doc/?url=Xedge.html" class="reference external">Xedge</a>. It lets you build and test Lua applications directly on an ESP32 board, while also giving Lua code access to ESP32 hardware such as GPIO, I2C, UART, PWM, ADC, RMT, camera modules, networking, storage, and firmware update services.

If you are new to BAS, Xedge, and Xedge32, start with the overview article <a href="https://realtimelogic.com/articles/How-Xedge32-Xedge-and-Barracuda-App-Server-Work-Together" class="reference external">How Xedge32, Xedge, and Barracuda App Server Work Together</a>. It explains the relationship between the general Barracuda App Server runtime, the generic Xedge development environment, and this ESP32-specific firmware.


<div id="what-this-manual-covers" class="section">

## What This Manual Covers<a href="#what-this-manual-covers" class="headerlink" title="Link to this heading"></a>

This manual focuses on what is specific to Xedge32:

- installing and configuring the ESP32 firmware,

- using the Xedge workflow on an ESP32 board, and

- controlling ESP32 hardware through the <span class="pre">`esp32`</span> Lua module.

It does not repeat the full generic Xedge or Barracuda App Server manuals. Use those documents when you need the broader web, security, protocol, file system, or application framework APIs:

- <a href="https://realtimelogic.com/ba/doc/?url=Xedge.html" class="reference external">Xedge documentation</a>

- <a href="https://realtimelogic.com/ba/doc/?url=lua.html" class="reference external">Barracuda App Server Lua API</a>

- <a href="https://realtimelogic.com/ba/doc/?url=IoT.html" class="reference external">IoT protocol APIs</a>

</div>

<div id="the-three-layers" class="section">

## The Three Layers<a href="#the-three-layers" class="headerlink" title="Link to this heading"></a>

Xedge32 use three layers together:

**Barracuda App Server**  
The portable embedded application server foundation. It provides the web server, Lua integration, security, protocols, I/O abstractions, and runtime services.

**Xedge**  
The Lua application model and browser-based development environment. It is not ESP32-specific.

**Xedge32**  
The ESP32 firmware that combines Xedge with ESP32-specific Lua APIs. This is the layer that lets Lua code work with pins, buses, sensors, and other board features.

For a DIY maker, the practical result is simple: flash Xedge32, open the web interface or LuaShell32, and start testing Lua code against real hardware without first writing C firmware.

</div>

<div id="lua-and-c" class="section">

## Lua and C<a href="#lua-and-c" class="headerlink" title="Link to this heading"></a>

The built-in APIs are Lua APIs. You normally do not need C code to read a sensor, toggle a pin, connect Wi-Fi, or build a small device dashboard.

The firmware itself is written in C and wraps selected ESP-IDF features for Lua. Developers who need custom native features can build and extend the firmware from the <a href="https://github.com/RealTimeLogic/Xedge32" class="reference external">Xedge32 repository</a>, but that is an advanced workflow. Start with Lua unless you have a specific reason to change the firmware.

</div>

<div id="callbacks-and-polling" class="section">

## Callbacks and Polling<a href="#callbacks-and-polling" class="headerlink" title="Link to this heading"></a>

Many ESP32 peripherals can work in an event-driven style. When an API provides a callback, prefer that callback for responsive applications. Polling with a timer is still useful for slow checks, simple experiments, and one-shot reads, but callbacks are usually the better model for serial input, GPIO interrupts, RMT receive jobs, PCNT watchpoints, and similar events.

</div>

<div id="hardware-resource-lifetime" class="section">

## Hardware Resource Lifetime<a href="#hardware-resource-lifetime" class="headerlink" title="Link to this heading"></a>

Most hardware APIs return Lua objects such as GPIO, UART, I2C, PWM, PCNT, RMT, ADC, or camera objects. Keep a reference to the object for as long as you need the hardware to stay open, and call <span class="pre">`:close()`</span> when you are done.

Short experiments can also use Lua’s <span class="pre">`<close>`</span> syntax:

<div class="highlight-lua notranslate">

<div class="highlight">

    local led <close> = esp32.gpio(18, "OUT")
    led:value(true)
    ba.sleep(1000)

</div>

</div>

This is convenient in Lua Server Pages and during hot reloading because the resource is released automatically at the end of the scope. Long-running applications should keep explicit references to active hardware objects so the garbage collector cannot release them unexpectedly.

</div>

<div id="pin-safety" class="section">

## Pin Safety<a href="#pin-safety" class="headerlink" title="Link to this heading"></a>

ESP32 boards vary. A GPIO number that is safe on one board may be connected to flash, PSRAM, USB, boot strapping, a camera, an SD card, or another fixed function on another board. Start with the pinout for your exact board, avoid reserved pins, and use current-limiting resistors when driving LEDs or other loads directly.

</div>

</div>

<div id="xedge-esp32" class="section">

# Xedge ESP32<a href="#xedge-esp32" class="headerlink" title="Link to this heading"></a>

<div class="toctree-wrapper compound">

<span class="caption-text">Contents:</span>

- <a href="source/GettingStarted.html" class="reference internal">Getting Started</a>
  - <a href="source/GettingStarted.html#first-time-installers" class="reference internal">First-Time Installers</a>
  - <a href="source/GettingStarted.html#ota-upgrade" class="reference internal">OTA Upgrade</a>
  - <a href="source/GettingStarted.html#advanced-installation-and-upgrade" class="reference internal">Advanced Installation and Upgrade</a>
    - <a href="source/GettingStarted.html#firmware-option-1-use-precompiled-firmware" class="reference internal">Firmware Option 1: Use Precompiled Firmware</a>
    - <a href="source/GettingStarted.html#firmware-option-2-compile-the-code" class="reference internal">Firmware Option 2: Compile the Code</a>
  - <a href="source/GettingStarted.html#configure-the-esp32" class="reference internal">Configure the ESP32</a>
    - <a href="source/GettingStarted.html#using-wi-fi" class="reference internal">Using Wi-Fi</a>
    - <a href="source/GettingStarted.html#using-ethernet" class="reference internal">Using Ethernet</a>
  - <a href="source/GettingStarted.html#next-step" class="reference internal">Next Step</a>
  - <a href="source/GettingStarted.html#firmware-upgrade-options" class="reference internal">Firmware Upgrade Options</a>
  - <a href="source/GettingStarted.html#support-and-discussions" class="reference internal">Support and Discussions</a>
- <a href="source/AccessPointMode.html" class="reference internal">Access Point Mode</a>
  - <a href="source/AccessPointMode.html#connecting-to-xedge32" class="reference internal">Connecting to Xedge32</a>
  - <a href="source/AccessPointMode.html#access-point-mode-considerations" class="reference internal">Access Point Mode Considerations</a>
  - <a href="source/AccessPointMode.html#switching-to-station-mode" class="reference internal">Switching to Station Mode</a>
  - <a href="source/AccessPointMode.html#reconnecting-after-the-mode-change" class="reference internal">Reconnecting After the Mode Change</a>
  - <a href="source/AccessPointMode.html#station-mode-considerations" class="reference internal">Station Mode Considerations</a>
  - <a href="source/AccessPointMode.html#switching-back-to-access-point-mode" class="reference internal">Switching Back to Access Point Mode</a>
- <a href="source/LuaShell32.html" class="reference internal">LuaShell32</a>
  - <a href="source/LuaShell32.html#starting-the-shell" class="reference internal">Starting the Shell</a>
  - <a href="source/LuaShell32.html#what-the-prompt-looks-like" class="reference internal">What the Prompt Looks Like</a>
  - <a href="source/LuaShell32.html#running-your-first-command" class="reference internal">Running Your First Command</a>
  - <a href="source/LuaShell32.html#working-interactively" class="reference internal">Working Interactively</a>
  - <a href="source/LuaShell32.html#what-you-can-access" class="reference internal">What You Can Access</a>
  - <a href="source/LuaShell32.html#when-to-use-luashell32" class="reference internal">When to Use LuaShell32</a>
- <a href="source/xedge.html" class="reference internal">Xedge32</a>
  - <a href="source/xedge.html#working-with-example-applications" class="reference internal">Working with Example Applications</a>
  - <a href="source/xedge.html#uploading-files-to-the-esp32" class="reference internal">Uploading Files to the ESP32</a>
  - <a href="source/xedge.html#creating-an-app-from-the-uploaded-directory" class="reference internal">Creating an App from the Uploaded Directory</a>
  - <a href="source/xedge.html#using-an-external-ide" class="reference internal">Using an External IDE</a>
  - <a href="source/xedge.html#debugging-lua-code" class="reference internal">Debugging Lua Code</a>
  - <a href="source/xedge.html#debugging-workflow" class="reference internal">Debugging Workflow</a>
  - <a href="source/xedge.html#starting-the-first-debug-session" class="reference internal">Starting the First Debug Session</a>
  - <a href="source/xedge.html#further-reading" class="reference internal">Further Reading</a>
- <a href="source/ADC.html" class="reference internal">ADC API</a>
  - <a href="source/ADC.html#one-shot-mode" class="reference internal">One-Shot Mode</a>
    - <a href="source/ADC.html#parameters" class="reference internal">Parameters</a>
    - <a href="source/ADC.html#return-values" class="reference internal">Return Values</a>
    - <a href="source/ADC.html#adc-object-methods" class="reference internal">ADC Object Methods</a>
    - <a href="source/ADC.html#one-shot-example" class="reference internal">One-Shot Example</a>
  - <a href="source/ADC.html#continuous-mode" class="reference internal">Continuous Mode</a>
    - <a href="source/ADC.html#required-continuous-mode-options" class="reference internal">Required continuous-mode options</a>
    - <a href="source/ADC.html#filters" class="reference internal">Filters</a>
    - <a href="source/ADC.html#callback-signatures" class="reference internal">Callback Signatures</a>
    - <a href="source/ADC.html#continuous-example-mean-filter" class="reference internal">Continuous Example: <span class="pre"><code class="docutils literal notranslate">mean</code></span> Filter</a>
    - <a href="source/ADC.html#continuous-example-data-filter" class="reference internal">Continuous Example: <span class="pre"><code class="docutils literal notranslate">data</code></span> Filter</a>
  - <a href="source/ADC.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/cam.html" class="reference internal">Camera API</a>
  - <a href="source/cam.html#creating-a-camera-object" class="reference internal">Creating a Camera Object</a>
    - <a href="source/cam.html#parameters" class="reference internal">Parameters</a>
    - <a href="source/cam.html#return-values" class="reference internal">Return Values</a>
  - <a href="source/cam.html#camera-object-methods" class="reference internal">Camera Object Methods</a>
    - <a href="source/cam.html#cam-read" class="reference internal"><span class="pre"><code class="docutils literal notranslate">cam:read()</code></span></a>
    - <a href="source/cam.html#cam-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">cam:close()</code></span></a>
  - <a href="source/cam.html#configuration-table" class="reference internal">Configuration Table</a>
    - <a href="source/cam.html#required-fields" class="reference internal">Required fields</a>
    - <a href="source/cam.html#optional-fields" class="reference internal">Optional fields</a>
  - <a href="source/cam.html#configuration-examples" class="reference internal">Configuration Examples</a>
  - <a href="source/cam.html#esp32-p4-configuration" class="reference internal">ESP32-P4 Configuration</a>
  - <a href="source/cam.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/GPIO.html" class="reference internal">GPIO API</a>
  - <a href="source/GPIO.html#creating-a-gpio-object" class="reference internal">Creating a GPIO Object</a>
    - <a href="source/GPIO.html#parameters" class="reference internal">Parameters</a>
  - <a href="source/GPIO.html#gpio-object-methods" class="reference internal">GPIO Object Methods</a>
    - <a href="source/GPIO.html#gpio-value-val" class="reference internal"><span class="pre"><code class="docutils literal notranslate">gpio:value([val])</code></span></a>
    - <a href="source/GPIO.html#gpio-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">gpio:close()</code></span></a>
  - <a href="source/GPIO.html#examples" class="reference internal">Examples</a>
    - <a href="source/GPIO.html#example-1-set-gpio-18-high" class="reference internal">Example 1: Set GPIO 18 High</a>
    - <a href="source/GPIO.html#example-2-toggle-an-led-through-garbage-collection" class="reference internal">Example 2: Toggle an LED Through Garbage Collection</a>
    - <a href="source/GPIO.html#example-3-read-a-button-state" class="reference internal">Example 3: Read a Button State</a>
    - <a href="source/GPIO.html#example-4-react-to-a-button-with-an-interrupt-callback" class="reference internal">Example 4: React to a Button with an Interrupt Callback</a>
  - <a href="source/GPIO.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/I2C.html" class="reference internal">I2C API</a>
  - <a href="source/I2C.html#creating-an-i2c-master" class="reference internal">Creating an I2C Master</a>
    - <a href="source/I2C.html#parameters" class="reference internal">Parameters</a>
    - <a href="source/I2C.html#return-value" class="reference internal">Return Value</a>
  - <a href="source/I2C.html#i2c-master-object-methods" class="reference internal">I2C Master Object Methods</a>
    - <a href="source/I2C.html#i2cm-probe-address-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:probe(address</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">timeout])</code></span></a>
    - <a href="source/I2C.html#i2cm-read-address-len-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:read(address,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">len</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">timeout])</code></span></a>
    - <a href="source/I2C.html#i2cm-readfrom-address-registerorbytes-len-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:readfrom(address,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">registerOrBytes,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">len</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">timeout])</code></span></a>
    - <a href="source/I2C.html#i2cm-write-address-data-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:write(address,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">data</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">timeout])</code></span></a>
    - <a href="source/I2C.html#i2cm-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:close()</code></span></a>
  - <a href="source/I2C.html#example" class="reference internal">Example</a>
  - <a href="source/I2C.html#simple-address-scanner" class="reference internal">Simple Address Scanner</a>
  - <a href="source/I2C.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/PCNT.html" class="reference internal">PCNT API</a>
  - <a href="source/PCNT.html#why-use-pcnt" class="reference internal">Why Use PCNT</a>
  - <a href="source/PCNT.html#key-features" class="reference internal">Key Features</a>
  - <a href="source/PCNT.html#creating-a-pcnt-object" class="reference internal">Creating a PCNT Object</a>
  - <a href="source/PCNT.html#top-level-configuration-fields" class="reference internal">Top-Level Configuration Fields</a>
  - <a href="source/PCNT.html#watchpoint-configuration" class="reference internal">Watchpoint Configuration</a>
  - <a href="source/PCNT.html#channel-configuration" class="reference internal">Channel Configuration</a>
  - <a href="source/PCNT.html#pcnt-object-methods" class="reference internal">PCNT Object Methods</a>
  - <a href="source/PCNT.html#rotary-encoder-example" class="reference internal">Rotary Encoder Example</a>
  - <a href="source/PCNT.html#how-the-example-works" class="reference internal">How the Example Works</a>
  - <a href="source/PCNT.html#hardware-setup" class="reference internal">Hardware Setup</a>
    - <a href="source/PCNT.html#wiring-example" class="reference internal">Wiring Example</a>
  - <a href="source/PCNT.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/PWM.html" class="reference internal">PWM API</a>
  - <a href="source/PWM.html#typical-pwm-workflow" class="reference internal">Typical PWM Workflow</a>
  - <a href="source/PWM.html#quick-start-example" class="reference internal">Quick Start Example</a>
  - <a href="source/PWM.html#creating-a-pwm-timer" class="reference internal">Creating a PWM Timer</a>
  - <a href="source/PWM.html#creating-a-pwm-channel" class="reference internal">Creating a PWM Channel</a>
  - <a href="source/PWM.html#channel-object-methods" class="reference internal">Channel Object Methods</a>
    - <a href="source/PWM.html#channel-duty-pwmdutycycle-hpoint" class="reference internal"><span class="pre"><code class="docutils literal notranslate">channel:duty(pwmDutyCycle</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">hpoint])</code></span></a>
    - <a href="source/PWM.html#channel-fade-pwmdutycycle-time" class="reference internal"><span class="pre"><code class="docutils literal notranslate">channel:fade(pwmDutyCycle,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">time)</code></span></a>
    - <a href="source/PWM.html#channel-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">channel:close()</code></span></a>
  - <a href="source/PWM.html#example-1-led-fading-with-callback" class="reference internal">Example 1: LED Fading with Callback</a>
  - <a href="source/PWM.html#example-2-servo-sweep" class="reference internal">Example 2: Servo Sweep</a>
  - <a href="source/PWM.html#important-runtime-note" class="reference internal">Important Runtime Note</a>
  - <a href="source/PWM.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/RMT.html" class="reference internal">RMT API</a>
  - <a href="source/RMT.html#application-examples" class="reference internal">Application Examples</a>
  - <a href="source/RMT.html#rmt-symbol-layout" class="reference internal">RMT Symbol Layout</a>
  - <a href="source/RMT.html#understanding-tick-resolution" class="reference internal">Understanding Tick Resolution</a>
  - <a href="source/RMT.html#lua-rmt-symbol-representation" class="reference internal">Lua RMT Symbol Representation</a>
  - <a href="source/RMT.html#lua-rmt-byte-encoding" class="reference internal">Lua RMT Byte Encoding</a>
  - <a href="source/RMT.html#rmt-tx-api" class="reference internal">RMT TX API</a>
  - <a href="source/RMT.html#creating-a-tx-channel" class="reference internal">Creating a TX Channel</a>
  - <a href="source/RMT.html#tx-configuration-options" class="reference internal">TX Configuration Options</a>
  - <a href="source/RMT.html#tx-object-methods" class="reference internal">TX Object Methods</a>
    - <a href="source/RMT.html#rmttx-enable" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmttx:enable()</code></span></a>
    - <a href="source/RMT.html#rmttx-disable" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmttx:disable()</code></span></a>
    - <a href="source/RMT.html#rmttx-transmit-cfg-symbols" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmttx:transmit(cfg,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">symbols)</code></span></a>
    - <a href="source/RMT.html#rmttx-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmttx:close()</code></span></a>
  - <a href="source/RMT.html#tx-example-1-musical-score" class="reference internal">TX Example 1: Musical Score</a>
    - <a href="source/RMT.html#how-the-musical-example-works" class="reference internal">How the Musical Example Works</a>
  - <a href="source/RMT.html#tx-example-2-ws2812b-led-strip" class="reference internal">TX Example 2: WS2812B LED Strip</a>
    - <a href="source/RMT.html#breaking-down-the-led-example" class="reference internal">Breaking Down the LED Example</a>
  - <a href="source/RMT.html#rmt-rx-api" class="reference internal">RMT RX API</a>
  - <a href="source/RMT.html#creating-an-rx-channel" class="reference internal">Creating an RX Channel</a>
  - <a href="source/RMT.html#rx-configuration-options" class="reference internal">RX Configuration Options</a>
  - <a href="source/RMT.html#rx-object-methods" class="reference internal">RX Object Methods</a>
    - <a href="source/RMT.html#rmtrx-receive-cfg" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmtrx:receive(cfg)</code></span></a>
    - <a href="source/RMT.html#rmtrx-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmtrx:close()</code></span></a>
  - <a href="source/RMT.html#rx-example-1-wire-temperature-read" class="reference internal">RX Example: 1-Wire Temperature Read</a>
    - <a href="source/RMT.html#understanding-the-1-wire-example" class="reference internal">Understanding the 1-Wire Example</a>
    - <a href="source/RMT.html#garbage-collection-consideration" class="reference internal">Garbage-Collection Consideration</a>
  - <a href="source/RMT.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/UART.html" class="reference internal">UART API</a>
  - <a href="source/UART.html#creating-a-uart-object" class="reference internal">Creating a UART Object</a>
    - <a href="source/UART.html#parameters" class="reference internal">Parameters</a>
  - <a href="source/UART.html#receive-callback-signatures" class="reference internal">Receive Callback Signatures</a>
  - <a href="source/UART.html#uart-object-methods" class="reference internal">UART Object Methods</a>
    - <a href="source/UART.html#uart-read-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">uart:read([timeout])</code></span></a>
    - <a href="source/UART.html#uart-write-data" class="reference internal"><span class="pre"><code class="docutils literal notranslate">uart:write(data)</code></span></a>
    - <a href="source/UART.html#uart-txsize" class="reference internal"><span class="pre"><code class="docutils literal notranslate">uart:txsize()</code></span></a>
    - <a href="source/UART.html#uart-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">uart:close()</code></span></a>
  - <a href="source/UART.html#uart-echo-example" class="reference internal">UART Echo Example</a>
  - <a href="source/UART.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/misc.html" class="reference internal">Miscellaneous API</a>
  - <a href="source/misc.html#esp32-apinfo" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.apinfo()</code></span></a>
  - <a href="source/misc.html#esp32-loglevel-level" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.loglevel(level)</code></span></a>
  - <a href="source/misc.html#esp32-mac" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.mac()</code></span></a>
  - <a href="source/misc.html#esp32-wscan-print" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.wscan([print])</code></span></a>
  - <a href="source/misc.html#esp32-netconnect-network-cfg" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.netconnect(network,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">cfg)</code></span></a>
    - <a href="source/misc.html#network-parameters" class="reference internal">Network Parameters</a>
  - <a href="source/misc.html#esp32-sdcard-width" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.sdcard(width)</code></span></a>
    - <a href="source/misc.html#sd-card-parameters" class="reference internal">SD Card Parameters</a>
    - <a href="source/misc.html#default-pins" class="reference internal">Default Pins</a>
    - <a href="source/misc.html#return-behavior" class="reference internal">Return Behavior</a>
  - <a href="source/misc.html#esp32-execute-command" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.execute(command)</code></span></a>
  - <a href="source/misc.html#xedge-event" class="reference internal"><span class="pre"><code class="docutils literal notranslate">xedge.event()</code></span></a>
    - <a href="source/misc.html#event-types" class="reference internal">Event Types</a>
    - <a href="source/misc.html#event-examples" class="reference internal">Event Examples</a>
    - <a href="source/misc.html#one-time-sntp-subscription-example" class="reference internal">One-Time SNTP Subscription Example</a>
  - <a href="source/misc.html#xedge32-ota" class="reference internal">Xedge32 OTA</a>
    - <a href="source/misc.html#examples" class="reference internal">Examples</a>
    - <a href="source/misc.html#ota-object-methods" class="reference internal">OTA Object Methods</a>
  - <a href="source/misc.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/BME280.html" class="reference internal">BME280 Module</a>
  - <a href="source/BME280.html#typical-workflow" class="reference internal">Typical Workflow</a>
  - <a href="source/BME280.html#creating-a-bme280-object" class="reference internal">Creating a BME280 Object</a>
    - <a href="source/BME280.html#parameters" class="reference internal">Parameters</a>
    - <a href="source/BME280.html#return-value" class="reference internal">Return Value</a>
  - <a href="source/BME280.html#bme280-object-methods" class="reference internal">BME280 Object Methods</a>
    - <a href="source/BME280.html#bme-read" class="reference internal"><span class="pre"><code class="docutils literal notranslate">bme:read()</code></span></a>
  - <a href="source/BME280.html#basic-example" class="reference internal">Basic Example</a>
  - <a href="source/BME280.html#practical-notes" class="reference internal">Practical Notes</a>
  - <a href="source/BME280.html#online-examples" class="reference internal">Online Examples</a>
- <a href="source/RTU.html" class="reference internal">Modbus RTU Module</a>
  - <a href="source/RTU.html#creating-an-rtu-client" class="reference internal">Creating an RTU Client</a>
    - <a href="source/RTU.html#parameters" class="reference internal">Parameters</a>
    - <a href="source/RTU.html#return-value" class="reference internal">Return Value</a>
  - <a href="source/RTU.html#basic-example" class="reference internal">Basic Example</a>
  - <a href="source/RTU.html#rs-232-full-duplex-rs-485-and-half-duplex-rs-485" class="reference internal">RS-232, Full-Duplex RS-485, and Half-Duplex RS-485</a>
  - <a href="source/RTU.html#modbus-test-bench" class="reference internal">Modbus Test Bench</a>
    - <a href="source/RTU.html#test-bench-components" class="reference internal">Test Bench Components</a>
    - <a href="source/RTU.html#wiring-example" class="reference internal">Wiring Example</a>
  - <a href="source/RTU.html#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="source/tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
  - <a href="source/tutorials.html#recommended-learning-resources" class="reference internal">Recommended Learning Resources</a>
  - <a href="source/tutorials.html#how-xedge32-fits-together" class="reference internal">How Xedge32 Fits Together</a>
  - <a href="source/tutorials.html#why-this-matters" class="reference internal">Why This Matters</a>
- <a href="source/applications.html" class="reference internal">Applications</a>
  - <a href="source/applications.html#creating-and-managing-xedge32-applications" class="reference internal">Creating and Managing Xedge32 Applications</a>
  - <a href="source/applications.html#uploading-your-application" class="reference internal">Uploading Your Application</a>
  - <a href="source/applications.html#zip-file-applications" class="reference internal">ZIP File Applications</a>
  - <a href="source/applications.html#creating-an-xedge32-application" class="reference internal">Creating an Xedge32 Application</a>
  - <a href="source/applications.html#optional-config-script" class="reference internal">Optional <span class="pre"><code class="docutils literal notranslate">.config</code></span> Script</a>
  - <a href="source/applications.html#example-config-script" class="reference internal">Example <span class="pre"><code class="docutils literal notranslate">.config</code></span> Script</a>
- <a href="source/license.html" class="reference internal">License</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="source/GettingStarted.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Getting Started">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/ADC.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="#" class="current reference internal">ADC API</a>
  - <a href="#one-shot-mode" class="reference internal">One-Shot Mode</a>
    - <a href="#parameters" class="reference internal">Parameters</a>
    - <a href="#return-values" class="reference internal">Return Values</a>
    - <a href="#adc-object-methods" class="reference internal">ADC Object Methods</a>
    - <a href="#one-shot-example" class="reference internal">One-Shot Example</a>
  - <a href="#continuous-mode" class="reference internal">Continuous Mode</a>
    - <a href="#required-continuous-mode-options" class="reference internal">Required continuous-mode options</a>
    - <a href="#filters" class="reference internal">Filters</a>
    - <a href="#callback-signatures" class="reference internal">Callback Signatures</a>
    - <a href="#continuous-example-mean-filter" class="reference internal">Continuous Example: <span class="pre"><code class="docutils literal notranslate">mean</code></span> Filter</a>
    - <a href="#continuous-example-data-filter" class="reference internal">Continuous Example: <span class="pre"><code class="docutils literal notranslate">data</code></span> Filter</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- ADC API
- <a href="../_sources/source/ADC.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="adc-api" class="section">

# ADC API<a href="#adc-api" class="headerlink" title="Link to this heading"></a>

The Analog-to-Digital Converter (ADC) API lets Lua code measure analog voltages using the ESP32’s built-in ADC hardware. This is the API you use when reading potentiometers, light sensors, simple analog outputs from other circuits, and similar low-bandwidth analog signals.

Xedge32 exposes two ADC usage models:

- **One-shot mode**, which is easy to use and well suited for occasional polling.

- **Continuous mode**, which samples in the background and delivers data through a callback.

For most applications, start with one-shot mode. Move to continuous mode only when you need steady high-rate sampling.

<div class="admonition important">

Important

The ESP32 ADC input is not tolerant of arbitrary voltages. Keep the signal within the safe input range for your exact board and attenuation setting. Use a resistor divider or sensor module output that is designed for 3.3 V logic when measuring higher voltages.

</div>

ADC readings are useful for sensors and control inputs, but they are not a precision instrument. Expect noise, board-to-board variation, and some non-linearity. For stable readings, average multiple samples or use continuous mode with the <span class="pre">`mean`</span> filter.

<div id="one-shot-mode" class="section">

## One-Shot Mode<a href="#one-shot-mode" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    adc, pinOrErr = esp32.adc(unit, channel [, cfg])

</div>

</div>

<div id="parameters" class="section">

### Parameters<a href="#parameters" class="headerlink" title="Link to this heading"></a>

- <span class="pre">`unit`</span>: ADC unit number. ESP32 devices provide ADC unit <span class="pre">`1`</span> and <span class="pre">`2`</span>.

- <span class="pre">`channel`</span>: ADC channel number within the selected unit.

- <span class="pre">`cfg`</span>: Optional configuration table.

Supported configuration options:

- <span class="pre">`attenuation`</span>: One of <span class="pre">`"0db"`</span>, <span class="pre">`"2.5db"`</span>, <span class="pre">`"6db"`</span>, or <span class="pre">`"11db"`</span>. The default is <span class="pre">`"11db"`</span>.

- <span class="pre">`volt`</span>: Boolean. When <span class="pre">`true`</span>, <span class="pre">`adc:read()`</span> returns both the raw ADC value and the value converted to millivolts.

- <span class="pre">`bitwidth`</span>: Number from <span class="pre">`9`</span> to <span class="pre">`13`</span>. The default is <span class="pre">`13`</span>.

Higher attenuation settings allow a larger input range, but they do not make the ADC pin safe for voltages above the ESP32’s limits. Always check the board data sheet.

</div>

<div id="return-values" class="section">

### Return Values<a href="#return-values" class="headerlink" title="Link to this heading"></a>

On success, <span class="pre">`esp32.adc()`</span> returns:

- an ADC object, and

- the GPIO pin number used by the selected channel.

On failure, the first return value is <span class="pre">`nil`</span> and the second value contains an error code or diagnostic string.

</div>

<div id="adc-object-methods" class="section">

### ADC Object Methods<a href="#adc-object-methods" class="headerlink" title="Link to this heading"></a>

<span class="pre">`adc:read()`</span>  
Returns the current sample. If <span class="pre">`volt=true`</span> was set, the method returns both the raw value and the converted millivolt value.

<span class="pre">`adc:close()`</span>  
Releases the ADC resources associated with the object.

</div>

<div id="one-shot-example" class="section">

### One-Shot Example<a href="#one-shot-example" class="headerlink" title="Link to this heading"></a>

The example below opens ADC unit 1, channel 0, and polls it once per second. The configuration requests both the raw reading and the converted millivolt value.

<div class="highlight-lua notranslate">

<div class="highlight">

    local adc, pin = esp32.adc(1, 0, {volt = true})
    if adc then
       trace("ADC is using GPIO pin", pin)

       local function pollADC()
          local raw, volt = adc:read()
          trace(string.format("ADC raw data: %d, volt: %d", raw, volt))
          return true
       end

       ba.timer(pollADC):set(1000)
    else
       trace("Failed to open ADC", pin)
    end

</div>

</div>

</div>

</div>

<div id="continuous-mode" class="section">

## Continuous Mode<a href="#continuous-mode" class="headerlink" title="Link to this heading"></a>

<div class="admonition note">

Note

Continuous mode is currently experimental.

</div>

Continuous mode is designed for applications that need repeated sampling at a much higher rate than typical timer-based polling. The ESP32 samples in the background and Lua receives processed results through a callback.

Because Lua cannot handle a callback for every single hardware sample, continuous mode uses a buffering and filtering layer. Internally, the ADC may sample at rates as high as <span class="pre">`48`</span>` `<span class="pre">`kHz`</span>, but the Lua callback is limited to a much lower rate that is practical for the runtime.

On ESP32 hardware, the effective sampling rate floor is around <span class="pre">`20,000`</span> Hz. Keep that in mind when deciding whether continuous mode is really necessary for your application.

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    adc, pinOrErr = esp32.adc(unit, channel, cfg)

</div>

</div>

<div id="required-continuous-mode-options" class="section">

### Required continuous-mode options<a href="#required-continuous-mode-options" class="headerlink" title="Link to this heading"></a>

When using continuous mode, set these options in <span class="pre">`cfg`</span>:

- <span class="pre">`callback`</span>: Lua function called when a processed sample block is ready.

- <span class="pre">`fs`</span>: Sampling frequency in Hz. The runtime adjusts values outside the supported range.

- <span class="pre">`bl`</span>: Block length. This controls how many samples are collected before the callback is invoked. Together with <span class="pre">`fs`</span>, this determines the callback rate.

- <span class="pre">`filter`</span>: One of <span class="pre">`"data"`</span> or <span class="pre">`"mean"`</span>.

</div>

<div id="filters" class="section">

### Filters<a href="#filters" class="headerlink" title="Link to this heading"></a>

<span class="pre">`"data"`</span>  
Returns the sampled block as a Lua string containing 16-bit little-endian PCM values.

<span class="pre">`"mean"`</span>  
Returns a single averaged value representing the current block of samples.

</div>

<div id="callback-signatures" class="section">

### Callback Signatures<a href="#callback-signatures" class="headerlink" title="Link to this heading"></a>

When using <span class="pre">`filter`</span>` `<span class="pre">`=`</span>` `<span class="pre">`"data"`</span>:

<div class="highlight-lua notranslate">

<div class="highlight">

    function mycallback(data, err)

</div>

</div>

- <span class="pre">`data`</span>: A PCM-encoded Lua string.

- <span class="pre">`err`</span>: An error string when <span class="pre">`data`</span> is <span class="pre">`nil`</span>. Occasional <span class="pre">`"overflow"`</span> events can happen under load.

When using <span class="pre">`filter`</span>` `<span class="pre">`=`</span>` `<span class="pre">`"mean"`</span>:

<div class="highlight-lua notranslate">

<div class="highlight">

    function mycallback(raw, voltOrErr)

</div>

</div>

- <span class="pre">`raw`</span>: The averaged raw ADC value.

- <span class="pre">`voltOrErr`</span>: The converted millivolt value when <span class="pre">`volt=true`</span> was enabled. If <span class="pre">`raw`</span> is <span class="pre">`nil`</span>, this argument contains the error.

</div>

<div id="continuous-example-mean-filter" class="section">

### Continuous Example: <span class="pre">`mean`</span> Filter<a href="#continuous-example-mean-filter" class="headerlink" title="Link to this heading"></a>

This example lets the runtime auto-adjust both <span class="pre">`fs`</span> and <span class="pre">`bl`</span> by setting them to zero. On ESP32 hardware, the values typically settle at <span class="pre">`20000`</span> and <span class="pre">`200`</span> respectively, which results in a fast but still manageable callback rate.

<div class="highlight-lua notranslate">

<div class="highlight">

    local cnt = 0

    local function callback(raw, volt)
       if cnt % 100 == 0 then
          trace(raw, volt)
       end
       cnt = cnt + 1
    end

    local adc, pin = esp32.adc(1, 0, {
       volt = true,
       bl = 0,
       callback = callback,
       fs = 0,
       filter = "mean"
    })

</div>

</div>

</div>

<div id="continuous-example-data-filter" class="section">

### Continuous Example: <span class="pre">`data`</span> Filter<a href="#continuous-example-data-filter" class="headerlink" title="Link to this heading"></a>

This example asks for the highest sample rate and lets the block length be auto-adjusted. The callback prints the size of the returned binary block so you can verify that data is flowing.

<div class="highlight-lua notranslate">

<div class="highlight">

    local cnt = 0

    local function callback(data, err)
       if data then
          if cnt % 100 == 0 then
             trace(#data)
          end
          cnt = cnt + 1
       else
          trace(err)
       end
    end

    local adc, pin = esp32.adc(1, 0, {
       volt = true,
       bl = 0,
       callback = callback,
       fs = 48000,
       filter = "data"
    })

</div>

</div>

</div>

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Use one-shot mode when you only need occasional readings.

- Use continuous mode when timing matters more than simplicity.

- Prefer the <span class="pre">`mean`</span> filter when you want a stable interpreted value.

- Prefer the <span class="pre">`data`</span> filter when you want to process raw sampled blocks in Lua or pass them into another decoding step.

- For slowly changing sensors such as light sensors, potentiometers, and soil moisture modules, a timer plus one-shot reads is usually easier to maintain than continuous mode.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="xedge.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Xedge32"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="cam.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Camera API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/AccessPointMode.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="#" class="current reference internal">Access Point Mode</a>
  - <a href="#connecting-to-xedge32" class="reference internal">Connecting to Xedge32</a>
  - <a href="#access-point-mode-considerations" class="reference internal">Access Point Mode Considerations</a>
  - <a href="#switching-to-station-mode" class="reference internal">Switching to Station Mode</a>
  - <a href="#reconnecting-after-the-mode-change" class="reference internal">Reconnecting After the Mode Change</a>
  - <a href="#station-mode-considerations" class="reference internal">Station Mode Considerations</a>
  - <a href="#switching-back-to-access-point-mode" class="reference internal">Switching Back to Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Access Point Mode
- <a href="../_sources/source/AccessPointMode.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="access-point-mode" class="section">


# Access Point Mode<a href="#access-point-mode" class="headerlink" title="Link to this heading"></a>

After you <a href="GettingStarted.html#flashing-the-firmware" class="reference internal"><span class="std std-ref">flash the firmware</span></a> for the first time, Xedge32 starts in **Access Point Mode**. In this mode, the ESP32 creates its own Wi-Fi network so you can connect to the device even before it has been configured for your local network.

The default access point name is <span class="pre">`xedge32`</span>. This startup mode is especially useful during first-time setup, lab work, and field service, since it gives you an immediate path to the device without requiring a pre-existing network.

<div id="connecting-to-xedge32" class="section">

## Connecting to Xedge32<a href="#connecting-to-xedge32" class="headerlink" title="Link to this heading"></a>

Use the following sequence the first time you connect:

1.  Connect your phone or computer to the <span class="pre">`xedge32`</span> Wi-Fi network. The default password is <span class="pre">`12345678`</span>.

2.  Open a browser and navigate to <a href="http://xedge32.local" class="reference external">http://xedge32.local</a>.

3.  If your computer does not support mDNS, use the default AP address <span class="pre">`http://192.168.190.0`</span> instead.

4.  The first page you see is a default 404 page. Click the <span class="pre">`Xedge`</span>` `<span class="pre">`IDE`</span> link to open the <a href="xedge.html#xedge32" class="reference internal"><span class="std std-ref">Xedge IDE Web Editor</span></a>.

<div class="admonition note">

Note

The default SSID and password are intended for initial onboarding. If you plan to keep a device in Access Point Mode for extended development or field use, update the configuration to match your deployment requirements.

</div>

<div class="admonition note">

Note

Custom firmware builds can use a different AP address. If <span class="pre">`xedge32.local`</span> does not resolve and the default address does not respond, open the serial console and check the boot log for the printed AP IP address.

</div>

</div>

<div id="access-point-mode-considerations" class="section">

## Access Point Mode Considerations<a href="#access-point-mode-considerations" class="headerlink" title="Link to this heading"></a>

Access Point Mode is primarily a setup and recovery mode. It is fully usable, but there are a few details worth knowing:

- The built-in editor always works, but on computers without Internet access it falls back to a basic HTML text area instead of the richer Monaco-based editor.

- If you want the more advanced browser editor while still keeping the ESP32 in Access Point Mode, use a computer that is connected to the ESP32 over Wi-Fi and to the Internet over another interface such as Ethernet.

- This mode is also convenient when you are bringing up a device on a workbench and do not yet want it to join a production network.

<figure id="id2" class="align-default">
<img src="../_images/Xedge32-IDE-Access-Point-Mode.svg" alt="Xedge32 IDE Access Point Mode" />
<figcaption><p><span class="caption-text">Figure 1: Loading the advanced editor while the ESP32 remains in Access Point Mode.</span><a href="#id2" class="headerlink" title="Link to this image"></a></p></figcaption>
</figure>

</div>

<div id="switching-to-station-mode" class="section">

## Switching to Station Mode<a href="#switching-to-station-mode" class="headerlink" title="Link to this heading"></a>

**Station Mode** is the recommended operating mode for most projects because it lets Xedge32 join your normal network and use the full set of IoT-oriented features.

To switch from Access Point Mode to Station Mode:

1.  Open the web interface and click the three dots (<span class="pre">`...`</span>) in the upper-right corner.

2.  Select **Lua Shell** to open <a href="LuaShell32.html#luashell32" class="reference internal"><span class="std std-ref">LuaShell32</span></a>.

3.  Run the following command:

    <div class="highlight-lua notranslate">

    <div class="highlight">

        esp32.netconnect("wifi", {ssid="your-Wi-Fi-SSID", pwd="password"})

    </div>

    </div>

4.  Replace <span class="pre">`your-Wi-Fi-SSID`</span> and <span class="pre">`password`</span> with your own credentials.

5.  Wait while the ESP32 attempts to join the network.

If the connection succeeds, the device stays in Station Mode. If the connection attempt fails during initial configuration, Xedge32 falls back to Access Point Mode so you can correct the settings and try again.

</div>

<div id="reconnecting-after-the-mode-change" class="section">

## Reconnecting After the Mode Change<a href="#reconnecting-after-the-mode-change" class="headerlink" title="Link to this heading"></a>

Once the ESP32 has joined your network:

- If your computer supports mDNS, reconnect by browsing to <span class="pre">`http://xedge32.local/`</span>.

- If your computer does not support mDNS, find the new IP address in your router’s DHCP client list. The device typically appears as <span class="pre">`xedge`</span>.

<div class="admonition note">

Note

If you were already connected to the IDE through <span class="pre">`http://xedge32.local/`</span> and both your computer and the ESP32 can still reach each other after the mode change, the IDE may reconnect automatically. It is normal to briefly see disconnect and reconnect messages in the IDE console before the final <span class="pre">`connected`</span> message appears.

</div>

</div>

<div id="station-mode-considerations" class="section">

## Station Mode Considerations<a href="#station-mode-considerations" class="headerlink" title="Link to this heading"></a>

Once your device runs in Station Mode, one of the following connection methods is usually the most convenient:

- <span class="pre">`http://xedge32.local/`</span> if you are using the precompiled firmware or a custom build with mDNS enabled.

- The DHCP-assigned IP address if you prefer direct IP access.

- A DHCP reservation on your router if you want the same IP address every time.

- A permanent public URL by enabling the SharkTrust Let’s Encrypt plugin from <a href="https://realtimelogic.com/ba/doc/?url=Xedge.html#cert" class="reference external">Xedge’s configuration menu</a>.

Additional notes:

- You can change the mDNS name by using <a href="misc.html#esp32-execute-label" class="reference internal"><span class="std std-ref">esp32.execute(command)</span></a>.

- mDNS is convenient, but it may resolve more slowly than normal DNS on some networks.

- Bookmarking a stable IP address or using a DHCP reservation is often the most practical choice for long-term development.

</div>

<div id="switching-back-to-access-point-mode" class="section">

## Switching Back to Access Point Mode<a href="#switching-back-to-access-point-mode" class="headerlink" title="Link to this heading"></a>

After the ESP32 has successfully connected in Station Mode at least once, it does not automatically revert to Access Point Mode if later network attempts fail. This behavior is intentional and helps prevent accidental exposure of the device as an access point.

To switch back manually, run the following command in <a href="LuaShell32.html#luashell32" class="reference internal"><span class="std std-ref">LuaShell32</span></a>:

<div class="highlight-lua notranslate">

<div class="highlight">

    esp32.netconnect"wifi"

</div>

</div>

This disconnects the device from Wi-Fi Station Mode and restores the original Access Point Mode behavior.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="GettingStarted.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Getting Started"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="LuaShell32.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="LuaShell32">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/BME280.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="#" class="current reference internal">BME280 Module</a>
  - <a href="#typical-workflow" class="reference internal">Typical Workflow</a>
  - <a href="#creating-a-bme280-object" class="reference internal">Creating a BME280 Object</a>
    - <a href="#parameters" class="reference internal">Parameters</a>
    - <a href="#return-value" class="reference internal">Return Value</a>
  - <a href="#bme280-object-methods" class="reference internal">BME280 Object Methods</a>
    - <a href="#bme-read" class="reference internal"><span class="pre"><code class="docutils literal notranslate">bme:read()</code></span></a>
  - <a href="#basic-example" class="reference internal">Basic Example</a>
  - <a href="#practical-notes" class="reference internal">Practical Notes</a>
  - <a href="#online-examples" class="reference internal">Online Examples</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- BME280 Module
- <a href="../_sources/source/BME280.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="bme280-module" class="section">


# BME280 Module<a href="#bme280-module" class="headerlink" title="Link to this heading"></a>

This Lua module provides a simple, high-level interface to the Bosch BME280 sensor family. It is a convenient starting point if you want to read temperature, humidity, and pressure from Lua without writing low-level I2C register code yourself.

The module communicates with the sensor through the <a href="I2C.html#i2c-api" class="reference internal"><span class="std std-ref">I2C API</span></a>. If you want to understand the lower-level interaction, see the <a href="https://github.com/RealTimeLogic/Xedge-ESP32/blob/master/Lua-Examples/.lua/bme280.lua" class="reference external">BME280 Lua source code</a>.

<div class="admonition note">

Note

The module is currently included in the Xedge32 firmware, but in future versions it may be shipped separately.

</div>

<div id="typical-workflow" class="section">

## Typical Workflow<a href="#typical-workflow" class="headerlink" title="Link to this heading"></a>

Using the module normally looks like this:

1.  Create a BME280 object.

2.  Call <span class="pre">`bme:read()`</span> whenever you want a new sensor reading.

3.  Reuse the object for repeated measurements.

For a first test, power the breakout from the voltage recommended by the breakout board vendor, connect GND to ESP32 GND, and verify the I2C address with the <a href="I2C.html#i2c-api" class="reference internal"><span class="std std-ref">I2C API</span></a> scanner before debugging the BME280 module itself.

</div>

<div id="creating-a-bme280-object" class="section">

## Creating a BME280 Object<a href="#creating-a-bme280-object" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    bme = require"bme280".create(port, address, sda, scl [, settings])

</div>

</div>

<div id="parameters" class="section">

### Parameters<a href="#parameters" class="headerlink" title="Link to this heading"></a>

- <span class="pre">`port`</span>: I2C port number, for example <span class="pre">`0`</span>.

- <span class="pre">`address`</span>: I2C address of the sensor, typically <span class="pre">`0x76`</span>.

- <span class="pre">`sda`</span>: GPIO used for I2C data.

- <span class="pre">`scl`</span>: GPIO used for I2C clock.

- <span class="pre">`settings`</span>: Optional table with driver-specific configuration values.

The optional <span class="pre">`settings`</span> table is useful when you want to match a particular oversampling or measurement setup. See the <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/Sparkplug#ready-to-run-sparkplug-enabled-weather-station-example" class="reference external">Weather Station demo</a> for a more advanced example.

</div>

<div id="return-value" class="section">

### Return Value<a href="#return-value" class="headerlink" title="Link to this heading"></a>

The function returns a new BME280 object.

</div>

</div>

<div id="bme280-object-methods" class="section">

## BME280 Object Methods<a href="#bme280-object-methods" class="headerlink" title="Link to this heading"></a>

<div id="bme-read" class="section">

### <span class="pre">`bme:read()`</span><a href="#bme-read" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    local temperature, humidity, pressure = bme:read()

</div>

</div>

This method reads the current measurement values from the sensor.

Return values:

- <span class="pre">`temperature`</span> in degrees Celsius,

- <span class="pre">`humidity`</span> in percent relative humidity, and

- <span class="pre">`pressure`</span> in Pascals.

</div>

</div>

<div id="basic-example" class="section">

## Basic Example<a href="#basic-example" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    local bmeModule = require"bme280"
    local port = 0
    local address = 0x76
    local sda = 5
    local scl = 6

    local bme = bmeModule.create(port, address, sda, scl)
    local temp, hum, pres = bme:read()

    trace("Temperature:", temp)
    trace("Humidity:", hum)
    trace("Pressure:", pres)

</div>

</div>

This example creates the sensor object once, reads one sample, and stores the three returned values in local variables.

</div>

<div id="practical-notes" class="section">

## Practical Notes<a href="#practical-notes" class="headerlink" title="Link to this heading"></a>

- The BME280 and BMP280 are closely related devices, but only BME280 sensors provide humidity readings.

- If communication fails, first verify the I2C address and the selected SDA/SCL pins.

- Many breakout boards use <span class="pre">`0x76`</span> or <span class="pre">`0x77`</span> depending on how the address pin is wired.

- Some boards are BME280-compatible at the connector but actually contain a BMP280. A BMP280 does not report humidity.

</div>

<div id="online-examples" class="section">

## Online Examples<a href="#online-examples" class="headerlink" title="Link to this heading"></a>

- <a href="https://github.com/RealTimeLogic/LSP-Examples/blob/master/ESP32/bme280.xlua" class="reference external">bme280.xlua</a>

- <a href="https://github.com/surfskidude/weather-station" class="reference external">Weather Station</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="misc.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Miscellaneous API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="RTU.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Modbus RTU Module">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/GPIO.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="#" class="current reference internal">GPIO API</a>
  - <a href="#creating-a-gpio-object" class="reference internal">Creating a GPIO Object</a>
    - <a href="#parameters" class="reference internal">Parameters</a>
  - <a href="#gpio-object-methods" class="reference internal">GPIO Object Methods</a>
    - <a href="#gpio-value-val" class="reference internal"><span class="pre"><code class="docutils literal notranslate">gpio:value([val])</code></span></a>
    - <a href="#gpio-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">gpio:close()</code></span></a>
  - <a href="#examples" class="reference internal">Examples</a>
    - <a href="#example-1-set-gpio-18-high" class="reference internal">Example 1: Set GPIO 18 High</a>
    - <a href="#example-2-toggle-an-led-through-garbage-collection" class="reference internal">Example 2: Toggle an LED Through Garbage Collection</a>
    - <a href="#example-3-read-a-button-state" class="reference internal">Example 3: Read a Button State</a>
    - <a href="#example-4-react-to-a-button-with-an-interrupt-callback" class="reference internal">Example 4: React to a Button with an Interrupt Callback</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- GPIO API
- <a href="../_sources/source/GPIO.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="gpio-api" class="section">

# GPIO API<a href="#gpio-api" class="headerlink" title="Link to this heading"></a>

The GPIO API provides access to the ESP32’s general-purpose input and output pins. This is the API you use to read buttons, drive LEDs, control enable pins, and react to digital edge transitions.

The API supports both simple polling and interrupt-driven operation. For interactive applications and responsive hardware handling, the callback-based approach is usually the better choice.

For simple maker projects, GPIO is often the first API to try. Use it for low-current logic signals, buttons, relays with proper driver circuits, and LED experiments with a current-limiting resistor. Do not connect motors, LED strips, or other high-current loads directly to an ESP32 GPIO pin.

<div id="creating-a-gpio-object" class="section">

## Creating a GPIO Object<a href="#creating-a-gpio-object" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    gpio = esp32.gpio(pin, mode [, cfg])

</div>

</div>

<div id="parameters" class="section">

### Parameters<a href="#parameters" class="headerlink" title="Link to this heading"></a>

- <span class="pre">`pin`</span>: GPIO pin number.

- <span class="pre">`mode`</span>: One of the following strings:

  - <span class="pre">`"IN"`</span>: Input only.

  - <span class="pre">`"OUT"`</span>: Output only.

  - <span class="pre">`"OUTOD"`</span>: Output only with open-drain behavior.

  - <span class="pre">`"INOUTOD"`</span>: Input/output with open-drain behavior.

  - <span class="pre">`"INOUT"`</span>: Input/output.

- <span class="pre">`cfg`</span>: Optional configuration table.

Configuration options:

- <span class="pre">`pullup`</span>: Enable the internal pull-up resistor. Default is <span class="pre">`false`</span>.

- <span class="pre">`pulldown`</span>: Enable the internal pull-down resistor. Default is <span class="pre">`false`</span>.

- <span class="pre">`callback`</span>: Lua callback function used for interrupt-driven input handling.

- <span class="pre">`type`</span>: Interrupt trigger type when <span class="pre">`callback`</span> is set.

Supported interrupt types:

- <span class="pre">`"LOW"`</span>: Trigger while the input level is low.

- <span class="pre">`"HIGH"`</span>: Trigger while the input level is high.

- <span class="pre">`"POSEDGE"`</span>: Trigger on rising edge.

- <span class="pre">`"NEGEDGE"`</span>: Trigger on falling edge.

- <span class="pre">`"ANYEDGE"`</span>: Trigger on both rising and falling edges.

If a callback is provided and <span class="pre">`type`</span> is omitted, the default interrupt type is <span class="pre">`"POSEDGE"`</span>.

Use edge triggers for buttons and pulse inputs when you want to react to a transition. Use level triggers only when the application really needs to know that a pin remains high or low, since a level condition can produce repeated interrupt activity while the level remains active.

</div>

</div>

<div id="gpio-object-methods" class="section">

## GPIO Object Methods<a href="#gpio-object-methods" class="headerlink" title="Link to this heading"></a>

<div id="gpio-value-val" class="section">

### <span class="pre">`gpio:value([val])`</span><a href="#gpio-value-val" class="headerlink" title="Link to this heading"></a>

If <span class="pre">`val`</span> is provided, this method writes the pin level:

- <span class="pre">`true`</span> for high

- <span class="pre">`false`</span> for low

If <span class="pre">`val`</span> is omitted, the method returns the current GPIO level.

On boards where an LED is wired as active-low, <span class="pre">`false`</span> turns the LED on and <span class="pre">`true`</span> turns it off. Check the schematic or board documentation if the output appears inverted.

</div>

<div id="gpio-close" class="section">

### <span class="pre">`gpio:close()`</span><a href="#gpio-close" class="headerlink" title="Link to this heading"></a>

Releases the GPIO object and frees the underlying hardware resources.

</div>

</div>

<div id="examples" class="section">


## Examples<a href="#examples" class="headerlink" title="Link to this heading"></a>

The following examples are designed to run well as Lua Server Pages (LSP), but the same GPIO patterns also work in regular Lua application code.

<div id="example-1-set-gpio-18-high" class="section">

### Example 1: Set GPIO 18 High<a href="#example-1-set-gpio-18-high" class="headerlink" title="Link to this heading"></a>

This example opens GPIO 18 as an output, drives it high, waits two seconds, and then automatically releases the GPIO object when it goes out of scope.

<div class="highlight-lua notranslate">

<div class="highlight">

    local pin <close> = esp32.gpio(18, "OUT")
    pin:value(true)
    ba.sleep(2000)

</div>

</div>

The <span class="pre">`<close>`</span> syntax is useful when you want the resource to be released automatically at the end of the current scope.

</div>

<div id="example-2-toggle-an-led-through-garbage-collection" class="section">

### Example 2: Toggle an LED Through Garbage Collection<a href="#example-2-toggle-an-led-through-garbage-collection" class="headerlink" title="Link to this heading"></a>

This example demonstrates an Xedge32 pattern that is especially convenient during hot reloading. When the page is refreshed, the previous GPIO object becomes unreachable and can be garbage-collected, which releases the pin.

<div class="highlight-lua notranslate">

<div class="highlight">

    collectgarbage()
    ba.sleep(2000)
    local pin = esp32.gpio(18, "OUT")
    pin:value(true)

</div>

</div>

If an LED is connected to GPIO 18, it turns off briefly while the previous object is collected, then turns on again when the new object is created.

</div>

<div id="example-3-read-a-button-state" class="section">

### Example 3: Read a Button State<a href="#example-3-read-a-button-state" class="headerlink" title="Link to this heading"></a>

This example configures GPIO 15 as an input with a pull-down resistor and polls the pin once per second for up to 30 seconds.

<div class="highlight-lua notranslate">

<div class="highlight">

    local pin <close> = esp32.gpio(15, "IN", {pulldown = true})
    for i = 1, 30 do
       local val = pin:value()
       trace(i, val)
       if val then
          break
       end
       ba.sleep(1000)
    end

</div>

</div>

</div>

<div id="example-4-react-to-a-button-with-an-interrupt-callback" class="section">

### Example 4: React to a Button with an Interrupt Callback<a href="#example-4-react-to-a-button-with-an-interrupt-callback" class="headerlink" title="Link to this heading"></a>

This example listens for both rising and falling edges on GPIO 15. It is a better pattern than polling when you want fast response and less idle CPU work.

<div class="highlight-lua notranslate">

<div class="highlight">

    local cfg = {
       pulldown = true,
       type = "ANYEDGE",
       callback = function(level)
          trace("level", level)
       end
    }

    trace(esp32.gpio(15, "IN", cfg))

</div>

</div>

</div>

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Use polling when you only need occasional state checks.

- Use callbacks for buttons, pulse inputs, and other event-driven signals.

- Confirm the board pinout before using a GPIO number. Some ESP32 pins are reserved for flash, PSRAM, USB, boot mode selection, camera, or SD card hardware.

- Prefer explicit cleanup with <span class="pre">`gpio:close()`</span> in long-running applications.

- In short-lived LSP pages, Lua’s automatic cleanup can simplify iteration.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="cam.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Camera API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="I2C.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="I2C API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/GettingStarted.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="#" class="current reference internal">Getting Started</a>
  - <a href="#first-time-installers" class="reference internal">First-Time Installers</a>
  - <a href="#ota-upgrade" class="reference internal">OTA Upgrade</a>
  - <a href="#advanced-installation-and-upgrade" class="reference internal">Advanced Installation and Upgrade</a>
    - <a href="#firmware-option-1-use-precompiled-firmware" class="reference internal">Firmware Option 1: Use Precompiled Firmware</a>
      - <a href="#windows-graphical-installer" class="reference internal">Windows Graphical Installer</a>
      - <a href="#how-to-flash-the-xedge32-firmware" class="reference internal">How to Flash the Xedge32 Firmware</a>
      - <a href="#common-flashing-issues" class="reference internal">Common Flashing Issues</a>
      - <a href="#linux-macos-and-windows-with-esptool" class="reference internal">Linux, macOS, and Windows with <span class="pre"><code class="docutils literal notranslate">esptool</code></span></a>
      - <a href="#esp32-s3-firmware-options" class="reference internal">ESP32-S3 Firmware Options</a>
    - <a href="#firmware-option-2-compile-the-code" class="reference internal">Firmware Option 2: Compile the Code</a>
  - <a href="#configure-the-esp32" class="reference internal">Configure the ESP32</a>
    - <a href="#using-wi-fi" class="reference internal">Using Wi-Fi</a>
    - <a href="#using-ethernet" class="reference internal">Using Ethernet</a>
  - <a href="#next-step" class="reference internal">Next Step</a>
  - <a href="#firmware-upgrade-options" class="reference internal">Firmware Upgrade Options</a>
  - <a href="#support-and-discussions" class="reference internal">Support and Discussions</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Getting Started
- <a href="../_sources/source/GettingStarted.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="getting-started" class="section">

# Getting Started<a href="#getting-started" class="headerlink" title="Link to this heading"></a>

To start using Xedge32, you first need to install the firmware on a supported ESP32 board. The precompiled firmware targets:

- **Standard ESP32 boards with PSRAM**, such as ESP32 WROVER, with at least 4 MB flash and 4 MB RAM.

- **ESP32-S3 boards**, with at least 8 MB flash and 8 MB RAM.

If you are completely new to Xedge32, start with the precompiled firmware and the default onboarding workflow. More advanced build and upgrade paths are included later on this page.

Before flashing your first board, it can also help to read <a href="https://realtimelogic.com/articles/How-Xedge32-Xedge-and-Barracuda-App-Server-Work-Together" class="reference external">How Xedge32, Xedge, and Barracuda App Server Work Together</a>. That article explains which parts are generic Xedge/BAS features and which parts are specific to ESP32 hardware.

<div id="first-time-installers" class="section">

## First-Time Installers<a href="#first-time-installers" class="headerlink" title="Link to this heading"></a>

For the easiest first installation, use the web installer on the <a href="https://realtimelogic.com/downloads/bas/ESP32/#install" class="reference external">Xedge32 installation page</a>. That installer is the quickest route to a working device because it avoids manual flash addresses and command-line tools.

If you need more control over flashing, board selection, or firmware variants, continue with the manual instructions below.

</div>

<div id="ota-upgrade" class="section">

## OTA Upgrade<a href="#ota-upgrade" class="headerlink" title="Link to this heading"></a>

If Xedge32 is already installed on an ESP32-S3, you can upgrade it with the <a href="https://youtu.be/GHBcjeO-Yhc" class="reference external">integrated Xedge32 OTA Upgrade Manager</a>.

Typical OTA workflow:

1.  Download the latest <a href="https://realtimelogic.com/downloads/bas/Xedge32-S3-Firmware.zip" class="reference external">ESP32-S3 firmware ZIP file</a>.

2.  Extract the ZIP file.

3.  Open the OTA Upgrade Manager.

4.  Drag either <span class="pre">`xedge.bin`</span> or <span class="pre">`xedge-s0.bin`</span> onto the upgrade page.

This upgrade path is especially convenient once a board is already deployed and reachable through the Xedge32 interface.

</div>

<div id="advanced-installation-and-upgrade" class="section">

## Advanced Installation and Upgrade<a href="#advanced-installation-and-upgrade" class="headerlink" title="Link to this heading"></a>

Two firmware delivery models are available:

1.  Precompiled binary files that you can flash directly to the board.

2.  Source code that you can build yourself with the Espressif ESP-IDF toolchain.

<div class="admonition important">

Important

The instructions on this page cover both ESP32 and ESP32-S3 hardware. Make sure you follow the command sequence and firmware layout that matches your specific chip.

</div>

Page contents

- <a href="#firmware-option-1-use-precompiled-firmware" id="id1" class="reference internal">Firmware Option 1: Use Precompiled Firmware</a>

  - <a href="#windows-graphical-installer" id="id2" class="reference internal">Windows Graphical Installer</a>

  - <a href="#how-to-flash-the-xedge32-firmware" id="id3" class="reference internal">How to Flash the Xedge32 Firmware</a>

  - <a href="#common-flashing-issues" id="id4" class="reference internal">Common Flashing Issues</a>

  - <a href="#linux-macos-and-windows-with-esptool" id="id5" class="reference internal">Linux, macOS, and Windows with <span class="pre"><code class="docutils literal notranslate">esptool</code></span></a>

  - <a href="#esp32-s3-firmware-options" id="id6" class="reference internal">ESP32-S3 Firmware Options</a>

- <a href="#firmware-option-2-compile-the-code" id="id7" class="reference internal">Firmware Option 2: Compile the Code</a>

<div id="firmware-option-1-use-precompiled-firmware" class="section">


### <a href="#id1" class="toc-backref" role="doc-backlink">Firmware Option 1: Use Precompiled Firmware</a><a href="#firmware-option-1-use-precompiled-firmware" class="headerlink" title="Link to this heading"></a>

The precompiled firmware can be flashed from Windows, macOS, or Linux. We begin with the Windows graphical workflow because it is the most approachable for many first-time users, then show the <span class="pre">`esptool`</span> command-line workflow.

<div id="windows-graphical-installer" class="section">

#### <a href="#id2" class="toc-backref" role="doc-backlink">Windows Graphical Installer</a><a href="#windows-graphical-installer" class="headerlink" title="Link to this heading"></a>

<div style="position: relative;width: 100%;height: 0;padding-bottom: 56.25%;">

<div class="iframe">

<div id="player">

</div>

<div class="player-unavailable">

# An error occurred.

<div class="submessage">

Unable to execute JavaScript.

</div>

</div>

</div>

</div>

</div>

<div id="how-to-flash-the-xedge32-firmware" class="section">

#### <a href="#id3" class="toc-backref" role="doc-backlink">How to Flash the Xedge32 Firmware</a><a href="#how-to-flash-the-xedge32-firmware" class="headerlink" title="Link to this heading"></a>

Download the <a href="https://www.espressif.com/en/support/download/other-tools" class="reference external">ESPRESSIF Flash Tool</a> and one of the following firmware packages:

- <a href="https://realtimelogic.com/downloads/bas/Xedge32-Firmware.zip" class="reference external">ESP32 Xedge firmware</a>

- <a href="https://realtimelogic.com/downloads/bas/Xedge32-S3-Firmware.zip" class="reference external">ESP32-S3 Xedge firmware</a> which includes two firmware variants. See <a href="#firmware-options" class="reference internal"><span class="std std-ref">ESP32-S3 Firmware Options</span></a>.

After downloading, extract the flash tool archive and the firmware ZIP file.

Then follow these steps:

1.  Connect the ESP32 board to your computer.

2.  Identify the COM port in Windows Device Manager.

3.  Start <span class="pre">`flash_download_tool_3.9.4.exe`</span>.

4.  Select the correct chip type, <span class="pre">`ESP32`</span> or <span class="pre">`ESP32-S3`</span>, and click <span class="guilabel">OK</span>.

5.  On the <span class="pre">`SPIDownload`</span> page, set the correct COM port and set the speed to <span class="pre">`115200`</span>.

6.  Click <span class="guilabel">ERASE</span> and wait for the erase operation to complete.

7.  Use the browse buttons to select the firmware file or files.

8.  Apply one of the following flash layouts.

<div id="option-1-merged-firmware-file" class="section">

##### Option 1: Merged Firmware File<a href="#option-1-merged-firmware-file" class="headerlink" title="Link to this heading"></a>

Use this when flashing <span class="pre">`merged-xedge.bin`</span>:

| Binary File                                 | Address                        |
|---------------------------------------------|--------------------------------|
| <span class="pre">`merged-xedge.bin`</span> | <span class="pre">`0x0`</span> |

Select the checkbox to the left of the file entry after adding it.

</div>

<div id="option-2-separate-firmware-files" class="section">

##### Option 2: Separate Firmware Files<a href="#option-2-separate-firmware-files" class="headerlink" title="Link to this heading"></a>

Use this when flashing the individual binary components:

| Binary File                                    | ESP32 Address                      | ESP32-S3 Address                   |
|------------------------------------------------|------------------------------------|------------------------------------|
| <span class="pre">`bootloader.bin`</span>      | <span class="pre">`0x1000`</span>  | <span class="pre">`0x0`</span>     |
| <span class="pre">`partition_table.bin`</span> | <span class="pre">`0x8000`</span>  | <span class="pre">`0x8000`</span>  |
| <span class="pre">`xedge.bin`</span>           | <span class="pre">`0x10000`</span> | <span class="pre">`0x20000`</span> |

Enable all three checkboxes after selecting the files.

</div>

<div id="finish-the-flash-process" class="section">

##### Finish the Flash Process<a href="#finish-the-flash-process" class="headerlink" title="Link to this heading"></a>

9.  Leave the remaining tool settings unchanged.

10. Click <span class="guilabel">START</span> and wait for the upload to finish.

11. Open a serial terminal such as PuTTY.

12. Watch the boot log.

13. When <span class="pre">`LuaShell32`</span>` `<span class="pre">`ready`</span> appears, continue with <a href="#configesp32" class="reference internal"><span class="std std-ref">Configure the ESP32</span></a>.


The screenshot above shows the flash tool on the left and a terminal window on the right. The merged firmware option is simpler because it packages the required firmware parts into a single file.

</div>

</div>

<div id="common-flashing-issues" class="section">

#### <a href="#id4" class="toc-backref" role="doc-backlink">Common Flashing Issues</a><a href="#common-flashing-issues" class="headerlink" title="Link to this heading"></a>

- Some boards require you to hold the boot button while connecting USB. Once the board is detected, you can release the button.

- On ESP32-S3 boards with both USB-OTG and USB-UART, use the USB-UART port for flashing. If needed, use the other port for the runtime console.

- If PuTTY does not show <a href="LuaShell32.html#luashell32" class="reference internal"><span class="std std-ref">LuaShell32</span></a>, prepare the serial terminal first, then press the board’s reset button and immediately open the terminal connection.

</div>

<div id="linux-macos-and-windows-with-esptool" class="section">

#### <a href="#id5" class="toc-backref" role="doc-backlink">Linux, macOS, and Windows with <span class="pre"><code class="docutils literal notranslate">esptool</code></span></a><a href="#linux-macos-and-windows-with-esptool" class="headerlink" title="Link to this heading"></a>

The following examples use Linux syntax, but the same <span class="pre">`esptool`</span> commands can be adapted for macOS and Windows.

Install the required tool:

<div class="highlight-sh notranslate">

<div class="highlight">

    sudo apt install python3-pip
    pip install esptool

</div>

</div>

Upload the **ESP32** firmware:

<div class="highlight-sh notranslate">

<div class="highlight">

    wget https://realtimelogic.com/downloads/bas/Xedge32-Firmware.zip
    unzip Xedge32-Firmware.zip
    cd Xedge32-Firmware

    # Use one of:

    # python -m esptool --chip esp32 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x0 merged-xedge.bin

    # python -m esptool --chip esp32 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 bootloader.bin 0x8000 partition-table.bin 0x10000 xedge.bin

</div>

</div>

Upload the **ESP32-S3** firmware:

<div class="highlight-sh notranslate">

<div class="highlight">

    wget https://realtimelogic.com/downloads/bas/Xedge32-S3-Firmware.zip
    unzip Xedge32-S3-Firmware.zip
    cd Xedge32-Firmware

    # Use one of:

    # python -m esptool --chip esp32s3 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size detect --flash_freq 80m 0x0 merged-xedge.bin

    # python -m esptool --chip esp32s3 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size detect --flash_freq 80m 0x0 bootloader.bin 0x8000 partition-table.bin 0x20000 xedge.bin

</div>

</div>

</div>

<div id="esp32-s3-firmware-options" class="section">


#### <a href="#id6" class="toc-backref" role="doc-backlink">ESP32-S3 Firmware Options</a><a href="#esp32-s3-firmware-options" class="headerlink" title="Link to this heading"></a>

<img src="../_images/esp-s3-usb.jpg" class="align-right" alt="../_images/esp-s3-usb.jpg" />

Two runtime variants are provided for ESP32-S3 boards:

**\`\`xedge.bin\`\` / \`\`merged-xedge.bin\`\`**  
Best for boards such as the XIAO ESP32-S3 that expose a single USB port. This build provides <a href="LuaShell32.html#luashell32" class="reference internal"><span class="std std-ref">LuaShell32</span></a> over that USB connection.

**\`\`xedge-s0.bin\`\` / \`\`merged-xedge-s0.bin\`\`**  
Best for boards that use UART0 for flashing or console access through a USB-to-UART bridge such as CP210x or FTDI.

<div class="admonition note">

Note

If you flash <span class="pre">`xedge.bin`</span> over a USB-to-UART adapter on a two-port board, you may need to move to the other USB port after flashing in order to access the runtime console. If you use <span class="pre">`xedge-s0.bin`</span>, you can usually keep using the same USB-to-UART connection.

</div>

</div>

</div>

<div id="firmware-option-2-compile-the-code" class="section">

### <a href="#id7" class="toc-backref" role="doc-backlink">Firmware Option 2: Compile the Code</a><a href="#firmware-option-2-compile-the-code" class="headerlink" title="Link to this heading"></a>

If you need to customize the firmware itself, use the source tree and build instructions in the <a href="https://github.com/RealTimeLogic/Xedge-ESP32" class="reference external">GitHub repository</a>.

Compiling the firmware is the right approach when you need custom C extensions, build-time configuration changes, or tighter control over the final image.

</div>

</div>

<div id="configure-the-esp32" class="section">


## Configure the ESP32<a href="#configure-the-esp32" class="headerlink" title="Link to this heading"></a>

After flashing, reboot the ESP32. On first boot, Xedge32 starts in Access Point Mode. From there, you can either connect over serial or connect to the device’s Wi-Fi access point and use the web-based shell.

Relevant guides:

- <a href="LuaShell32.html#luashell32" class="reference internal"><span class="std std-ref">LuaShell32</span></a> for serial or web-shell access

- <a href="AccessPointMode.html#access-point-mode" class="reference internal"><span class="std std-ref">Access Point Mode</span></a> for browser-based onboarding

If you want the device to join your normal network instead of staying in Access Point Mode, open <a href="LuaShell32.html#luashell32" class="reference internal"><span class="std std-ref">LuaShell32</span></a> and run one of the following commands.

<div id="using-wi-fi" class="section">

### Using Wi-Fi<a href="#using-wi-fi" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    esp32.netconnect("wifi", {ssid="your-Wi-Fi-SSID", pwd="password"})

</div>

</div>

</div>

<div id="using-ethernet" class="section">

### Using Ethernet<a href="#using-ethernet" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    esp32.netconnect("W5500", {spi-settings})

</div>

</div>

On a successful connection, the configuration is stored so the device can reconnect automatically after the next restart.

</div>

</div>

<div id="next-step" class="section">

## Next Step<a href="#next-step" class="headerlink" title="Link to this heading"></a>

Once Xedge32 is connected to your network, continue with <a href="xedge.html#xedge32" class="reference internal"><span class="std std-ref">Xedge32</span></a> to learn how to upload examples, create applications, and work with the IDE.

</div>

<div id="firmware-upgrade-options" class="section">

## Firmware Upgrade Options<a href="#firmware-upgrade-options" class="headerlink" title="Link to this heading"></a>

For ESP32-S3 boards, ongoing upgrades are usually simpler than the initial install. You can either:

1.  Repeat the firmware flashing procedure and upload only <span class="pre">`xedge.bin`</span> or <span class="pre">`xedge-s0.bin`</span>.

2.  Use the drag-and-drop OTA workflow described earlier on this page.

</div>

<div id="support-and-discussions" class="section">

## Support and Discussions<a href="#support-and-discussions" class="headerlink" title="Link to this heading"></a>

If you run into issues, want to compare notes with other users, or need help choosing a board or workflow, visit the project’s discussion forum:

<a href="https://github.com/RealTimeLogic/Xedge32/discussions" class="reference external">Xedge32 Discussions on GitHub</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="../index.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Xedge32 Introduction"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="AccessPointMode.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Access Point Mode">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/I2C.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="#" class="current reference internal">I2C API</a>
  - <a href="#creating-an-i2c-master" class="reference internal">Creating an I2C Master</a>
    - <a href="#parameters" class="reference internal">Parameters</a>
    - <a href="#return-value" class="reference internal">Return Value</a>
  - <a href="#i2c-master-object-methods" class="reference internal">I2C Master Object Methods</a>
    - <a href="#i2cm-probe-address-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:probe(address</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">timeout])</code></span></a>
    - <a href="#i2cm-read-address-len-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:read(address,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">len</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">timeout])</code></span></a>
    - <a href="#i2cm-readfrom-address-registerorbytes-len-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:readfrom(address,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">registerOrBytes,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">len</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">timeout])</code></span></a>
    - <a href="#i2cm-write-address-data-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:write(address,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">data</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">timeout])</code></span></a>
    - <a href="#i2cm-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">i2cm:close()</code></span></a>
  - <a href="#example" class="reference internal">Example</a>
  - <a href="#simple-address-scanner" class="reference internal">Simple Address Scanner</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- I2C API
- <a href="../_sources/source/I2C.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="i2c-api" class="section">


# I2C API<a href="#i2c-api" class="headerlink" title="Link to this heading"></a>

The I2C API lets Xedge32 communicate with sensors, displays, EEPROMs, and other peripherals that use the I2C bus. It exposes a straightforward master-side API that is well suited for both direct register access and small driver modules written in Lua.

The typical workflow is:

1.  Create an I2C master object.

2.  Probe the device address if needed.

3.  Read from or write to the target device.

4.  Close the bus object when you are done.

I2C is a shared two-wire bus. Many sensors can use the same SDA and SCL pins as long as each device has a unique address. Most small breakout boards already include pull-up resistors, but bare sensors often do not. If the bus behaves unreliably, check power, ground, address selection, and pull-ups before changing the Lua code.

<div id="creating-an-i2c-master" class="section">

## Creating an I2C Master<a href="#creating-an-i2c-master" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    i2cm = esp32.i2cmaster(port, pinSDA, pinSCL, speed)

</div>

</div>

<div id="parameters" class="section">

### Parameters<a href="#parameters" class="headerlink" title="Link to this heading"></a>

- <span class="pre">`port`</span>: I2C controller number, for example <span class="pre">`0`</span>.

- <span class="pre">`pinSDA`</span>: GPIO pin used for the SDA line.

- <span class="pre">`pinSCL`</span>: GPIO pin used for the SCL line.

- <span class="pre">`speed`</span>: Bus speed in Hertz.

Use the 7-bit I2C address shown in the device data sheet, for example <span class="pre">`0x76`</span>. Do not left-shift the address; Xedge32 passes the address to the ESP-IDF driver in normal 7-bit form.

</div>

<div id="return-value" class="section">

### Return Value<a href="#return-value" class="headerlink" title="Link to this heading"></a>

The function returns an I2C master object.

</div>

</div>

<div id="i2c-master-object-methods" class="section">

## I2C Master Object Methods<a href="#i2c-master-object-methods" class="headerlink" title="Link to this heading"></a>

<div class="admonition note">

Note

Some operations may block for up to the specified timeout. If predictable responsiveness matters, consider running longer I2C transactions in a separate thread or on a dedicated LSP page. See the <a href="https://realtimelogic.com/ba/doc/en/lua/auxlua.html#thread_lib" class="reference external">Lua thread library documentation</a>.

</div>

<div id="i2cm-probe-address-timeout" class="section">

### <span class="pre">`i2cm:probe(address`</span>` `<span class="pre">`[,`</span>` `<span class="pre">`timeout])`</span><a href="#i2cm-probe-address-timeout" class="headerlink" title="Link to this heading"></a>

Checks whether a device responds at the given address.

Parameters:

- <span class="pre">`address`</span>: I2C device address.

- <span class="pre">`timeout`</span>: Optional timeout in milliseconds. Default is <span class="pre">`500`</span>.

Returns <span class="pre">`true`</span> if the device responds. Otherwise returns <span class="pre">`nil,`</span>` `<span class="pre">`error`</span>.

</div>

<div id="i2cm-read-address-len-timeout" class="section">

### <span class="pre">`i2cm:read(address,`</span>` `<span class="pre">`len`</span>` `<span class="pre">`[,`</span>` `<span class="pre">`timeout])`</span><a href="#i2cm-read-address-len-timeout" class="headerlink" title="Link to this heading"></a>

Reads raw data directly from the device without first selecting a register.

Parameters:

- <span class="pre">`address`</span>: I2C device address.

- <span class="pre">`len`</span>: Number of bytes to read.

- <span class="pre">`timeout`</span>: Optional timeout in milliseconds. Default is <span class="pre">`500`</span>.

Returns the data as a Lua string on success, otherwise <span class="pre">`nil,`</span>` `<span class="pre">`error`</span>.

</div>

<div id="i2cm-readfrom-address-registerorbytes-len-timeout" class="section">

### <span class="pre">`i2cm:readfrom(address,`</span>` `<span class="pre">`registerOrBytes,`</span>` `<span class="pre">`len`</span>` `<span class="pre">`[,`</span>` `<span class="pre">`timeout])`</span><a href="#i2cm-readfrom-address-registerorbytes-len-timeout" class="headerlink" title="Link to this heading"></a>

Reads data from a specific device register. This method performs the common write-then-read transaction without issuing a stop condition between the two phases, which results in a repeated start.

Parameters:

- <span class="pre">`address`</span>: I2C device address.

- <span class="pre">`registerOrBytes`</span>: Register selector to write before reading. This can be a single numeric byte such as <span class="pre">`0xF7`</span> or a Lua string when the device needs a multi-byte register address.

- <span class="pre">`len`</span>: Number of bytes to read.

- <span class="pre">`timeout`</span>: Optional timeout in milliseconds. Default is <span class="pre">`500`</span>.

Returns the data as a Lua string on success, otherwise <span class="pre">`nil,`</span>` `<span class="pre">`error`</span>.

</div>

<div id="i2cm-write-address-data-timeout" class="section">

### <span class="pre">`i2cm:write(address,`</span>` `<span class="pre">`data`</span>` `<span class="pre">`[,`</span>` `<span class="pre">`timeout])`</span><a href="#i2cm-write-address-data-timeout" class="headerlink" title="Link to this heading"></a>

Writes data to the target device.

Parameters:

- <span class="pre">`address`</span>: I2C device address.

- <span class="pre">`data`</span>: Either a Lua string or a single byte value.

- <span class="pre">`timeout`</span>: Optional timeout in milliseconds. Default is <span class="pre">`500`</span>.

Returns <span class="pre">`true`</span> on success, otherwise <span class="pre">`nil,`</span>` `<span class="pre">`error`</span>.

</div>

<div id="i2cm-close" class="section">

### <span class="pre">`i2cm:close()`</span><a href="#i2cm-close" class="headerlink" title="Link to this heading"></a>

Closes the I2C master object and releases the associated resources.

Returns <span class="pre">`true`</span> on success, otherwise <span class="pre">`nil,`</span>` `<span class="pre">`error`</span>.

</div>

</div>

<div id="example" class="section">

## Example<a href="#example" class="headerlink" title="Link to this heading"></a>

The example below demonstrates the same style of operations typically used in a <a href="BME280.html#bme280-module" class="reference internal"><span class="std std-ref">BME280 Module</span></a> driver: probing the device, writing a configuration register, reading from a specific register, and then performing a plain read.

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Initialize the I2C master
    local i2cm = esp32.i2cmaster(0, 21, 22, 400000)

    -- Probe the device at address 0x76
    local found = i2cm:probe(0x76)
    if found then
       print("Device found at address 0x76")
    else
       print("Device not found")
    end

    -- Write a value to a register
    i2cm:write(0x76, "\xF4\x27")

    -- Read multiple bytes from a specific register
    local data = i2cm:readfrom(0x76, 0xF7, 8)
    print("Data read from register:", data)

    -- Perform a simple read without specifying a register
    local simple_data = i2cm:read(0x76, 4)
    print("Simple data read:", simple_data)

    -- Close the I2C connection when done
    i2cm:close()

</div>

</div>

</div>

<div id="simple-address-scanner" class="section">

## Simple Address Scanner<a href="#simple-address-scanner" class="headerlink" title="Link to this heading"></a>

If you do not know which address a sensor uses, scan the common 7-bit address range:

<div class="highlight-lua notranslate">

<div class="highlight">

    local i2cm <close> = esp32.i2cmaster(0, 21, 22, 100000)

    for address = 0x03, 0x77 do
       if i2cm:probe(address, 50) then
          trace(string.format("I2C device found at 0x%02X", address))
       end
    end

</div>

</div>

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Use <span class="pre">`probe`</span> during bring-up when you are not yet sure which address a device is using.

- Use <span class="pre">`readfrom`</span> for most register-based sensors.

- Use <span class="pre">`read`</span> for devices or protocols that stream data without a register pointer phase.

- If every address fails, verify that SDA and SCL are not swapped and that the sensor shares ground with the ESP32.

- If communication works at <span class="pre">`100000`</span> but not <span class="pre">`400000`</span>, the bus wiring, pull-ups, or sensor may not support the faster speed reliably.

- Close the bus object when a transaction sequence is complete, especially in longer-running applications.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="GPIO.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="GPIO API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="PCNT.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="PCNT API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/LuaShell32.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="#" class="current reference internal">LuaShell32</a>
  - <a href="#starting-the-shell" class="reference internal">Starting the Shell</a>
  - <a href="#what-the-prompt-looks-like" class="reference internal">What the Prompt Looks Like</a>
  - <a href="#running-your-first-command" class="reference internal">Running Your First Command</a>
  - <a href="#working-interactively" class="reference internal">Working Interactively</a>
  - <a href="#what-you-can-access" class="reference internal">What You Can Access</a>
  - <a href="#when-to-use-luashell32" class="reference internal">When to Use LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- LuaShell32
- <a href="../_sources/source/LuaShell32.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="luashell32" class="section">


# LuaShell32<a href="#luashell32" class="headerlink" title="Link to this heading"></a>

LuaShell32 is the interactive Lua prompt included with Xedge32. You can access it either through a USB serial connection or through the web interface. It is the quickest way to test commands, inspect the current runtime environment, and configure networking without having to create a full application first.

Think of LuaShell32 as your live control console for the device. You can use it for one-off experiments, hardware bring-up, diagnostics, and small setup tasks.

<div id="starting-the-shell" class="section">

## Starting the Shell<a href="#starting-the-shell" class="headerlink" title="Link to this heading"></a>

You can open LuaShell32 in two ways:

- **USB/serial**: Connect the ESP32 to your computer, open a serial terminal, and connect at the appropriate baud rate, typically <span class="pre">`115200`</span>.

- **Web shell**: In the Xedge editor, click the three dots (<span class="pre">`...`</span>) in the upper-right corner and select **Lua Shell**.

<div class="admonition note">

Note

The web-based shell keeps previously entered text in the editor. Before you run a new command, clear out old input so you do not accidentally submit a longer command sequence than intended.

</div>

</div>

<div id="what-the-prompt-looks-like" class="section">

## What the Prompt Looks Like<a href="#what-the-prompt-looks-like" class="headerlink" title="Link to this heading"></a>

After the ESP32 finishes booting, the shell displays a prompt like this:

<div class="highlight-default notranslate">

<div class="highlight">

    >

</div>

</div>

The <span class="pre">`>`</span> character means LuaShell32 is ready to accept Lua code.

</div>

<div id="running-your-first-command" class="section">

## Running Your First Command<a href="#running-your-first-command" class="headerlink" title="Link to this heading"></a>

Type a Lua expression or statement and press Enter. For example:

<div class="highlight-default notranslate">

<div class="highlight">

    print("Hello, world!")

</div>

</div>

LuaShell32 runs the code immediately and prints the result:

<div class="highlight-default notranslate">

<div class="highlight">

    > print("Hello, world!")
    Hello, world!

</div>

</div>

</div>

<div id="working-interactively" class="section">

## Working Interactively<a href="#working-interactively" class="headerlink" title="Link to this heading"></a>

You are not limited to single-line commands. You can create variables, call functions, and explore APIs directly from the shell.

For example:

<div class="highlight-default notranslate">

<div class="highlight">

    x = 42
    y = x + 8
    print(y)

</div>

</div>

This produces:

<div class="highlight-default notranslate">

<div class="highlight">

    > x = 42
    > y = x + 8
    > print(y)
    50

</div>

</div>

</div>

<div id="what-you-can-access" class="section">

## What You Can Access<a href="#what-you-can-access" class="headerlink" title="Link to this heading"></a>

Inside LuaShell32 you have access to:

- the standard Lua libraries,

- the Barracuda App Server APIs,

- the generic Xedge APIs, and

- the Xedge32 ESP32-specific APIs documented in this manual.

This makes the shell a practical place to try hardware calls such as <span class="pre">`esp32.gpio(...)`</span>, inspect network information, or test a sensor before you wrap the code in an LSP page or application.

</div>

<div id="when-to-use-luashell32" class="section">

## When to Use LuaShell32<a href="#when-to-use-luashell32" class="headerlink" title="Link to this heading"></a>

LuaShell32 is especially useful for:

- entering Wi-Fi credentials during first-time setup,

- verifying pin mappings and peripheral behavior,

- testing small snippets before turning them into reusable code,

- checking event delivery and network state, and

- troubleshooting a device in the field.

For larger development work, the <a href="xedge.html#xedge32" class="reference internal"><span class="std std-ref">Web-Based Lua REPL</span></a> in Xedge usually provides a better experience because you can edit LSP pages, reload them in the browser, and keep your code organized in files.

See <a href="xedge.html#xedge32" class="reference internal"><span class="std std-ref">Xedge32</span></a> for the broader development workflow.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="AccessPointMode.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Access Point Mode"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="xedge.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Xedge32">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/PCNT.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="#" class="current reference internal">PCNT API</a>
  - <a href="#why-use-pcnt" class="reference internal">Why Use PCNT</a>
  - <a href="#key-features" class="reference internal">Key Features</a>
  - <a href="#creating-a-pcnt-object" class="reference internal">Creating a PCNT Object</a>
  - <a href="#top-level-configuration-fields" class="reference internal">Top-Level Configuration Fields</a>
  - <a href="#watchpoint-configuration" class="reference internal">Watchpoint Configuration</a>
  - <a href="#channel-configuration" class="reference internal">Channel Configuration</a>
  - <a href="#pcnt-object-methods" class="reference internal">PCNT Object Methods</a>
  - <a href="#rotary-encoder-example" class="reference internal">Rotary Encoder Example</a>
  - <a href="#how-the-example-works" class="reference internal">How the Example Works</a>
  - <a href="#hardware-setup" class="reference internal">Hardware Setup</a>
    - <a href="#wiring-example" class="reference internal">Wiring Example</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- PCNT API
- <a href="../_sources/source/PCNT.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="pcnt-api" class="section">

# PCNT API<a href="#pcnt-api" class="headerlink" title="Link to this heading"></a>

The PCNT API exposes the ESP32 pulse counter hardware to Lua. Pulse counters are useful when you need to count edges, track direction, or monitor quadrature signals from devices such as rotary encoders, tachometers, flow sensors, and other pulse-producing hardware.

Xedge32 mirrors the underlying ESP-IDF PCNT model closely enough that the official <a href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/pcnt.html" class="reference external">ESP-IDF PCNT documentation</a> is still a useful reference when you need hardware-level background.

<div id="why-use-pcnt" class="section">

## Why Use PCNT<a href="#why-use-pcnt" class="headerlink" title="Link to this heading"></a>

The PCNT peripheral is much better suited than ordinary GPIO polling when you need reliable counting at higher edge rates. It moves the edge handling into hardware and lets Lua read the accumulated result or respond to watchpoints.

</div>

<div id="key-features" class="section">

## Key Features<a href="#key-features" class="headerlink" title="Link to this heading"></a>

- **16-bit signed counter** Each PCNT unit contains a signed 16-bit counter register. The hardware can handle input frequencies up to 40 MHz.

- **Two configurable channels per unit** Each channel can be programmed to increment, decrement, or hold the counter based on edge and level conditions.

- **Separate signal and control inputs** The signal input is used for edge detection, while the control input modifies how those edges affect the counter.

- **Optional glitch filtering** A hardware filter can ignore very short pulses that are likely to be noise.

- **Watchpoints with callbacks** You can trigger Lua callbacks when the counter reaches selected values such as zero, thresholds, or high and low limits.

For a deeper hardware description, see Chapter 17 of the <a href="https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf" class="reference external">ESP32 Technical Reference Manual</a>.

</div>

<div id="creating-a-pcnt-object" class="section">

## Creating a PCNT Object<a href="#creating-a-pcnt-object" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    pcnt, err = esp32.pcnt(cfg)

</div>

</div>

The <span class="pre">`cfg`</span> argument is a Lua table that describes the counter limits, optional filters, optional watchpoints, and one or two channel definitions.

</div>

<div id="top-level-configuration-fields" class="section">

## Top-Level Configuration Fields<a href="#top-level-configuration-fields" class="headerlink" title="Link to this heading"></a>

<span class="pre">`high`</span>  
Upper limit for the pulse counter.

<span class="pre">`low`</span>  
Lower limit for the pulse counter.

<span class="pre">`accumulator`</span>  
Optional boolean. When <span class="pre">`true`</span>, the driver accumulates values instead of resetting at the limits. Default is <span class="pre">`false`</span>.

<span class="pre">`glitch`</span>  
Optional glitch-filter duration in nanoseconds. Default is <span class="pre">`0`</span> which means no filter.

<span class="pre">`watch`</span>  
Optional watchpoint configuration table.

<span class="pre">`channels`</span>  
Required array of channel definitions.

</div>

<div id="watchpoint-configuration" class="section">

## Watchpoint Configuration<a href="#watchpoint-configuration" class="headerlink" title="Link to this heading"></a>

If <span class="pre">`watch`</span> is provided, it must contain:

<span class="pre">`points`</span>  
Array of counter values that should trigger a callback.

<span class="pre">`callback`</span>  
Function called when one of the watchpoints is reached.

Callback signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    function(count, crossmode)

</div>

</div>

Arguments:

- <span class="pre">`count`</span>: The watchpoint value that triggered the callback.

- <span class="pre">`crossmode`</span>: How the counter crossed zero most recently.

Possible zero-cross modes:

- <span class="pre">`0`</span>: <span class="pre">`+N`</span>` `<span class="pre">`->`</span>` `<span class="pre">`0`</span>

- <span class="pre">`1`</span>: <span class="pre">`-N`</span>` `<span class="pre">`->`</span>` `<span class="pre">`0`</span>

- <span class="pre">`2`</span>: <span class="pre">`-N`</span>` `<span class="pre">`->`</span>` `<span class="pre">`+M`</span>

- <span class="pre">`3`</span>: <span class="pre">`+N`</span>` `<span class="pre">`->`</span>` `<span class="pre">`-M`</span>

</div>

<div id="channel-configuration" class="section">

## Channel Configuration<a href="#channel-configuration" class="headerlink" title="Link to this heading"></a>

Each entry in <span class="pre">`channels`</span> is a table with <span class="pre">`gpio`</span> and <span class="pre">`action`</span> sub-tables.

<span class="pre">`gpio`</span> fields:

- <span class="pre">`edge`</span>: GPIO used for edge detection.

- <span class="pre">`level`</span>: GPIO used for control/level detection.

<span class="pre">`action.edge`</span> fields:

- <span class="pre">`positive`</span>: Action on positive edge.

- <span class="pre">`negative`</span>: Action on negative edge.

Valid edge actions:

- <span class="pre">`"HOLD"`</span>

- <span class="pre">`"INCREASE"`</span>

- <span class="pre">`"DECREASE"`</span>

<span class="pre">`action.level`</span> fields:

- <span class="pre">`high`</span>: Behavior when the control level is high.

- <span class="pre">`low`</span>: Behavior when the control level is low.

Valid level actions:

- <span class="pre">`"KEEP"`</span>

- <span class="pre">`"INVERSE"`</span>

- <span class="pre">`"HOLD"`</span>

</div>

<div id="pcnt-object-methods" class="section">

## PCNT Object Methods<a href="#pcnt-object-methods" class="headerlink" title="Link to this heading"></a>

The object returned by <span class="pre">`esp32.pcnt()`</span> provides the following methods:

<span class="pre">`pcnt:start()`</span>  
Starts counting.

<span class="pre">`pcnt:stop()`</span>  
Stops counting.

<span class="pre">`pcnt:count()`</span>  
Returns the current counter value.

<span class="pre">`pcnt:clear()`</span>  
Clears the current counter value.

</div>

<div id="rotary-encoder-example" class="section">

## Rotary Encoder Example<a href="#rotary-encoder-example" class="headerlink" title="Link to this heading"></a>

The following example mirrors the functionality of Espressif’s <a href="https://github.com/espressif/esp-idf/tree/master/examples/peripherals/pcnt/rotary_encoder" class="reference external">rotary encoder PCNT example</a>.

The example assumes the encoder signals are already pulled to a valid logic level. Many rotary encoder modules include pull-up resistors. If you use a bare mechanical encoder, add suitable pull-ups or pull-downs for your board.

<div class="highlight-lua notranslate">

<div class="highlight">

    local gpioA = 0
    local gpioB = 2

    local pcnt, err = esp32.pcnt{
       high = 100,
       low = -100,
       glitch = 1000,
       watch = {
          points = {-100, -50, 0, 50, 100},
          callback = function(count)
             trace("Watch point reached with count:", count)
          end
       },
       channels = {
          {
             gpio = {
                edge = gpioA,
                level = gpioB
             },
             action = {
                edge = {
                   positive = "DECREASE",
                   negative = "INCREASE"
                },
                level = {
                   high = "KEEP",
                   low = "INVERSE"
                }
             }
          },
          {
             gpio = {
                edge = gpioB,
                level = gpioA
             },
             action = {
                edge = {
                   positive = "INCREASE",
                   negative = "DECREASE"
                },
                level = {
                   high = "KEEP",
                   low = "INVERSE"
                }
             }
          }
       }
    }

    if pcnt then
       pcnt:start()
       local timer = ba.timer(function()
          trace("Pulse count:", pcnt:count())
          return true
       end)
       timer:set(1000)
    else
       trace(err)
    end

</div>

</div>

</div>

<div id="how-the-example-works" class="section">

## How the Example Works<a href="#how-the-example-works" class="headerlink" title="Link to this heading"></a>

This configuration is designed for a quadrature rotary encoder:

- Channel 1 looks at <span class="pre">`gpioA`</span> as the edge source and <span class="pre">`gpioB`</span> as the control level.

- Channel 2 swaps the two signals.

- The level configuration inverts counting direction depending on the state of the other encoder line.

That is what allows the counter to move up or down as the encoder is rotated in different directions.

</div>

<div id="hardware-setup" class="section">

## Hardware Setup<a href="#hardware-setup" class="headerlink" title="Link to this heading"></a>

The example assumes:

- an ESP development board, and

- an EC11 rotary encoder or another encoder that outputs quadrature waveforms.

<div id="wiring-example" class="section">

### Wiring Example<a href="#wiring-example" class="headerlink" title="Link to this heading"></a>

Connect the encoder as follows:

<div class="highlight-text notranslate">

<div class="highlight">

    +--------+              +---------------------------------+
    |        |              |                                 |
    |      A +--------------+ GPIO 0                         |
    |        |              |                                 |
    +-------+|              |                                 |
    |     | |  GND +--------------+ GND                       |
    +-------+|              |                                 |
    |        |              |                                 |
    |      B +--------------+ GPIO 2                         |
    |        |              |                                 |
    +--------+              +---------------------------------+

</div>

</div>

In this setup:

- encoder pin **A (CLK)** connects to GPIO 0,

- encoder pin **B (DT)** connects to GPIO 2, and

- encoder **GND** connects to board **GND**.

With the example configuration above, one full rotary detent typically changes the PCNT counter by four counts because the hardware sees four signal edges per quadrature cycle.

The PCNT API configures the pulse counter peripheral. It does not, by itself, turn on GPIO pull-up or pull-down resistors. Treat signal conditioning as part of the hardware setup.

</div>

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Use the glitch filter when mechanical switches or noisy signals cause false counts.

- Use watchpoints when you want low-overhead notifications at important count thresholds.

- For rotary encoders, expect to do some board-specific tuning of pull-ups, filter values, and direction mapping.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="I2C.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="I2C API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="PWM.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="PWM API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/PWM.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="#" class="current reference internal">PWM API</a>
  - <a href="#typical-pwm-workflow" class="reference internal">Typical PWM Workflow</a>
  - <a href="#quick-start-example" class="reference internal">Quick Start Example</a>
  - <a href="#creating-a-pwm-timer" class="reference internal">Creating a PWM Timer</a>
  - <a href="#creating-a-pwm-channel" class="reference internal">Creating a PWM Channel</a>
  - <a href="#channel-object-methods" class="reference internal">Channel Object Methods</a>
    - <a href="#channel-duty-pwmdutycycle-hpoint" class="reference internal"><span class="pre"><code class="docutils literal notranslate">channel:duty(pwmDutyCycle</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">[,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">hpoint])</code></span></a>
    - <a href="#channel-fade-pwmdutycycle-time" class="reference internal"><span class="pre"><code class="docutils literal notranslate">channel:fade(pwmDutyCycle,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">time)</code></span></a>
    - <a href="#channel-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">channel:close()</code></span></a>
  - <a href="#example-1-led-fading-with-callback" class="reference internal">Example 1: LED Fading with Callback</a>
  - <a href="#example-2-servo-sweep" class="reference internal">Example 2: Servo Sweep</a>
  - <a href="#important-runtime-note" class="reference internal">Important Runtime Note</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- PWM API
- <a href="../_sources/source/PWM.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="pwm-api" class="section">

# PWM API<a href="#pwm-api" class="headerlink" title="Link to this heading"></a>

The PWM API is built on top of the ESP-IDF LEDC peripheral. Although LEDC was originally designed for LED dimming, it is also a general-purpose PWM engine that works well for tasks such as:

- LED brightness control,

- RGB color mixing,

- simple tone generation,

- fan and motor control interfaces, and

- hobby servo signaling.

The ESP32 LEDC peripheral provides 16 channels that can generate independent waveforms. These channels are divided into two groups of eight. One group runs in **high-speed mode**, where duty updates are handled fully in hardware. The other group runs in **low-speed mode**. For background information, see the <a href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html" class="reference external">ESP-IDF LEDC documentation</a>.


<div id="typical-pwm-workflow" class="section">

## Typical PWM Workflow<a href="#typical-pwm-workflow" class="headerlink" title="Link to this heading"></a>

Using PWM in Xedge32 normally follows three steps:

1.  Configure a timer with <span class="pre">`esp32.pwmtimer()`</span>.

2.  Open one or more output channels with <span class="pre">`esp32.pwmchannel()`</span>.

3.  Change the duty cycle with <span class="pre">`channel:duty()`</span> or perform hardware-driven transitions with <span class="pre">`channel:fade()`</span>.

The timer controls the shared timing: frequency and duty resolution. The channel controls the actual output pin and duty value. Several channels can use the same timer when they should run at the same frequency.

</div>

<div id="quick-start-example" class="section">

## Quick Start Example<a href="#quick-start-example" class="headerlink" title="Link to this heading"></a>

The example below configures timer 0, opens channel 0 on GPIO 18, drives the output at full duty cycle for two seconds, then turns it off again.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ok, err = esp32.pwmtimer{
       mode = "LOW",
       bits = 13,
       timer = 0,
       freq = 5000,
    }

    if ok then
       local led, chErr = esp32.pwmchannel{
          mode = "LOW",
          timer = 0,
          channel = 0,
          gpio = 18,
          duty = 2^13 - 1,
       }

       if led then
          ba.sleep(2000)
          led:duty(0)
          led:close()
       else
          trace(chErr)
       end
    else
       trace(err)
    end

</div>

</div>

</div>

<div id="creating-a-pwm-timer" class="section">

## Creating a PWM Timer<a href="#creating-a-pwm-timer" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    ok, err = esp32.pwmtimer(config)

</div>

</div>

This function configures a timer. Call it before opening any channel that uses that timer. A given timer only needs to be configured once.

Required <span class="pre">`config`</span> fields:

- <span class="pre">`mode`</span>: <span class="pre">`"LOW"`</span> or <span class="pre">`"HIGH"`</span>.

- <span class="pre">`bits`</span>: Duty resolution in bits.

- <span class="pre">`timer`</span>: Timer number.

- <span class="pre">`freq`</span>: Output frequency in Hertz.

The duty range is determined by <span class="pre">`bits`</span>. For example, <span class="pre">`bits`</span>` `<span class="pre">`=`</span>` `<span class="pre">`13`</span> gives a range from <span class="pre">`0`</span> to <span class="pre">`8191`</span>. A 50 percent duty cycle is therefore roughly <span class="pre">`4096`</span>.

Some newer ESP32 targets do not provide the same high-speed LEDC mode as the original ESP32. If high-speed mode is not available on the selected target, the firmware uses the supported low-speed mode internally.

Return values:

- <span class="pre">`true`</span> on success

- <span class="pre">`nil,`</span>` `<span class="pre">`err`</span> on failure

</div>

<div id="creating-a-pwm-channel" class="section">

## Creating a PWM Channel<a href="#creating-a-pwm-channel" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    channel, err = esp32.pwmchannel(config)

</div>

</div>

This function opens a channel object that is bound to a previously configured timer.

<span class="pre">`config`</span> fields:

- <span class="pre">`mode`</span>: Required. Must match the mode used by <span class="pre">`esp32.pwmtimer()`</span>.

- <span class="pre">`timer`</span>: Required. Must match the timer configured earlier.

- <span class="pre">`channel`</span>: Required. Channel number to use.

- <span class="pre">`gpio`</span>: Required. Output GPIO pin.

- <span class="pre">`duty`</span>: Optional initial duty cycle from <span class="pre">`0`</span> to <span class="pre">`2^bits`</span>` `<span class="pre">`-`</span>` `<span class="pre">`1`</span>.

- <span class="pre">`hpoint`</span>: Optional initial hpoint value. Default is <span class="pre">`0`</span>.

- <span class="pre">`callback`</span>: Optional Lua callback used with hardware-supported fading.

Use a callback only when you need to know that a fade has completed. For simple brightness or servo-position updates, <span class="pre">`channel:duty()`</span> is usually enough.

Return values:

- channel object on success

- <span class="pre">`nil,`</span>` `<span class="pre">`error`</span> on failure

</div>

<div id="channel-object-methods" class="section">

## Channel Object Methods<a href="#channel-object-methods" class="headerlink" title="Link to this heading"></a>

<div id="channel-duty-pwmdutycycle-hpoint" class="section">

### <span class="pre">`channel:duty(pwmDutyCycle`</span>` `<span class="pre">`[,`</span>` `<span class="pre">`hpoint])`</span><a href="#channel-duty-pwmdutycycle-hpoint" class="headerlink" title="Link to this heading"></a>

Sets the channel duty cycle immediately.

Parameters:

- <span class="pre">`pwmDutyCycle`</span>: Value from <span class="pre">`0`</span> to <span class="pre">`2^bits`</span>` `<span class="pre">`-`</span>` `<span class="pre">`1`</span>.

- <span class="pre">`hpoint`</span>: Optional updated hpoint value.

Use this method when you want direct, immediate control over the output level.

</div>

<div id="channel-fade-pwmdutycycle-time" class="section">

### <span class="pre">`channel:fade(pwmDutyCycle,`</span>` `<span class="pre">`time)`</span><a href="#channel-fade-pwmdutycycle-time" class="headerlink" title="Link to this heading"></a>

Uses the hardware fade engine to transition from the current duty cycle to a new target duty cycle.

Parameters:

- <span class="pre">`pwmDutyCycle`</span>: Target duty cycle from <span class="pre">`0`</span> to <span class="pre">`2^bits`</span>` `<span class="pre">`-`</span>` `<span class="pre">`1`</span>.

- <span class="pre">`time`</span>: Maximum fade duration in milliseconds.

To use fading, the channel must be created with a callback. The callback is invoked when the fade operation completes.

</div>

<div id="channel-close" class="section">

### <span class="pre">`channel:close()`</span><a href="#channel-close" class="headerlink" title="Link to this heading"></a>

Releases the PWM channel and the resources associated with it. Closing a channel also stops its output.

</div>

</div>

<div id="example-1-led-fading-with-callback" class="section">

## Example 1: LED Fading with Callback<a href="#example-1-led-fading-with-callback" class="headerlink" title="Link to this heading"></a>

This example demonstrates interrupt-driven fading. The callback is invoked when each fade completes, and it starts the next fade in the opposite direction.

<div class="highlight-lua notranslate">

<div class="highlight">

    local bits = 13
    local maxPwmDuty = 2^bits - 1

    local ok, err = esp32.pwmtimer{
       mode = "HIGH",
       bits = bits,
       timer = 0,
       freq = 5000,
    }

    if ok then
       local duty, led = 0, 0

       local function callback()
          trace("led callback triggered", duty)
          duty = duty == 0 and maxPwmDuty or 0
          led:fade(duty, 1000)
       end

       led, err = esp32.pwmchannel{
          callback = callback,
          mode = "HIGH",
          channel = 0,
          timer = 0,
          gpio = 18,
       }

       if led then
          callback()
       else
          trace(err)
       end
    else
       trace(err)
    end

</div>

</div>

Why this pattern is useful:

- It keeps the animation logic in Lua.

- It lets the hardware perform the actual ramp.

- It avoids manually updating duty cycle values in a timer loop.

</div>

<div id="example-2-servo-sweep" class="section">

## Example 2: Servo Sweep<a href="#example-2-servo-sweep" class="headerlink" title="Link to this heading"></a>

The next example uses PWM to move a hobby servo between two end positions. It follows the same callback-driven pattern as the LED example, but uses a 50 Hz timer and duty-cycle values chosen for a typical 0 to 180 degree servo range.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ok, err = esp32.pwmtimer{
       mode = "LOW",
       bits = 13,
       timer = 0,
       freq = 50,
    }

    if ok then
       local minServoDuty, maxServoDuty = 409, 819
       local duty = maxServoDuty
       local servo

       local function callback()
          trace("servo callback triggered", duty)
          duty = duty == minServoDuty and maxServoDuty or minServoDuty
          servo:fade(duty, 3000)
       end

       servo, err = esp32.pwmchannel{
          callback = callback,
          mode = "LOW",
          channel = 0,
          timer = 0,
          gpio = 14,
       }

       if servo then
          callback()
       else
          trace(err)
       end
    else
       trace(err)
    end

</div>

</div>

The two duty values in this example were derived from the <span class="pre">`calculatePwmDutyCycle()`</span> logic used in the <a href="https://github.com/RealTimeLogic/xedge-ESP32/blob/master/Lua-Examples/servo.lsp" class="reference external">servo.lsp example</a>.

</div>

<div id="important-runtime-note" class="section">

## Important Runtime Note<a href="#important-runtime-note" class="headerlink" title="Link to this heading"></a>

If you place a callback-driven PWM example in an LSP page, remember that the Lua object can be garbage-collected after the page finishes executing if no reference is kept alive. That is convenient during quick experiments, but in a real application you normally store the PWM object somewhere persistent.

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Use **high-speed mode** when you want the cleanest hardware-driven duty-cycle updates.

- Use **low-speed mode** when that matches your application or existing design.

- Choose the timer <span class="pre">`bits`</span> value based on the balance you need between duty resolution and frequency.

- Use <span class="pre">`channel:duty()`</span> for immediate updates and <span class="pre">`channel:fade()`</span> when you want hardware-assisted transitions without building your own timer loop.

- For motors and LED strips, use the proper driver hardware. A GPIO/PWM pin is a control signal, not a power output.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="PCNT.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="PCNT API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="RMT.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="RMT API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/RMT.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="#" class="current reference internal">RMT API</a>
  - <a href="#application-examples" class="reference internal">Application Examples</a>
  - <a href="#rmt-symbol-layout" class="reference internal">RMT Symbol Layout</a>
  - <a href="#understanding-tick-resolution" class="reference internal">Understanding Tick Resolution</a>
  - <a href="#lua-rmt-symbol-representation" class="reference internal">Lua RMT Symbol Representation</a>
  - <a href="#lua-rmt-byte-encoding" class="reference internal">Lua RMT Byte Encoding</a>
  - <a href="#rmt-tx-api" class="reference internal">RMT TX API</a>
  - <a href="#creating-a-tx-channel" class="reference internal">Creating a TX Channel</a>
  - <a href="#tx-configuration-options" class="reference internal">TX Configuration Options</a>
  - <a href="#tx-object-methods" class="reference internal">TX Object Methods</a>
    - <a href="#rmttx-enable" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmttx:enable()</code></span></a>
    - <a href="#rmttx-disable" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmttx:disable()</code></span></a>
    - <a href="#rmttx-transmit-cfg-symbols" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmttx:transmit(cfg,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">symbols)</code></span></a>
    - <a href="#rmttx-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmttx:close()</code></span></a>
  - <a href="#tx-example-1-musical-score" class="reference internal">TX Example 1: Musical Score</a>
    - <a href="#how-the-musical-example-works" class="reference internal">How the Musical Example Works</a>
  - <a href="#tx-example-2-ws2812b-led-strip" class="reference internal">TX Example 2: WS2812B LED Strip</a>
    - <a href="#breaking-down-the-led-example" class="reference internal">Breaking Down the LED Example</a>
  - <a href="#rmt-rx-api" class="reference internal">RMT RX API</a>
  - <a href="#creating-an-rx-channel" class="reference internal">Creating an RX Channel</a>
  - <a href="#rx-configuration-options" class="reference internal">RX Configuration Options</a>
  - <a href="#rx-object-methods" class="reference internal">RX Object Methods</a>
    - <a href="#rmtrx-receive-cfg" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmtrx:receive(cfg)</code></span></a>
    - <a href="#rmtrx-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">rmtrx:close()</code></span></a>
  - <a href="#rx-example-1-wire-temperature-read" class="reference internal">RX Example: 1-Wire Temperature Read</a>
    - <a href="#understanding-the-1-wire-example" class="reference internal">Understanding the 1-Wire Example</a>
    - <a href="#garbage-collection-consideration" class="reference internal">Garbage-Collection Consideration</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- RMT API
- <a href="../_sources/source/RMT.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="rmt-api" class="section">

# RMT API<a href="#rmt-api" class="headerlink" title="Link to this heading"></a>

The ESP32 RMT peripheral, short for **Remote Control**, is a timing-oriented signal engine that can both transmit and receive precisely timed digital pulse sequences. It was originally introduced for infrared remote-control protocols, but it is far more general than the name suggests.

In practice, the RMT peripheral is a very useful tool whenever a protocol is defined in terms of pulse widths rather than bytes on a conventional bus. This makes it a strong fit for:

- infrared protocols,

- 1-wire devices,

- addressable LED strips such as WS2812B,

- custom pulse-encoded signaling,

- pulse measurement and decoding tasks, and

- other applications where sub-microsecond timing matters.

Because the peripheral handles timing in hardware, it is often a better choice than trying to generate or measure pulse trains directly from Lua.

For beginners, the easiest way to decide whether RMT is the right API is to ask whether the protocol is described by pulse widths. If the device talks in bytes, try UART, I2C, or another bus first. If the device talks in high/low timings, RMT is usually the correct tool.

<div id="application-examples" class="section">

## Application Examples<a href="#application-examples" class="headerlink" title="Link to this heading"></a>

Two especially common uses for RMT in Xedge32 projects are:

- **1-wire communication** Timing is critical in 1-wire protocols, and RMT provides the precision needed for reliable reads and writes.

- **Addressable LED strips** LED strips such as WS2812B require very specific pulse timing. RMT lets you generate those waveforms in hardware instead of bit-banging them from Lua.

</div>

<div id="rmt-symbol-layout" class="section">


## RMT Symbol Layout<a href="#rmt-symbol-layout" class="headerlink" title="Link to this heading"></a>

The RMT hardware represents a waveform as a sequence of **RMT symbols**. Each symbol contains timing and logic-level information describing two consecutive pulse segments.

<figure id="id2" class="align-center">
<img src="../_images/RMT-Symbol-Layout.png" alt="RMT-symbol layout" />
<figcaption><p><span class="caption-text">Bit fields in an RMT symbol as defined by the hardware.</span><a href="#id2" class="headerlink" title="Link to this image"></a></p></figcaption>
</figure>

Each RMT symbol contains:

- a **duration value** for the first segment,

- a **logic level** for the first segment,

- a **duration value** for the second segment, and

- a **logic level** for the second segment.

Duration values are expressed in **RMT clock ticks**, not in seconds directly. The tick duration depends on the configured channel resolution.

</div>

<div id="understanding-tick-resolution" class="section">

## Understanding Tick Resolution<a href="#understanding-tick-resolution" class="headerlink" title="Link to this heading"></a>

The configured <span class="pre">`resolution`</span> determines how to interpret durations:

- at <span class="pre">`1,000,000`</span> Hz, one tick is <span class="pre">`1`</span>` `<span class="pre">`us`</span>,

- at <span class="pre">`10,000,000`</span> Hz, one tick is <span class="pre">`0.1`</span>` `<span class="pre">`us`</span>,

- at <span class="pre">`80,000,000`</span> Hz, one tick is <span class="pre">`12.5`</span>` `<span class="pre">`ns`</span>.

This is one of the most important ideas when working with RMT. The symbol data does not store human-friendly time units. It stores counts that are meaningful only when combined with the selected resolution.

</div>

<div id="lua-rmt-symbol-representation" class="section">

## Lua RMT Symbol Representation<a href="#lua-rmt-symbol-representation" class="headerlink" title="Link to this heading"></a>

In Lua, a single RMT symbol is represented as a four-element table:

<div class="highlight-lua notranslate">

<div class="highlight">

    { level0, duration0, level1, duration1 }

</div>

</div>

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    {
       1, 350, 0, 800
    }

</div>

</div>

If the channel resolution is <span class="pre">`1`</span>` `<span class="pre">`MHz`</span>, the symbol above means:

- drive the line high for <span class="pre">`350`</span>` `<span class="pre">`us`</span>,

- then drive it low for <span class="pre">`800`</span>` `<span class="pre">`us`</span>.

This table format is compact, but it is helpful to think of it as a small waveform fragment rather than as generic numeric data.

</div>

<div id="lua-rmt-byte-encoding" class="section">

## Lua RMT Byte Encoding<a href="#lua-rmt-byte-encoding" class="headerlink" title="Link to this heading"></a>

Many pulse protocols define how each data bit is encoded as a pair of pulses. For example:

- a binary <span class="pre">`0`</span> might be sent as a short-high then long-low pulse pair,

- a binary <span class="pre">`1`</span> might be sent as a long-high then short-low pulse pair.

Xedge32 supports a convenient Lua structure for describing this pattern at the byte level:

<div class="highlight-lua notranslate">

<div class="highlight">

    {
       booleanMsbFirst,
       symbolForBit0,
       symbolForBit1,
       { byteArray }
    }

</div>

</div>

Fields:

- <span class="pre">`booleanMsbFirst`</span>: <span class="pre">`true`</span> to transmit the most-significant bit first, <span class="pre">`false`</span> for least-significant-bit first.

- <span class="pre">`symbolForBit0`</span>: RMT symbol that represents a transmitted zero bit.

- <span class="pre">`symbolForBit1`</span>: RMT symbol that represents a transmitted one bit.

- <span class="pre">`{`</span>` `<span class="pre">`byteArray`</span>` `<span class="pre">`}`</span>: Array of bytes to encode and transmit.

Example at <span class="pre">`10`</span>` `<span class="pre">`MHz`</span> resolution:

<div class="highlight-lua notranslate">

<div class="highlight">

    {
       true,
       {1, 4, 0, 8},
       {1, 8, 0, 4},
       {0x00, 0x55, 0xFF}
    }

</div>

</div>

With a resolution of <span class="pre">`10,000,000`</span> Hz, the values <span class="pre">`4`</span> and <span class="pre">`8`</span> correspond to <span class="pre">`0.4`</span>` `<span class="pre">`us`</span> and <span class="pre">`0.8`</span>` `<span class="pre">`us`</span> respectively.

</div>

<div id="rmt-tx-api" class="section">

## RMT TX API<a href="#rmt-tx-api" class="headerlink" title="Link to this heading"></a>

</div>

<div id="creating-a-tx-channel" class="section">

## Creating a TX Channel<a href="#creating-a-tx-channel" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    rmttx, err = esp32.rmttx(cfg [, rx])

</div>

</div>

This function creates a transmit channel object. The channel starts out disabled, so you must call <span class="pre">`rmttx:enable()`</span> before transmitting.

The optional <span class="pre">`rx`</span> argument lets you pair a TX and RX channel on the same GPIO so they can cooperate on a bidirectional bus such as 1-wire.

</div>

<div id="tx-configuration-options" class="section">

## TX Configuration Options<a href="#tx-configuration-options" class="headerlink" title="Link to this heading"></a>

Required fields:

- <span class="pre">`gpio`</span>: Output GPIO pin.

- <span class="pre">`resolution`</span>: Tick resolution in Hertz.

Optional fields:

- <span class="pre">`mem`</span>: Memory or DMA buffer sizing. Default is <span class="pre">`64`</span>.

- <span class="pre">`queue`</span>: Transaction queue depth. Default is <span class="pre">`4`</span>.

- <span class="pre">`invert`</span>: Invert the outgoing logic level. Default is <span class="pre">`false`</span>.

- <span class="pre">`dma`</span>: Enable DMA backend. Default is <span class="pre">`false`</span>.

- <span class="pre">`opendrain`</span>: Configure the GPIO in open-drain mode. Default is <span class="pre">`false`</span>.

- <span class="pre">`callback`</span>: Function called when transmission completes.

Optional carrier modulation fields:

- <span class="pre">`dutycycle`</span>: Carrier duty cycle.

- <span class="pre">`frequency`</span>: Carrier frequency in Hertz. Maximum is <span class="pre">`80000000`</span>.

- <span class="pre">`polaritylow`</span>: Select the logic level on which the carrier is applied.

</div>

<div id="tx-object-methods" class="section">

## TX Object Methods<a href="#tx-object-methods" class="headerlink" title="Link to this heading"></a>

<div id="rmttx-enable" class="section">

### <span class="pre">`rmttx:enable()`</span><a href="#rmttx-enable" class="headerlink" title="Link to this heading"></a>

Enables the RMT TX channel and prepares it for transmission.

</div>

<div id="rmttx-disable" class="section">

### <span class="pre">`rmttx:disable()`</span><a href="#rmttx-disable" class="headerlink" title="Link to this heading"></a>

Disables the TX channel and stops further activity. This is especially useful for transmissions that were started in loop mode.

</div>

<div id="rmttx-transmit-cfg-symbols" class="section">

### <span class="pre">`rmttx:transmit(cfg,`</span>` `<span class="pre">`symbols)`</span><a href="#rmttx-transmit-cfg-symbols" class="headerlink" title="Link to this heading"></a>

Queues or starts transmission of one or more RMT symbols.

Parameters:

- <span class="pre">`cfg`</span>: Transmission options.

- <span class="pre">`symbols`</span>: Either an array of direct RMT symbols or one or more byte-encode structures as described earlier.

Supported <span class="pre">`cfg`</span> fields:

- <span class="pre">`loop`</span>: Number of repeat loops. Use <span class="pre">`-1`</span> for infinite looping.

- <span class="pre">`eot`</span>: Output level to hold at end of transmission.

</div>

<div id="rmttx-close" class="section">

### <span class="pre">`rmttx:close()`</span><a href="#rmttx-close" class="headerlink" title="Link to this heading"></a>

Closes the TX channel and releases its resources.

</div>

</div>

<div id="tx-example-1-musical-score" class="section">

## TX Example 1: Musical Score<a href="#tx-example-1-musical-score" class="headerlink" title="Link to this heading"></a>

The following example plays a simple melody, Beethoven’s *Ode to Joy*, by transmitting a square wave for each note frequency. The score table is adapted from Espressif’s <a href="https://github.com/espressif/esp-idf/tree/master/examples/peripherals/rmt/musical_buzzer" class="reference external">musical buzzer example</a>.

<div class="highlight-lua notranslate">

<div class="highlight">

    local score = {
       {740, 400}, {740, 600}, {784, 400}, {880, 400},
       {880, 400}, {784, 400}, {740, 400}, {659, 400},
       {587, 400}, {587, 400}, {659, 400}, {740, 400},
       {740, 400}, {740, 200}, {659, 200}, {659, 800},

       {740, 400}, {740, 600}, {784, 400}, {880, 400},
       {880, 400}, {784, 400}, {740, 400}, {659, 400},
       {587, 400}, {587, 400}, {659, 400}, {740, 400},
       {659, 400}, {659, 200}, {587, 200}, {587, 800},

       {659, 400}, {659, 400}, {740, 400}, {587, 400},
       {659, 400}, {740, 200}, {784, 200}, {740, 400}, {587, 400},
       {659, 400}, {740, 200}, {784, 200}, {740, 400}, {659, 400},
       {587, 400}, {659, 400}, {440, 400}, {440, 400},

       {740, 400}, {740, 600}, {784, 400}, {880, 400},
       {880, 400}, {784, 400}, {740, 400}, {659, 400},
       {587, 400}, {587, 400}, {659, 400}, {740, 400},
       {659, 400}, {659, 200}, {587, 200}, {587, 800},
    }

    local resolution = 1000000

    local function play(rmt)
       for _, note in ipairs(score) do
          local freq, duration = note[1], note[2]
          local symbolDuration = resolution / freq / 2
          rmt:transmit({loop = duration * freq / 1000}, {
             {0, symbolDuration, 1, symbolDuration}
          })
          coroutine.yield()
       end
       rmt:close()
    end

    local coro = coroutine.create(play)

    local rmt, err = esp32.rmttx{
       gpio = 0,
       resolution = resolution,
       callback = function()
          coroutine.resume(coro)
       end
    }

    if rmt then
       rmt:enable()
       coroutine.resume(coro, rmt)
    else
       trace(err)
    end

    function onunload()
       rmt:close()
    end

</div>

</div>

<div id="how-the-musical-example-works" class="section">

### How the Musical Example Works<a href="#how-the-musical-example-works" class="headerlink" title="Link to this heading"></a>

The <span class="pre">`play()`</span> function runs as a Lua coroutine. That lets the code look sequential even though the actual note timing is event-driven.

For each note:

1.  The frequency is converted into a half-period duration.

2.  A single RMT symbol is transmitted in a loop to generate a square wave.

3.  The coroutine yields.

4.  The TX callback resumes the coroutine after the note duration finishes.

This is a useful general RMT pattern: let hardware perform the timing while Lua coordinates higher-level sequencing.

</div>

</div>

<div id="tx-example-2-ws2812b-led-strip" class="section">

## TX Example 2: WS2812B LED Strip<a href="#tx-example-2-ws2812b-led-strip" class="headerlink" title="Link to this heading"></a>

The following example drives a WS2812B-compatible LED strip by defining the timing for bit <span class="pre">`0`</span> and bit <span class="pre">`1`</span> and then repeatedly retransmitting updated color data.

<div class="highlight-lua notranslate">

<div class="highlight">

    local leds = 1
    local gpioPin = 48
    local rmt, err

    local data = {}
    for i = 1, leds * 3 do
       table.insert(data, ba.rnd(0, 0xFF))
    end

    local function transmit()
       rmt:transmit({}, {
          {
             true,
             {1, 4, 0, 8},
             {1, 8, 0, 4},
             data
          },
          {0, 150, 0, 150}
       })
    end

    rmt, err = esp32.rmttx{
       gpio = gpioPin,
       resolution = 10000000,
       callback = function()
          for i = 1, #data do
             local c = data[i] + 1
             if c > 0xFF then
                c = 0
             end
             data[i] = c
          end
          transmit()
       end
    }

    if rmt then
       rmt:enable()
       transmit()
    else
       trace(err)
    end

    function onunload()
       trace"Stopping wled"
       if rmt then
          for i = 1, #data do
             data[i] = 0
          end
          transmit()
          transmit = function()
             rmt:close()
          end
       end
    end

</div>

</div>

<div id="breaking-down-the-led-example" class="section">

### Breaking Down the LED Example<a href="#breaking-down-the-led-example" class="headerlink" title="Link to this heading"></a>

Preparing color data:

- <span class="pre">`data`</span> contains three bytes per LED.

- Each group of three bytes represents one RGB color value.

Transmit function:

- The first item in the transmit table describes byte encoding for the LED protocol.

- <span class="pre">`true`</span> selects MSB-first transmission.

- <span class="pre">`{1,`</span>` `<span class="pre">`4,`</span>` `<span class="pre">`0,`</span>` `<span class="pre">`8}`</span> defines the waveform for a zero bit.

- <span class="pre">`{1,`</span>` `<span class="pre">`8,`</span>` `<span class="pre">`0,`</span>` `<span class="pre">`4}`</span> defines the waveform for a one bit.

- <span class="pre">`{0,`</span>` `<span class="pre">`150,`</span>` `<span class="pre">`0,`</span>` `<span class="pre">`150}`</span> adds the reset pulse after the color data.

Continuous animation:

- The TX callback updates the color table after each completed transfer.

- The callback then starts the next frame.

- This creates a continuous animation loop without a separate timer.

Cleanup:

- <span class="pre">`onunload()`</span> turns the LEDs off before closing the channel.

- This matters when the code runs as an <span class="pre">`.xlua`</span> page and may be reloaded or stopped while the previous transmission is still active.

</div>

</div>

<div id="rmt-rx-api" class="section">

## RMT RX API<a href="#rmt-rx-api" class="headerlink" title="Link to this heading"></a>

</div>

<div id="creating-an-rx-channel" class="section">

## Creating an RX Channel<a href="#creating-an-rx-channel" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    rmtrx, err = esp32.rmtrx(cfg)

</div>

</div>

This creates a receive channel that captures incoming pulse timings as RMT symbols.

</div>

<div id="rx-configuration-options" class="section">

## RX Configuration Options<a href="#rx-configuration-options" class="headerlink" title="Link to this heading"></a>

Required fields:

- <span class="pre">`gpio`</span>: Input GPIO pin.

- <span class="pre">`resolution`</span>: Tick resolution in Hertz.

- <span class="pre">`callback`</span>: Lua function called when a receive job completes.

Optional fields:

- <span class="pre">`mem`</span>: Memory or DMA buffer sizing. Default is <span class="pre">`64`</span>.

- <span class="pre">`invert`</span>: Invert input levels before processing. Default is <span class="pre">`false`</span>.

- <span class="pre">`dma`</span>: Enable DMA backend. Default is <span class="pre">`false`</span>.

RX callback signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    function callback(symbols, overflow)

</div>

</div>

Arguments:

- <span class="pre">`symbols`</span>: Array of received RMT symbols.

- <span class="pre">`overflow`</span>: Boolean indicating whether the receive buffer overflowed.

</div>

<div id="rx-object-methods" class="section">

## RX Object Methods<a href="#rx-object-methods" class="headerlink" title="Link to this heading"></a>

<div id="rmtrx-receive-cfg" class="section">

### <span class="pre">`rmtrx:receive(cfg)`</span><a href="#rmtrx-receive-cfg" class="headerlink" title="Link to this heading"></a>

Starts a receive job and returns immediately.

Receive <span class="pre">`cfg`</span> fields:

- <span class="pre">`min`</span>: Minimum valid pulse duration in nanoseconds. Shorter pulses are treated as glitches.

- <span class="pre">`max`</span>: Maximum valid pulse duration in nanoseconds. Longer pulses are treated as a stop signal and end the receive operation.

- <span class="pre">`len`</span>: Optional receive buffer length in RMT symbols. Default is <span class="pre">`512`</span>.

- <span class="pre">`defer`</span>: Optional boolean. Relevant when TX and RX are linked to the same GPIO.

When <span class="pre">`defer`</span> is used with a linked TX/RX pair:

- <span class="pre">`false`</span> means reception starts immediately, so transmitted symbols may also appear in the receive result.

- <span class="pre">`true`</span> delays reception until the linked TX channel finishes transmitting.

</div>

<div id="rmtrx-close" class="section">

### <span class="pre">`rmtrx:close()`</span><a href="#rmtrx-close" class="headerlink" title="Link to this heading"></a>

Closes the RX channel and releases its resources.

</div>

</div>

<div id="rx-example-1-wire-temperature-read" class="section">

## RX Example: 1-Wire Temperature Read<a href="#rx-example-1-wire-temperature-read" class="headerlink" title="Link to this heading"></a>

The following example shows how RMT can be used to implement a 1-wire temperature transaction against a DS18B20-style sensor.

<div class="highlight-lua notranslate">

<div class="highlight">

     1local tInsert = table.insert
     2local cResume, cYield = coroutine.resume, coroutine.yield
     3
     4local function decodeBytes(symbols)
     5   local mask, byte, t = 1, 0, {}
     6   for _, sym in ipairs(symbols) do
     7      if sym[2] <= 15 then
     8         byte = byte | mask
     9      end
    10      mask = mask << 1
    11      if 256 == mask then
    12         tInsert(t, byte)
    13         mask, byte = 1, 0
    14      end
    15   end
    16   return t
    17end
    18
    19local function readTemp(gpio, callback)
    20   local coro
    21
    22   local function tempThread()
    23      local txCfg = {eot = 1}
    24
    25      local rx <close> = esp32.rmtrx{
    26         gpio = gpio,
    27         resolution = 1000000,
    28         callback = function(symbols)
    29            cResume(coro, symbols)
    30         end
    31      }
    32
    33      local tx <close> = esp32.rmttx({
    34         gpio = gpio,
    35         opendrain = true,
    36         resolution = 1000000,
    37      }, rx)
    38
    39      local function busReset()
    40         rx:receive{min = 2000, max = 480 * 2 * 1000}
    41         tx:transmit(txCfg, {{0, 480, 1, 70}})
    42         local symbols = cYield()
    43         if #symbols < 2 then
    44            callback(nil, "No sensors connected")
    45         end
    46         return #symbols < 2
    47      end
    48
    49      local function sendCommand(cmd)
    50         rx:receive{min = 900, max = 70 * 1000}
    51         tx:transmit(txCfg, {
    52            {
    53               false,
    54               {0, 60, 1, 2},
    55               {0, 6, 1, 56},
    56               cmd
    57            }
    58         })
    59         return cYield()
    60      end
    61
    62      local function readBytes(len)
    63         local t = {}
    64         for i = 1, len do
    65            tInsert(t, 0xFF)
    66         end
    67         return sendCommand(t)
    68      end
    69
    70      tx:enable()
    71      tx:transmit(txCfg, {{1, 1, 1, 0}})
    72      if busReset() then
    73         return
    74      end
    75
    76      sendCommand{0xCC, 0x44}
    77      ba.timer(function()
    78         cResume(coro)
    79      end):set(1000, true)
    80      cYield()
    81
    82      if busReset() then
    83         return
    84      end
    85
    86      sendCommand{0xCC, 0xBE}
    87      local data = decodeBytes(readBytes(2))
    88      local raw = (data[2] << 8) + data[1]
    89      callback(raw * 0.0625)
    90   end
    91
    92   coro = coroutine.create(tempThread)
    93   cResume(coro)
    94end
    95
    96readTemp(1, function(temp, err)
    97   trace(temp, err)
    98end)

</div>

</div>

<div id="understanding-the-1-wire-example" class="section">

### Understanding the 1-Wire Example<a href="#understanding-the-1-wire-example" class="headerlink" title="Link to this heading"></a>

This example is dense, so it helps to break it into pieces.

1-wire background:

- A bus reset begins communication by pulling the line low for at least <span class="pre">`480`</span>` `<span class="pre">`us`</span>.

- A device indicates presence by pulling the line low in response.

- Bit timing determines whether the transmitted or received value is a zero or a one.

How the Lua code is organized:

- <span class="pre">`tempThread()`</span> is written as a coroutine so the protocol can be expressed in sequential steps.

- The RX callback resumes the coroutine whenever a receive operation completes.

- TX and RX share the same GPIO pin, which is required for 1-wire signaling.

What <span class="pre">`decodeBytes()`</span> does:

- It examines the low-duration part of each received symbol.

- Short low pulses are interpreted as binary <span class="pre">`1`</span>.

- Longer low pulses are interpreted as binary <span class="pre">`0`</span>.

- Bits are shifted into bytes in least-significant-bit-first order.

Protocol flow:

1.  Reset the bus.

2.  Send <span class="pre">`0xCC`</span> and <span class="pre">`0x44`</span> to skip ROM selection and start temperature conversion.

3.  Wait for the conversion to complete.

4.  Reset the bus again.

5.  Send <span class="pre">`0xCC`</span> and <span class="pre">`0xBE`</span> to read the scratchpad.

6.  Decode the returned bytes into a temperature value.

Why the example uses <span class="pre">`0xFF`</span> during reads:

In 1-wire, the master must still generate timing slots while reading. Sending a series of one bits creates those read slots, and the sensor alters the observed pulse widths to encode its response.

</div>

<div id="garbage-collection-consideration" class="section">

### Garbage-Collection Consideration<a href="#garbage-collection-consideration" class="headerlink" title="Link to this heading"></a>

One subtle issue in coroutine-driven code like this is object lifetime. The example intentionally keeps the logic focused on protocol flow, but a production implementation should keep an explicit reference to the coroutine or wrap the entire behavior in a long-lived object.

Without a stable reference, Lua’s garbage collector could reclaim the coroutine while it is waiting for an event. If that happened, the callback would never resume the protocol.

</div>

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Start by choosing a resolution that makes your protocol timing easy to reason about.

- Use direct symbols when you are hand-crafting pulse sequences.

- Use byte encoding when the protocol naturally describes timing for bit <span class="pre">`0`</span> and bit <span class="pre">`1`</span>.

- Pair TX and RX channels on the same GPIO for bidirectional single-wire buses.

- Always think about cleanup and object lifetime when running long-lived RMT activity from LSP or hot-reloaded code.

- If your timing values are hard to read, lower the resolution so common pulse widths convert to simple integer tick counts.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="PWM.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="PWM API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="UART.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="UART API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/RTU.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="#" class="current reference internal">Modbus RTU Module</a>
  - <a href="#creating-an-rtu-client" class="reference internal">Creating an RTU Client</a>
    - <a href="#parameters" class="reference internal">Parameters</a>
    - <a href="#return-value" class="reference internal">Return Value</a>
  - <a href="#basic-example" class="reference internal">Basic Example</a>
  - <a href="#rs-232-full-duplex-rs-485-and-half-duplex-rs-485" class="reference internal">RS-232, Full-Duplex RS-485, and Half-Duplex RS-485</a>
  - <a href="#modbus-test-bench" class="reference internal">Modbus Test Bench</a>
    - <a href="#test-bench-components" class="reference internal">Test Bench Components</a>
    - <a href="#wiring-example" class="reference internal">Wiring Example</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Modbus RTU Module
- <a href="../_sources/source/RTU.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="modbus-rtu-module" class="section">

# Modbus RTU Module<a href="#modbus-rtu-module" class="headerlink" title="Link to this heading"></a>

This Lua module extends the Barracuda App Server Modbus TCP client so it can communicate over Modbus RTU on serial links such as RS-232 and RS-485. In practice, the module acts as a transport adapter: it keeps the familiar Modbus client API while using the <a href="UART.html#uart-api" class="reference internal"><span class="std std-ref">UART API</span></a> for the underlying serial communication.

<div class="admonition note">

Note

The module is currently included in the Xedge32 firmware, but it may be moved into a separately distributed Lua module in a future release.

</div>

<div id="creating-an-rtu-client" class="section">

## Creating an RTU Client<a href="#creating-an-rtu-client" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    rtu = require"modbus.rtu".connect(port, config)

</div>

</div>

<div id="parameters" class="section">

### Parameters<a href="#parameters" class="headerlink" title="Link to this heading"></a>

The <span class="pre">`port`</span> and <span class="pre">`config`</span> arguments are forwarded to <a href="UART.html#uart-func" class="reference internal"><span class="std std-ref">Creating a UART Object</span></a>. The <span class="pre">`config`</span> table also supports the Modbus TCP client’s \`\`onclose\` option \<<a href="https://realtimelogic.com/ba/doc/?url=Modbus.html#onclose" class="reference external">https://realtimelogic.com/ba/doc/?url=Modbus.html#onclose</a>\>\`\_.

</div>

<div id="return-value" class="section">

### Return Value<a href="#return-value" class="headerlink" title="Link to this heading"></a>

The function returns a Modbus object with the same method set as the <a href="https://realtimelogic.com/ba/doc/?url=Modbus.html" class="reference external">Modbus TCP client</a>.

The returned object is preconfigured for asynchronous cosocket operation, which means you should provide a callback for each Modbus operation.

</div>

</div>

<div id="basic-example" class="section">

## Basic Example<a href="#basic-example" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    local cfg = {
       baudrate = 9600,
       txpin = 42,
       rxpin = 41,
       onclose = function(err)
          trace("Serial Comm. Err.", err)
       end
    }

    -- Does not return errors, but may throw on incorrect settings
    local mb = require"modbus.rtu".connect(1, cfg)

    local function mycallback(data, err, transaction, client)
       trace("table" == type(data) and ba.json.encode(data) or data, err)
    end

    local unitId = 1
    mb:wholding(0, {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20}, unitId, mycallback)
    mb:rholding(0, 20, unitId, mycallback)

</div>

</div>

The callback above prints results similar to:

<div class="highlight-text notranslate">

<div class="highlight">

    true  nil
    [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]  nil

</div>

</div>

</div>

<div id="rs-232-full-duplex-rs-485-and-half-duplex-rs-485" class="section">

## RS-232, Full-Duplex RS-485, and Half-Duplex RS-485<a href="#rs-232-full-duplex-rs-485-and-half-duplex-rs-485" class="headerlink" title="Link to this heading"></a>

The first example configures the UART for RS-232 or full-duplex RS-485.

For half-duplex two-wire RS-485, you also need collision-detection support and a suitable transceiver such as ADM483 wired to the RTS GPIO pin. In that case, enable the UART <span class="pre">`rs485`</span> option:

<div class="highlight-lua notranslate">

<div class="highlight">

    local cfg = {
       baudrate = 9600,
       txpin = 42,
       rxpin = 41,
       rtspin = 40,
       rs485 = true
    }

</div>

</div>

</div>

<div id="modbus-test-bench" class="section">

## Modbus Test Bench<a href="#modbus-test-bench" class="headerlink" title="Link to this heading"></a>

The image below shows a simple two-wire RS-485 test setup.

<img src="../_images/Modbus-test-bench.jpg" class="align-center" alt="Modbus RTU test bench" />

<div id="test-bench-components" class="section">

### Test Bench Components<a href="#test-bench-components" class="headerlink" title="Link to this heading"></a>

- ESP32-S3

- ANMBEST MAX485 RS485 transceiver module

- USB-to-RS485 converter

- A Modbus slave simulator running on Windows and connected to the converter

</div>

<div id="wiring-example" class="section">

### Wiring Example<a href="#wiring-example" class="headerlink" title="Link to this heading"></a>

The following wiring matches the half-duplex configuration shown earlier.

Power connections:

- **VCC** on MAX485 to **5V** on the ESP32

- **GND** on MAX485 to **GND** on the ESP32

Data connections:

- **DI** on MAX485 to GPIO 42 on the ESP32 for **TX**

- **RO** on MAX485 to GPIO 41 on the ESP32 for **RX**

Control pin:

- **RE** and **DE** connected together, then wired to GPIO 40 for **RTS**

RS-485 bus terminals:

- **A** and **B** on the MAX485 to the matching **A** and **B** lines on the USB-to-RS485 converter

For longer RS-485 wiring, also consider termination and biasing. Short bench tests may work without them, but production wiring should follow normal RS-485 bus design rules.

</div>

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Use RS-232 or full-duplex RS-485 when the wiring and hardware already support separate send and receive paths.

- Use half-duplex RS-485 when you need a multidrop industrial bus, but make sure your transceiver and RTS wiring are configured correctly.

- Keep the callback-based programming style from the Modbus client API; the RTU transport layer is designed around asynchronous requests and responses.

- If requests time out, first verify baud rate, parity, slave unit ID, A/B polarity, and shared ground before changing the Lua code.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="BME280.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="BME280 Module"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="tutorials.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Tutorials and AUX APIs">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/UART.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="#" class="current reference internal">UART API</a>
  - <a href="#creating-a-uart-object" class="reference internal">Creating a UART Object</a>
    - <a href="#parameters" class="reference internal">Parameters</a>
  - <a href="#receive-callback-signatures" class="reference internal">Receive Callback Signatures</a>
  - <a href="#uart-object-methods" class="reference internal">UART Object Methods</a>
    - <a href="#uart-read-timeout" class="reference internal"><span class="pre"><code class="docutils literal notranslate">uart:read([timeout])</code></span></a>
    - <a href="#uart-write-data" class="reference internal"><span class="pre"><code class="docutils literal notranslate">uart:write(data)</code></span></a>
    - <a href="#uart-txsize" class="reference internal"><span class="pre"><code class="docutils literal notranslate">uart:txsize()</code></span></a>
    - <a href="#uart-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">uart:close()</code></span></a>
  - <a href="#uart-echo-example" class="reference internal">UART Echo Example</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- UART API
- <a href="../_sources/source/UART.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="uart-api" class="section">


# UART API<a href="#uart-api" class="headerlink" title="Link to this heading"></a>

The UART API provides access to the ESP32’s Universal Asynchronous Receiver/Transmitter hardware. Use it when you need serial communication with sensors, modems, external MCUs, industrial adapters, or RS-232/RS-485 transceivers.

Xedge32 supports both polling and callback-driven receive handling. In most cases, the callback-based approach is preferred because it is more responsive and avoids constant polling from Lua.

The transmit side can also operate efficiently in a semi-asynchronous style as long as the TX ring buffer has enough free space to accept the outgoing data.

<div class="admonition important">

Important

ESP32 UART pins use 3.3 V logic. Do not connect them directly to classic RS-232 voltage levels. Use the correct level shifter, RS-232 transceiver, or RS-485 transceiver for the device you are connecting.

</div>

<div id="creating-a-uart-object" class="section">


## Creating a UART Object<a href="#creating-a-uart-object" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    uart = esp32.uart(port [, config])

</div>

</div>

<div id="parameters" class="section">

### Parameters<a href="#parameters" class="headerlink" title="Link to this heading"></a>

<span class="pre">`port`</span>  
UART port number, for example <span class="pre">`0`</span>.

Be careful with UART0 on boards where it is also used for flashing or the serial console. For external devices, UART1 or UART2 is often easier to dedicate to the project.

<span class="pre">`config`</span>  
Optional table with the following settings:

- <span class="pre">`callback`</span>: Receive callback function. Setting this enables interrupt-driven RX handling.

- <span class="pre">`databits`</span>: Data bits from <span class="pre">`5`</span> to <span class="pre">`8`</span>. Default is <span class="pre">`8`</span>.

- <span class="pre">`baudrate`</span>: Baud rate. Default is <span class="pre">`9600`</span>.

- <span class="pre">`rxbufsize`</span>: Receive buffer size. Default is <span class="pre">`1024`</span>.

- <span class="pre">`txbufsize`</span>: Transmit buffer size. Default is <span class="pre">`1024`</span>.

- <span class="pre">`txpin`</span>: TX GPIO. Defaults to the port’s default pin.

- <span class="pre">`rxpin`</span>: RX GPIO. Defaults to the port’s default pin.

- <span class="pre">`rtspin`</span>: RTS GPIO. Disabled by default.

- <span class="pre">`ctspin`</span>: CTS GPIO. Disabled by default.

- <span class="pre">`stopbits`</span>: Stop-bit setting. Use <span class="pre">`1`</span> or <span class="pre">`2`</span> explicitly. Values other than <span class="pre">`1`</span> and <span class="pre">`2`</span> are interpreted as <span class="pre">`1.5`</span>.

- <span class="pre">`parity`</span>: Either <span class="pre">`"EVEN"`</span> or <span class="pre">`"ODD"`</span>.

- <span class="pre">`flowctrl`</span>: Either <span class="pre">`"RTS"`</span>, <span class="pre">`"CTS"`</span>, or <span class="pre">`"CTSRTS"`</span>.

- <span class="pre">`rs485`</span>: Enables RS-485 half-duplex mode. Requires <span class="pre">`rtspin`</span> when used with a suitable transceiver such as ADM483.

- <span class="pre">`timeout`</span>: Default <span class="pre">`0`</span>. Without <span class="pre">`pattern`</span>, this defines how long to wait before triggering the callback, measured in UART character times.

- <span class="pre">`pattern`</span>: Enables pattern detection, for example <span class="pre">`"+++"`</span>. Requires a callback.

Additional options when <span class="pre">`pattern`</span> is enabled:

- <span class="pre">`maxlen`</span>: Maximum number of recorded pattern positions. Default is <span class="pre">`0`</span>.

- <span class="pre">`timeout`</span>: Maximum gap allowed between characters in the same pattern.

- <span class="pre">`postidle`</span>: Minimum idle time required after the pattern.

- <span class="pre">`preidle`</span>: Minimum idle time required before the pattern.

For the lower-level hardware behavior behind pattern detection, see the ESP-IDF function <a href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/uart.html?highlight=enable_pattern_det_baud_intr#_CPPv433uart_enable_pattern_det_baud_intr11uart_port_tc7uint8_tiii" class="reference external">uart_enable_pattern_det_baud_intr()</a>.

</div>

</div>

<div id="receive-callback-signatures" class="section">

## Receive Callback Signatures<a href="#receive-callback-signatures" class="headerlink" title="Link to this heading"></a>

When using a callback, Xedge32 calls one of the following forms:

<span class="pre">`function(data,`</span>` `<span class="pre">`pattern)`</span>  
<span class="pre">`data`</span> is a Lua string containing one or more received characters. <span class="pre">`pattern`</span> is a boolean set to <span class="pre">`true`</span> when the data corresponds to a detected pattern.

<span class="pre">`function(nil,`</span>` `<span class="pre">`emsg)`</span>  
Called for receive-side errors or special UART conditions.

Possible <span class="pre">`emsg`</span> values:

- <span class="pre">`"full"`</span>: RX buffer full; queue reset.

- <span class="pre">`"overflow"`</span>: RX FIFO overflow; queue reset.

- <span class="pre">`"frame"`</span>: Framing error.

- <span class="pre">`"parity"`</span>: Parity error.

- <span class="pre">`"databreak"`</span>: Data-and-break event.

- <span class="pre">`"break"`</span>: Break event.

</div>

<div id="uart-object-methods" class="section">

## UART Object Methods<a href="#uart-object-methods" class="headerlink" title="Link to this heading"></a>

<div id="uart-read-timeout" class="section">

### <span class="pre">`uart:read([timeout])`</span><a href="#uart-read-timeout" class="headerlink" title="Link to this heading"></a>

Reads data from the RX queue and optionally waits up to <span class="pre">`timeout`</span> milliseconds. Do not combine this method with a receive callback on the same object.

Use <span class="pre">`read`</span> for quick tests or request/response protocols where your code knows when a reply should arrive. Use a callback when the device may send data at any time.

</div>

<div id="uart-write-data" class="section">

### <span class="pre">`uart:write(data)`</span><a href="#uart-write-data" class="headerlink" title="Link to this heading"></a>

Writes data to the UART. The call blocks until the data has been pushed into the TX FIFO or TX ring buffer. If the transmit side is full, the call waits.

</div>

<div id="uart-txsize" class="section">

### <span class="pre">`uart:txsize()`</span><a href="#uart-txsize" class="headerlink" title="Link to this heading"></a>

Returns the remaining free space in the TX ring buffer. This is useful when you want to decide how much data you can queue without blocking.

</div>

<div id="uart-close" class="section">

### <span class="pre">`uart:close()`</span><a href="#uart-close" class="headerlink" title="Link to this heading"></a>

Releases the UART object and its associated resources.

</div>

</div>

<div id="uart-echo-example" class="section">

## UART Echo Example<a href="#uart-echo-example" class="headerlink" title="Link to this heading"></a>

The following example opens UART port 2, configures GPIO 4 and GPIO 5 as TX and RX, and echoes all received data back to the sender.

<div class="highlight-lua notranslate">

<div class="highlight">

    local uart
    local cfg = {
       baudrate = 115200,
       txpin = 4,
       rxpin = 5,
       callback = function(data, err)
          if data then
             uart:write(data)
          else
             trace("Err", err)
          end
       end
    }

    uart = esp32.uart(2, cfg)

</div>

</div>

This callback-driven pattern is a good default when you need responsive serial input handling without writing your own polling loop.

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Prefer callbacks over <span class="pre">`uart:read()`</span> when the device should react to incoming serial data promptly.

- Use <span class="pre">`uart:txsize()`</span> if you are streaming large bursts and want to avoid unexpected blocking.

- Enable <span class="pre">`rs485`</span> only when your hardware design specifically requires half-duplex RS-485 control through RTS.

- Match baud rate, parity, stop bits, and wiring to the external device before troubleshooting higher-level protocol code.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="RMT.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="RMT API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="misc.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Miscellaneous API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/applications.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="#" class="current reference internal">Applications</a>
  - <a href="#creating-and-managing-xedge32-applications" class="reference internal">Creating and Managing Xedge32 Applications</a>
  - <a href="#uploading-your-application" class="reference internal">Uploading Your Application</a>
  - <a href="#zip-file-applications" class="reference internal">ZIP File Applications</a>
  - <a href="#creating-an-xedge32-application" class="reference internal">Creating an Xedge32 Application</a>
  - <a href="#optional-config-script" class="reference internal">Optional <span class="pre"><code class="docutils literal notranslate">.config</code></span> Script</a>
  - <a href="#example-config-script" class="reference internal">Example <span class="pre"><code class="docutils literal notranslate">.config</code></span> Script</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Applications
- <a href="../_sources/source/applications.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="applications" class="section">

# Applications<a href="#applications" class="headerlink" title="Link to this heading"></a>

<div id="creating-and-managing-xedge32-applications" class="section">

## Creating and Managing Xedge32 Applications<a href="#creating-and-managing-xedge32-applications" class="headerlink" title="Link to this heading"></a>

An Xedge32 application is typically packaged as a ZIP file containing Lua code and, when needed, supporting assets such as HTML, CSS, JavaScript, images, and configuration scripts. Xedge32 can run this ZIP file directly, which makes deployment simple and keeps the upload workflow friendly for both development and production use.

This packaging model is helpful because it lets you move a complete application onto the device as a single artifact instead of uploading individual files one by one.

</div>

<div id="uploading-your-application" class="section">

## Uploading Your Application<a href="#uploading-your-application" class="headerlink" title="Link to this heading"></a>

You can install an application ZIP file in several ways:

1.  **Firmware Update & App Upload page** Open the page from the three-dot menu in the upper-right corner of the Xedge32 interface and drag the ZIP file into the browser window.

2.  **Web File Manager** Upload the ZIP file manually, then add it as an application from the Xedge32 IDE.

3.  **WebDAV** Mount the device as a drive, copy the ZIP file over, and then enable it as an application from the IDE.

The first method is the easiest for most users because it uploads and installs the ZIP file in one step. Some packaged applications also depend on this path because they include <a href="#mkapp" class="reference internal">post-installation configuration scripts</a> that should run automatically during installation.

</div>

<div id="zip-file-applications" class="section">

## ZIP File Applications<a href="#zip-file-applications" class="headerlink" title="Link to this heading"></a>

A ZIP file uploaded as an application is treated as a ready-to-run deployment unit. This is different from editing loose Lua files directly on the device.

This approach gives you two useful workflows:

- keep the application packaged when you want a clean deployment artifact, or

- unpack and modify the application on the device when you want to iterate directly in Xedge32.

<div class="admonition note">

Note

You can inspect files stored inside an uploaded ZIP archive, but you cannot save changes back into the archive itself. If you want to edit files on the device, unpack the application first.

</div>

For more examples of the upload workflow, see:

- <a href="https://realtimelogic.com/articles/Designing-Your-First-Professional-Embedded-Web-Interface#upload" class="reference external">Designing Your First Professional Embedded Web Interface</a>

- <a href="https://realtimelogic.com/articles/Your-First-Embedded-Single-Page-Application#upload" class="reference external">Your First Embedded Single Page Application</a>

</div>

<div id="creating-an-xedge32-application" class="section">


## Creating an Xedge32 Application<a href="#creating-an-xedge32-application" class="headerlink" title="Link to this heading"></a>

To build your own application package:

1.  Gather your Lua files and any supporting assets.

2.  Place them in a directory structure exactly as you want them to appear in the application.

3.  Create a ZIP file from the *contents* of that directory, not from the directory itself.

In practice, this means files such as <span class="pre">`.preload`</span> or <span class="pre">`.config`</span> must be at the root of the ZIP archive if you want Xedge32 to detect them correctly.

</div>

<div id="optional-config-script" class="section">

## Optional <span class="pre">`.config`</span> Script<a href="#optional-config-script" class="headerlink" title="Link to this heading"></a>

An application may include a <span class="pre">`.config`</span> script for metadata and installation hooks. If present, the script must return a Lua table with any of the following optional fields:

- <span class="pre">`name`</span>: String shown for the application in the Xedge32 left-hand pane. If omitted, the ZIP filename is used.

- <span class="pre">`autostart`</span>: Boolean that enables automatic startup and marks the app as running after installation.

- <span class="pre">`install`</span>: Function called for a new installation.

- <span class="pre">`upgrade`</span>: Function called when an existing application is updated.

</div>

<div id="example-config-script" class="section">

## Example <span class="pre">`.config`</span> Script<a href="#example-config-script" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Argument 'io' is the application's IO instance
    local function install(io)
       trace("install", io:realpath(""))
       return "This message is sent to the browser and displayed after installation"
    end

    local function upgrade(io)
       return "This message is sent to the browser and displayed after upgrade"
    end

    return {
       name = "MY APP",
       autostart = true,
       install = install,
       upgrade = upgrade
    }

</div>

</div>

The <span class="pre">`install`</span> and <span class="pre">`upgrade`</span> hooks are a good place to perform first-run setup, initialize directories, migrate stored data, or provide a helpful status message back to the browser.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="tutorials.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Tutorials and AUX APIs"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="license.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="License">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/cam.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="#" class="current reference internal">Camera API</a>
  - <a href="#creating-a-camera-object" class="reference internal">Creating a Camera Object</a>
    - <a href="#parameters" class="reference internal">Parameters</a>
    - <a href="#return-values" class="reference internal">Return Values</a>
  - <a href="#camera-object-methods" class="reference internal">Camera Object Methods</a>
    - <a href="#cam-read" class="reference internal"><span class="pre"><code class="docutils literal notranslate">cam:read()</code></span></a>
    - <a href="#cam-close" class="reference internal"><span class="pre"><code class="docutils literal notranslate">cam:close()</code></span></a>
  - <a href="#configuration-table" class="reference internal">Configuration Table</a>
    - <a href="#required-fields" class="reference internal">Required fields</a>
    - <a href="#optional-fields" class="reference internal">Optional fields</a>
  - <a href="#configuration-examples" class="reference internal">Configuration Examples</a>
  - <a href="#esp32-p4-configuration" class="reference internal">ESP32-P4 Configuration</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Camera API
- <a href="../_sources/source/cam.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="camera-api" class="section">

# Camera API<a href="#camera-api" class="headerlink" title="Link to this heading"></a>

Xedge32 includes the ESP32 camera driver together with a Lua interface for capturing images from supported camera modules. This API is typically used on boards such as ESP32-CAM and ESP32-S3 camera variants where the camera wiring is fixed by the board design.

The basic workflow is:

1.  Create a camera object with <span class="pre">`esp32.cam(cfg)`</span>.

2.  Call <span class="pre">`cam:read()`</span> whenever you want a frame.

3.  Call <span class="pre">`cam:close()`</span> when you are finished.

<div class="admonition note">

Note

The camera API is still evolving, so this page focuses on the currently documented setup and configuration model.

</div>

There are two camera paths in the firmware:

- classic ESP32/ESP32-S3 camera support using the ESP-IDF camera driver and a parallel camera pin map, and

- ESP32-P4 camera support using the V4L2/PPA pipeline.

Use the section that matches your target board.

<div id="creating-a-camera-object" class="section">

## Creating a Camera Object<a href="#creating-a-camera-object" class="headerlink" title="Link to this heading"></a>

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    cam, err = esp32.cam(cfg)

</div>

</div>

<div id="parameters" class="section">

### Parameters<a href="#parameters" class="headerlink" title="Link to this heading"></a>

<span class="pre">`cfg`</span>  
Configuration table describing the camera pin mapping and runtime options.

</div>

<div id="return-values" class="section">

### Return Values<a href="#return-values" class="headerlink" title="Link to this heading"></a>

- On success, returns a camera object.

- On failure, returns <span class="pre">`nil,`</span>` `<span class="pre">`error`</span>` `<span class="pre">`message`</span>.

</div>

</div>

<div id="camera-object-methods" class="section">

## Camera Object Methods<a href="#camera-object-methods" class="headerlink" title="Link to this heading"></a>

<div id="cam-read" class="section">

### <span class="pre">`cam:read()`</span><a href="#cam-read" class="headerlink" title="Link to this heading"></a>

Captures and returns a new image from the camera.

</div>

<div id="cam-close" class="section">

### <span class="pre">`cam:close()`</span><a href="#cam-close" class="headerlink" title="Link to this heading"></a>

Closes the camera object and releases the associated hardware resources.

</div>

</div>

<div id="configuration-table" class="section">

## Configuration Table<a href="#configuration-table" class="headerlink" title="Link to this heading"></a>

The configuration table maps the camera’s parallel data bus and control signals to GPIO pins on the ESP32. In most cases, you copy the mapping used by your specific board design and only adjust optional settings when needed.

<div id="required-fields" class="section">

### Required fields<a href="#required-fields" class="headerlink" title="Link to this heading"></a>

- <span class="pre">`d0`</span> to <span class="pre">`d7`</span>: Pixel data pins.

- <span class="pre">`xclk`</span>: Camera master clock output.

- <span class="pre">`pclk`</span>: Pixel clock input.

- <span class="pre">`vsync`</span>: Vertical synchronization signal.

- <span class="pre">`href`</span>: Horizontal reference signal.

- <span class="pre">`sda`</span>: SCCB data line.

- <span class="pre">`scl`</span>: SCCB clock line.

- <span class="pre">`reset`</span>: Camera reset pin.

- <span class="pre">`freq`</span>: XCLK frequency, typically between 10 MHz and 20 MHz.

</div>

<div id="optional-fields" class="section">

### Optional fields<a href="#optional-fields" class="headerlink" title="Link to this heading"></a>

- <span class="pre">`pwdn`</span>: Camera power-down pin.

- <span class="pre">`frame`</span>: Frame size, for example <span class="pre">`"HD"`</span>.

- <span class="pre">`format`</span>: Pixel format such as <span class="pre">`"RGB565"`</span>, <span class="pre">`"YUV422"`</span>, <span class="pre">`"GRAYSCALE"`</span>, or <span class="pre">`"JPEG"`</span>.

- <span class="pre">`vflip`</span>: Boolean that flips the image vertically.

- <span class="pre">`hmirror`</span>: Boolean that mirrors the image horizontally.

</div>

</div>

<div id="configuration-examples" class="section">

## Configuration Examples<a href="#configuration-examples" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Seeed Studio XIAO ESP32S3 Sense
    local cfg = {
       d0 = 15, d1 = 17, d2 = 18, d3 = 16, d4 = 14, d5 = 12, d6 = 11, d7 = 48,
       xclk = 10, pclk = 13, vsync = 38, href = 47, sda = 40, scl = 39, pwdn = -1,
       reset = -1, freq = 20000000, frame = "HD"
    }

    -- Aideepen ESP32-CAM
    local cfg = {
       d0 = 5, d1 = 18, d2 = 19, d3 = 21, d4 = 36, d5 = 39, d6 = 34, d7 = 35,
       xclk = 0, pclk = 22, vsync = 25, href = 23, sda = 26, scl = 27, pwdn = 32,
       reset = -1, freq = 20000000, frame = "HD"
    }

    -- FREENOVE ESP32-S3 WROOM
    local cfg = {
       d0 = 11, d1 = 9, d2 = 8, d3 = 10, d4 = 12, d5 = 18, d6 = 17, d7 = 16,
       xclk = 15, pclk = 13, vsync = 6, href = 7, sda = 4, scl = 5, pwdn = -1,
       reset = -1, freq = 20000000, frame = "HD"
    }

</div>

</div>

</div>

<div id="esp32-p4-configuration" class="section">

## ESP32-P4 Configuration<a href="#esp32-p4-configuration" class="headerlink" title="Link to this heading"></a>

On ESP32-P4 builds, <span class="pre">`esp32.cam(cfg)`</span> uses a V4L2/PPA camera pipeline. The configuration is shorter because the driver negotiates the video pipeline and uses width/height settings instead of the full classic parallel pin map.

Common ESP32-P4 fields:

- <span class="pre">`scl`</span>: SCCB/I2C clock pin. Default is <span class="pre">`8`</span>.

- <span class="pre">`sda`</span>: SCCB/I2C data pin. Default is <span class="pre">`7`</span>.

- <span class="pre">`xclk`</span>: Camera master clock pin. Default is <span class="pre">`22`</span>.

- <span class="pre">`format`</span>: <span class="pre">`"JPEG"`</span>, <span class="pre">`"YUV422"`</span>, or <span class="pre">`"RGB565"`</span>.

- <span class="pre">`width`</span> and <span class="pre">`height`</span>: Requested frame size in pixels.

- <span class="pre">`frame`</span>: Convenience size when width/height are not supplied. Supported values include <span class="pre">`"QVGA"`</span>, <span class="pre">`"VGA"`</span>, <span class="pre">`"SVGA"`</span>, and <span class="pre">`"HD"`</span>.

- <span class="pre">`quality`</span>: JPEG quality from <span class="pre">`1`</span> to <span class="pre">`100`</span> when JPEG output is used.

- <span class="pre">`vflip`</span> and <span class="pre">`hmirror`</span>: Flip or mirror the final image.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local cam, err = esp32.cam{
       format = "JPEG",
       frame = "VGA",
       quality = 80,
       sda = 7,
       scl = 8,
       xclk = 22
    }

    if cam then
       local jpg = cam:read()
       trace("JPEG bytes:", #jpg)
       cam:close()
    else
       trace(err)
    end

</div>

</div>

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Start with a known-good pin map for your exact board.

- If camera initialization fails, the first thing to verify is the board’s pin mapping.

- If the image orientation is wrong, try <span class="pre">`vflip`</span> and <span class="pre">`hmirror`</span> before changing anything else.

- Lower clock rates can sometimes improve stability during bring-up.

- Camera support depends on the firmware build. If <span class="pre">`esp32.cam`</span> is not available, the current firmware was built without camera support.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="ADC.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="ADC API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="GPIO.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="GPIO API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/license.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="#" class="current reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- License
- <a href="../_sources/source/license.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="license" class="section">

# License<a href="#license" class="headerlink" title="Link to this heading"></a>

Xedge32 is built on top of components from the <a href="https://github.com/RealTimeLogic/BAS" class="reference external">Barracuda App Server amalgamated C code library</a>. That library is available under multiple licensing options, including <a href="https://github.com/RealTimeLogic/BAS#license" class="reference external">GPLv2, a free commercial license, and a standard royalty-free commercial license</a>.

The ESP32-specific south bridge implementation for Xedge32 is available in the <a href="https://github.com/RealTimeLogic/Xedge32" class="reference external">Xedge32 source repository</a> and is licensed under the permissive MIT license.

If you plan to embed Xedge32 in a commercial product, review both the Barracuda App Server licensing terms and the Xedge32 repository licensing details so your deployment model matches the correct license option.

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="applications.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Applications"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/misc.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="#" class="current reference internal">Miscellaneous API</a>
  - <a href="#esp32-apinfo" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.apinfo()</code></span></a>
  - <a href="#esp32-loglevel-level" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.loglevel(level)</code></span></a>
  - <a href="#esp32-mac" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.mac()</code></span></a>
  - <a href="#esp32-wscan-print" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.wscan([print])</code></span></a>
  - <a href="#esp32-netconnect-network-cfg" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.netconnect(network,</code></span><code class="docutils literal notranslate"> </code><span class="pre"><code class="docutils literal notranslate">cfg)</code></span></a>
    - <a href="#network-parameters" class="reference internal">Network Parameters</a>
  - <a href="#esp32-sdcard-width" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.sdcard(width)</code></span></a>
    - <a href="#sd-card-parameters" class="reference internal">SD Card Parameters</a>
    - <a href="#default-pins" class="reference internal">Default Pins</a>
    - <a href="#return-behavior" class="reference internal">Return Behavior</a>
  - <a href="#esp32-execute-command" class="reference internal"><span class="pre"><code class="docutils literal notranslate">esp32.execute(command)</code></span></a>
  - <a href="#xedge-event" class="reference internal"><span class="pre"><code class="docutils literal notranslate">xedge.event()</code></span></a>
    - <a href="#event-types" class="reference internal">Event Types</a>
    - <a href="#event-examples" class="reference internal">Event Examples</a>
    - <a href="#one-time-sntp-subscription-example" class="reference internal">One-Time SNTP Subscription Example</a>
  - <a href="#xedge32-ota" class="reference internal">Xedge32 OTA</a>
    - <a href="#examples" class="reference internal">Examples</a>
    - <a href="#ota-object-methods" class="reference internal">OTA Object Methods</a>
  - <a href="#practical-guidance" class="reference internal">Practical Guidance</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Miscellaneous API
- <a href="../_sources/source/misc.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="miscellaneous-api" class="section">

# Miscellaneous API<a href="#miscellaneous-api" class="headerlink" title="Link to this heading"></a>

The <span class="pre">`esp32`</span> module also includes a set of utility functions that do not fit under a single peripheral category. These functions cover Wi-Fi information, network connection management, SD card setup, device control actions, event subscriptions, and OTA firmware updates.

This page groups those helpers into one place so you can quickly find common system-level operations.

<div id="esp32-apinfo" class="section">

## <span class="pre">`esp32.apinfo()`</span><a href="#esp32-apinfo" class="headerlink" title="Link to this heading"></a>

Returns a Lua table describing the currently connected Wi-Fi network.

One of the most useful fields is <span class="pre">`rssi`</span>, the Received Signal Strength Indicator. RSSI is reported in dBm and is usually a negative number.

General RSSI guidance:

- <span class="pre">`-30`</span>` `<span class="pre">`dBm`</span>: Outstanding signal strength.

- <span class="pre">`-50`</span>` `<span class="pre">`dBm`</span>: Excellent signal strength.

- <span class="pre">`-60`</span>` `<span class="pre">`dBm`</span>: Good for normal use.

- <span class="pre">`-70`</span>` `<span class="pre">`dBm`</span>: Fair, but performance may start to degrade.

- <span class="pre">`-80`</span>` `<span class="pre">`dBm`</span> or lower: Weak and potentially unstable.

Example using the <a href="https://github.com/pkulchenko/serpent" class="reference external">serpent</a> module for formatting:

<div class="highlight-lua notranslate">

<div class="highlight">

    local serpent = require("serpent")
    print(serpent.block(esp32.apinfo()))

</div>

</div>

</div>

<div id="esp32-loglevel-level" class="section">

## <span class="pre">`esp32.loglevel(level)`</span><a href="#esp32-loglevel-level" class="headerlink" title="Link to this heading"></a>

Controls the ESP32 system log verbosity.

Parameter:

- <span class="pre">`level`</span>: One of <span class="pre">`"none"`</span>, <span class="pre">`"error"`</span>, <span class="pre">`"warn"`</span>, <span class="pre">`"info"`</span>, <span class="pre">`"debug"`</span>, or <span class="pre">`"verbose"`</span>.

Meaning of each level:

- <span class="pre">`"none"`</span>: Disable log output.

- <span class="pre">`"error"`</span>: Show only error messages. This is the default.

- <span class="pre">`"warn"`</span>: Show errors and warnings.

- <span class="pre">`"info"`</span>: Show errors, warnings, and informational messages.

- <span class="pre">`"debug"`</span>: Add debug messages.

- <span class="pre">`"verbose"`</span>: Show the most detailed available output.

<div class="admonition note">

Note

The maximum available log level depends on the firmware build settings in <span class="pre">`menuconfig`</span>. If you request a level that exceeds the compiled maximum, the function raises an error.

</div>

</div>

<div id="esp32-mac" class="section">

## <span class="pre">`esp32.mac()`</span><a href="#esp32-mac" class="headerlink" title="Link to this heading"></a>

Returns the ESP32 base MAC address as a 6-byte Lua string.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local mac = esp32.mac()
    print(mac:gsub(".", function(x)
       return string.format("%02X", string.byte(x))
    end))

</div>

</div>

</div>

<div id="esp32-wscan-print" class="section">

## <span class="pre">`esp32.wscan([print])`</span><a href="#esp32-wscan-print" class="headerlink" title="Link to this heading"></a>

Scans for visible Wi-Fi networks and returns a list of tables, one table per network.

If the optional argument is <span class="pre">`true`</span>, the function also prints the result to <a href="LuaShell32.html#luashell32" class="reference internal"><span class="std std-ref">LuaShell32</span></a>.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    local fmt = string.format
    for _, net in pairs(esp32.wscan()) do
       print(fmt(
          "SSID: %s\nChannel: %d\nRSSI: %d\nAuthmode: %s\nPChiper: %s\nGCipher: %s\n",
          net.ssid, net.channel, net.rssi, net.authmode, net.pchiper, net.gcipher
       ))
    end

</div>

</div>

<div class="admonition note">

Note

Wi-Fi scanning takes time. Plan for a noticeable delay before the function returns.

</div>

</div>

<div id="esp32-netconnect-network-cfg" class="section">

## <span class="pre">`esp32.netconnect(network,`</span>` `<span class="pre">`cfg)`</span><a href="#esp32-netconnect-network-cfg" class="headerlink" title="Link to this heading"></a>

Starts a Wi-Fi or wired network connection in the background and returns immediately. Connection progress and status messages are shown in <a href="LuaShell32.html#luashell32" class="reference internal"><span class="std std-ref">LuaShell32</span></a>.

If the connection succeeds, the configuration is stored in NVRAM so the device can reconnect automatically after a restart.

By default, an unconfigured system starts in Access Point Mode with SSID <span class="pre">`xedge32`</span> and password <span class="pre">`12345678`</span>.

<div id="network-parameters" class="section">

### Network Parameters<a href="#network-parameters" class="headerlink" title="Link to this heading"></a>

<span class="pre">`network`</span>  
One of:

- <span class="pre">`"wifi"`</span>: Connect through Wi-Fi.

- <span class="pre">`"W5500"`</span> or <span class="pre">`"DM9051"`</span>: Connect through SPI Ethernet when the firmware includes support for that controller.

- <span class="pre">`"IP101"`</span>, <span class="pre">`"RTL8201"`</span>, <span class="pre">`"LAN87XX"`</span>, <span class="pre">`"DP83848"`</span>, or <span class="pre">`"KSZ80XX"`</span>: Connect through RMII Ethernet when the firmware and board support that PHY.

<span class="pre">`cfg`</span>  
Configuration table. Required fields depend on the selected network type.

Wi-Fi configuration fields:

- <span class="pre">`ssid`</span>: Wi-Fi network name.

- <span class="pre">`pwd`</span>: Wi-Fi password.

SPI Ethernet configuration fields:

- <span class="pre">`spi`</span>: SPI bus number.

- <span class="pre">`clk`</span>: SPI clock GPIO.

- <span class="pre">`mosi`</span>: MOSI GPIO.

- <span class="pre">`miso`</span>: MISO GPIO.

- <span class="pre">`cs`</span>: Chip-select GPIO.

- <span class="pre">`irq`</span>: Interrupt GPIO.

- <span class="pre">`freq`</span>: SPI clock frequency in Hertz.

- <span class="pre">`rst`</span>: Optional PHY reset GPIO.

RMII Ethernet configuration fields:

- <span class="pre">`rst`</span>: PHY reset GPIO.

- <span class="pre">`mdio`</span>: MDIO GPIO.

- <span class="pre">`mdc`</span>: MDC GPIO.

If <span class="pre">`cfg`</span> is omitted while the device is already running in Wi-Fi Station Mode, the device switches back to Access Point Mode.

Examples:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Connect to Wi-Fi
    esp32.netconnect("wifi", {ssid = "My-Wi-Fi", pwd = "My-Password"})

    -- Configure Ethernet for EdgeBox-ESP-100
    esp32.netconnect("W5500", {spi = 2, clk = 13, mosi = 12, miso = 11, cs = 10, freq = 40000000, irq = 14})

    -- Configure Ethernet for LILYGO T-ETH-Lite
    esp32.netconnect("W5500", {spi = 2, clk = 13, mosi = 12, miso = 11, cs = 10, freq = 40000000, irq = 14})

    -- Configure RMII Ethernet with a DP83848 PHY
    esp32.netconnect("DP83848", {rst = 5, mdio = 18, mdc = 23})

    -- Return from Station Mode to Access Point Mode
    esp32.netconnect"wifi"

</div>

</div>

<div class="admonition note">

Note

Ethernet support is build-dependent. If an adapter name is rejected or the driver fails to start, verify that the firmware was built with the matching Ethernet PHY support and that the board wiring matches the selected adapter.

</div>

</div>

</div>

<div id="esp32-sdcard-width" class="section">

## <span class="pre">`esp32.sdcard(width)`</span><a href="#esp32-sdcard-width" class="headerlink" title="Link to this heading"></a>

Registers a new disk named <span class="pre">`sd`</span> when the board provides an SDMMC interface. After configuration, you can open the device through the <a href="https://realtimelogic.com/ba/doc/?url=lua.html#ba_ioinfo" class="reference external">IO interface</a> with <span class="pre">`ba.openio("sd")`</span>.

Supported call forms:

<div class="highlight-lua notranslate">

<div class="highlight">

    esp32.sdcard(width)
    esp32.sdcard(width, clk, cmd, d0)
    esp32.sdcard(width, clk, cmd, d0, d1, d2, d3)
    esp32.sdcard(width, clk, cmd, d0, d1, d2, d3, d4, d5, d6, d7)

</div>

</div>

<div id="sd-card-parameters" class="section">

### SD Card Parameters<a href="#sd-card-parameters" class="headerlink" title="Link to this heading"></a>

- <span class="pre">`width`</span>: Bus width. Valid values are <span class="pre">`1`</span>, <span class="pre">`4`</span>, or <span class="pre">`8`</span>.

- <span class="pre">`clk`</span>: Clock pin.

- <span class="pre">`cmd`</span>: Command pin.

- <span class="pre">`d0`</span> to <span class="pre">`d7`</span>: Data pins.

</div>

<div id="default-pins" class="section">

### Default Pins<a href="#default-pins" class="headerlink" title="Link to this heading"></a>

On **ESP32**, the SDMMC peripheral uses fixed IO-MUX-connected pins:

- <span class="pre">`clk`</span> = GPIO14

- <span class="pre">`cmd`</span> = GPIO15

- <span class="pre">`d0`</span> = GPIO2

- <span class="pre">`d1`</span> = GPIO4

- <span class="pre">`d2`</span> = GPIO12

- <span class="pre">`d3`</span> = GPIO13

- <span class="pre">`d4`</span> = GPIO33

- <span class="pre">`d5`</span> = GPIO34

- <span class="pre">`d6`</span> = GPIO35

- <span class="pre">`d7`</span> = GPIO36

On **ESP32-S3** and other targets with SDMMC GPIO matrix routing, the pins are more flexible. The default ESP32-S3 mapping is:

- <span class="pre">`clk`</span> = GPIO34

- <span class="pre">`cmd`</span> = GPIO33

- <span class="pre">`d0`</span> = GPIO37

- <span class="pre">`d1`</span> = GPIO38

- <span class="pre">`d2`</span> = GPIO39

- <span class="pre">`d3`</span> = GPIO36

- <span class="pre">`d4`</span> = GPIO35

- <span class="pre">`d5`</span> = GPIO40

- <span class="pre">`d6`</span> = GPIO42

- <span class="pre">`d7`</span> = GPIO41

</div>

<div id="return-behavior" class="section">

### Return Behavior<a href="#return-behavior" class="headerlink" title="Link to this heading"></a>

- Returns <span class="pre">`nil,`</span>` `<span class="pre">`error`</span> if the SD card cannot be detected.

- On success, stores the configuration in NVRAM and reboots the system.

- Calling the function without arguments removes an existing SD card configuration.

Custom SD card pins are only available on targets where the SDMMC peripheral can be routed through the GPIO matrix. On original ESP32 targets, use the fixed IO-MUX-connected SDMMC pins.

Examples:

<div class="highlight-lua notranslate">

<div class="highlight">

    -- Initialize a 1-bit SD card bus using the default pin mapping
    esp32.sdcard(1)

    -- Example custom mappings for specific boards
    esp32.sdcard(1, 39, 38, 40) -- ESP32-S3-WROOM CAM Board
    esp32.sdcard(1,  7,  6,  5) -- LilyGO T-ETH-Lite
    esp32.sdcard(1,  7,  9,  8) -- XIAO ESP32S3 Sense

</div>

</div>

</div>

</div>

<div id="esp32-execute-command" class="section">


## <span class="pre">`esp32.execute(command)`</span><a href="#esp32-execute-command" class="headerlink" title="Link to this heading"></a>

Performs device-level control actions.

Supported commands:

- <span class="pre">`"erase"`</span>: Erase the first FAT partition, reformat it on reboot, and restart the device. Use this if the internal FAT filesystem becomes corrupt.

- <span class="pre">`"restart"`</span>: Restart the ESP32.

- <span class="pre">`"killmain"`</span>: Stop the main process that powers LuaShell32 and reclaim its memory.

- <span class="pre">`"mdns"`</span>: Change the mDNS name.

Example:

<div class="highlight-lua notranslate">

<div class="highlight">

    esp32.execute("mdns", "myesp")
    esp32.execute"restart"

</div>

</div>

After restarting, the device can be reached at <span class="pre">`http://myesp.local`</span> if mDNS is available on your network.

</div>

<div id="xedge-event" class="section">

## <span class="pre">`xedge.event()`</span><a href="#xedge-event" class="headerlink" title="Link to this heading"></a>

Xedge32 extends the generic Xedge event mechanism so Lua code can subscribe to network-related runtime events.

Function signature:

<div class="highlight-lua notranslate">

<div class="highlight">

    xedge.event(event, callback [, unsubscribe])

</div>

</div>

All Xedge32-generated events are retained. That means a late subscriber can still receive the most recent event state when appropriate.

<div id="event-types" class="section">

### Event Types<a href="#event-types" class="headerlink" title="Link to this heading"></a>

<span class="pre">`"wifi"`</span>  
Reports Wi-Fi state changes and Wi-Fi-related warnings or errors.

Callback arguments:

- <span class="pre">`"up"`</span> when Wi-Fi transitions to connected.

- <span class="pre">`"down"`</span> when Wi-Fi transitions to disconnected.

- an ESP-IDF warning or error number as a string.

- a second argument, <span class="pre">`"ap"`</span> or <span class="pre">`"sta"`</span>, identifying Access Point or Station mode.

<span class="pre">`"wip"`</span>  
Reports that Wi-Fi has received IP configuration from DHCP.

Callback arguments:

- IP address

- netmask

- gateway

<span class="pre">`"eth"`</span>  
Reports that Ethernet has received IP configuration from DHCP.

Callback arguments:

- IP address

- netmask

- gateway

<span class="pre">`"sntp"`</span>  
Reports that the system time has been synchronized over SNTP.

This event is especially important before opening TLS connections, because certificate validation depends on the device clock being correct.

</div>

<div id="event-examples" class="section">

### Event Examples<a href="#event-examples" class="headerlink" title="Link to this heading"></a>

<div class="highlight-lua notranslate">

<div class="highlight">

    xedge.event("wifi", function(status)
       if status == "up" then
          trace("Wi-Fi connected")
       elseif status == "down" then
          trace("Wi-Fi disconnected")
       else
          trace("Wi-Fi error:", status)
       end
    end)

    xedge.event("wip", function(ip, mask, gw)
       trace("IP address:", ip, "network mask", mask, "gateway", gw)
       esp32.execute"killmain"
    end)

    xedge.event("eth", function(ip, mask, gw)
       trace("IP address:", ip, "network mask", mask, "gateway", gw)
    end)

    xedge.event("sntp", function()
       trace("Time synchronized")
    end)

</div>

</div>

</div>

<div id="one-time-sntp-subscription-example" class="section">

### One-Time SNTP Subscription Example<a href="#one-time-sntp-subscription-example" class="headerlink" title="Link to this heading"></a>

The following example subscribes to the <span class="pre">`"sntp"`</span> event, records the first time synchronization, and then unregisters itself.

<div class="highlight-lua notranslate">

<div class="highlight">

    local sntp

    function onunload()
       xedge.event("sntp", sntp, true)
    end

    sntp = function()
       onunload()
       startTime = ba.datetime"NOW"
       xedge.elog({ts = true}, "Boot time=%s", startTime:tostring())
       xedge.eflush({subject = "App starting"})
    end

    xedge.event("sntp", sntp)

</div>

</div>

<div class="admonition note">

Note

All arguments supplied by C-generated Xedge32 events are represented as Lua strings, including values that conceptually represent numbers.

</div>

</div>

</div>

<div id="xedge32-ota" class="section">

## Xedge32 OTA<a href="#xedge32-ota" class="headerlink" title="Link to this heading"></a>

Xedge32 provides core OTA support through <span class="pre">`esp32.ota`</span>.

Call forms:

- <span class="pre">`esp32.ota()`</span> returns the current firmware version table.

- <span class="pre">`esp32.ota"begin"`</span> starts an upgrade session and returns an OTA object.

<div id="examples" class="section">

### Examples<a href="#examples" class="headerlink" title="Link to this heading"></a>

Retrieve the current firmware version:

<div class="highlight-lua notranslate">

<div class="highlight">

    local ver = esp32.ota()
    for k, v in pairs(ver) do
       trace(k, v)
    end

</div>

</div>

Begin an OTA upgrade:

<div class="highlight-lua notranslate">

<div class="highlight">

    local ota, err = esp32.ota"begin"

</div>

</div>

</div>

<div id="ota-object-methods" class="section">

### OTA Object Methods<a href="#ota-object-methods" class="headerlink" title="Link to this heading"></a>

<span class="pre">`ota:write(data)`</span>  
Writes a chunk of firmware data. Keep calling this method until the full image has been written.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ok, err = ota:write(data)

</div>

</div>

<span class="pre">`ota:commit()`</span>  
Finalizes the upgrade. Returns <span class="pre">`true`</span> on success, otherwise <span class="pre">`nil,`</span>` `<span class="pre">`error`</span>.

<div class="highlight-lua notranslate">

<div class="highlight">

    local ok, err = ota:commit()

</div>

</div>

<span class="pre">`ota:abort()`</span>  
Cancels the current OTA session.

<div class="highlight-lua notranslate">

<div class="highlight">

    ota:abort()

</div>

</div>

</div>

</div>

<div id="practical-guidance" class="section">

## Practical Guidance<a href="#practical-guidance" class="headerlink" title="Link to this heading"></a>

- Use <span class="pre">`apinfo`</span> and <span class="pre">`wscan`</span> during network bring-up and troubleshooting.

- Use <span class="pre">`netconnect`</span> for both first-time network configuration and automated reconnect logic.

- Use <span class="pre">`xedge.event("sntp",`</span>` `<span class="pre">`...)`</span> before TLS-dependent application startup if accurate time matters.

- Use OTA only after verifying image integrity and transport reliability in your application workflow.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="UART.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="UART API"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="BME280.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="BME280 Module">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/tutorials.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="xedge.html" class="reference internal">Xedge32</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="#" class="current reference internal">Tutorials and AUX APIs</a>
  - <a href="#recommended-learning-resources" class="reference internal">Recommended Learning Resources</a>
  - <a href="#how-xedge32-fits-together" class="reference internal">How Xedge32 Fits Together</a>
  - <a href="#why-this-matters" class="reference internal">Why This Matters</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Tutorials and AUX APIs
- <a href="../_sources/source/tutorials.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="tutorials-and-aux-apis" class="section">

# Tutorials and AUX APIs<a href="#tutorials-and-aux-apis" class="headerlink" title="Link to this heading"></a>

This documentation set focuses on the Xedge32-specific south bridge APIs. In practice, most Xedge32 projects also make use of the broader Lua, Xedge, and Barracuda App Server documentation. The links below are the quickest way to expand beyond the ESP32 hardware pages in this manual.

<div id="recommended-learning-resources" class="section">

## Recommended Learning Resources<a href="#recommended-learning-resources" class="headerlink" title="Link to this heading"></a>

- <a href="https://realtimelogic.com/articles/How-Xedge32-Xedge-and-Barracuda-App-Server-Work-Together" class="reference external">How Xedge32, Xedge, and Barracuda App Server Work Together</a> The best first read when you need to understand which documentation set to use for each layer.

- <a href="https://realtimelogic.com/xedge32-tutorials/" class="reference external">Xedge32 Tutorials</a> A collection of step-by-step guides focused on getting productive with Xedge32.

- <a href="https://tutorial.realtimelogic.com/Lua-Types.lsp" class="reference external">Online Interactive Lua Tutorial</a> A hands-on introduction to Lua for users who are new to the language.

- <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/ESP32" class="reference external">GitHub Xedge32 Examples</a> Ready-to-run example programs and reference code for ESP32 targets.

- <a href="https://makoserver.net/tutorials/" class="reference external">Mako Server Tutorials</a> Broader tutorials that often apply directly to Xedge32 because both products share the same underlying development model.

</div>

<div id="how-xedge32-fits-together" class="section">

## How Xedge32 Fits Together<a href="#how-xedge32-fits-together" class="headerlink" title="Link to this heading"></a>

Xedge32 is easiest to understand when you view it as three layers working together:

1.  **Barracuda App Server** This is the underlying runtime and IoT foundation. It provides the majority of the network, protocol, security, storage, and web capabilities used by Xedge32 applications.

    - <a href="https://realtimelogic.com/ba/doc/?url=lua.html" class="reference external">Lua API</a>

    - <a href="https://realtimelogic.com/ba/doc/?url=IoT.html" class="reference external">IoT Protocols</a>

    - <a href="https://realtimelogic.com/products/barracuda-application-server/" class="reference external">Product page</a>

2.  **Xedge** Xedge is the Lua-oriented development environment and workflow layer. It gives you the browser-based editor, rapid prototyping model, and application runtime used on several platforms, not only ESP32.

    - <a href="https://realtimelogic.com/ba/doc/?url=Xedge.html" class="reference external">Xedge API and how-to documentation</a>

    - <a href="https://tutorial.realtimelogic.com/rtl/" class="reference external">Online demo</a>

3.  **Xedge32 for ESP32** This is the ESP32-specific extension layer documented in this manual. It adds the south bridge API for GPIO, ADC, UART, RMT, camera access, and other peripherals.

    - <a href="../index.html" class="reference external">Introduction</a>

</div>

<div id="why-this-matters" class="section">

## Why This Matters<a href="#why-this-matters" class="headerlink" title="Link to this heading"></a>

New users often start by reading only the ESP32 peripheral pages. That is a good starting point, but most real applications combine:

- Xedge32 peripheral access,

- generic Xedge application behavior, and

- Barracuda App Server networking or IoT protocol features.

If you keep those three layers in mind while learning, the rest of the documentation becomes much easier to navigate.

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="RTU.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="Modbus RTU Module"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="applications.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="Applications">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>


---

# Source: ESP32/source/xedge.html

<div class="wy-grid-for-nav">

<div class="wy-side-scroll">

<div class="wy-side-nav-search">

<a href="../index.html" class="icon icon-home">Xedge32</a>

<div role="search">

</div>

</div>

<div class="wy-menu wy-menu-vertical" spy="affix" role="navigation" aria-label="Navigation menu">

<span class="caption-text">Contents:</span>

- <a href="GettingStarted.html" class="reference internal">Getting Started</a>
- <a href="AccessPointMode.html" class="reference internal">Access Point Mode</a>
- <a href="LuaShell32.html" class="reference internal">LuaShell32</a>
- <a href="#" class="current reference internal">Xedge32</a>
  - <a href="#working-with-example-applications" class="reference internal">Working with Example Applications</a>
  - <a href="#uploading-files-to-the-esp32" class="reference internal">Uploading Files to the ESP32</a>
  - <a href="#creating-an-app-from-the-uploaded-directory" class="reference internal">Creating an App from the Uploaded Directory</a>
  - <a href="#using-an-external-ide" class="reference internal">Using an External IDE</a>
  - <a href="#debugging-lua-code" class="reference internal">Debugging Lua Code</a>
  - <a href="#debugging-workflow" class="reference internal">Debugging Workflow</a>
  - <a href="#starting-the-first-debug-session" class="reference internal">Starting the First Debug Session</a>
  - <a href="#further-reading" class="reference internal">Further Reading</a>
- <a href="ADC.html" class="reference internal">ADC API</a>
- <a href="cam.html" class="reference internal">Camera API</a>
- <a href="GPIO.html" class="reference internal">GPIO API</a>
- <a href="I2C.html" class="reference internal">I2C API</a>
- <a href="PCNT.html" class="reference internal">PCNT API</a>
- <a href="PWM.html" class="reference internal">PWM API</a>
- <a href="RMT.html" class="reference internal">RMT API</a>
- <a href="UART.html" class="reference internal">UART API</a>
- <a href="misc.html" class="reference internal">Miscellaneous API</a>
- <a href="BME280.html" class="reference internal">BME280 Module</a>
- <a href="RTU.html" class="reference internal">Modbus RTU Module</a>
- <a href="tutorials.html" class="reference internal">Tutorials and AUX APIs</a>
- <a href="applications.html" class="reference internal">Applications</a>
- <a href="license.html" class="reference internal">License</a>

</div>

</div>

<div class="section wy-nav-content-wrap" toggle="wy-nav-shift">

[Xedge32](../index.html)

<div class="wy-nav-content">

<div class="rst-content">

<div role="navigation" aria-label="Page navigation">

- <a href="../index.html" class="icon icon-home" aria-label="Home"></a>
- Xedge32
- <a href="../_sources/source/xedge.rst.txt" rel="nofollow">View page source</a>

------------------------------------------------------------------------

</div>

<div class="document" role="main" itemscope="itemscope" itemtype="http://schema.org/Article">

<div itemprop="articleBody">

<div id="xedge32" class="section">


# Xedge32<a href="#xedge32" class="headerlink" title="Link to this heading"></a>

Xedge32 is a development environment for embedded edge devices. It combines a web-based interface with Lua-driven application development so you can build, test, and iterate directly on the target device.


Xedge itself is available on multiple platforms, but this guide focuses on the ESP32-specific workflow. For the broader Xedge feature set, see the <a href="https://realtimelogic.com/ba/doc/?url=Xedge.html" class="reference external">Xedge main documentation</a>.

<div id="working-with-example-applications" class="section">

## Working with Example Applications<a href="#working-with-example-applications" class="headerlink" title="Link to this heading"></a>

The <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/ESP32" class="reference external">LSP-Examples repository</a> contains a good set of ESP32-oriented examples. A practical way to learn Xedge32 is to upload these examples, open them in the browser IDE, and run them directly on the device.

Basic workflow:

1.  Download the ESP32 example files to your computer.

2.  Upload them to the ESP32 using WebDAV or the built-in Web File Manager.

3.  Create a new application from the uploaded directory.

4.  Open the example files in the IDE and run them one by one.

</div>

<div id="uploading-files-to-the-esp32" class="section">

## Uploading Files to the ESP32<a href="#uploading-files-to-the-esp32" class="headerlink" title="Link to this heading"></a>

You can use the combined WebDAV and Web File Manager endpoint at <span class="pre">`http://ip-address/rtl/apps/`</span>.

If you want a walkthrough for mounting the device as a WebDAV drive, see the <a href="https://youtu.be/i5ubScGwUOc" class="reference download external" download=""><span class="pre"><code class="xref download docutils literal notranslate">WebDAV</code></span><code class="xref download docutils literal notranslate"> </code><span class="pre"><code class="xref download docutils literal notranslate">how-to</code></span><code class="xref download docutils literal notranslate"> </code><span class="pre"><code class="xref download docutils literal notranslate">video</code></span></a>.

If you prefer the browser workflow:

1.  Open <span class="pre">`http://ip-address/rtl/apps/`</span>.

2.  Click the <span class="guilabel">+</span> button and create a directory named <span class="pre">`Lua-Examples`</span>.

3.  Open the new directory at <span class="pre">`http://ip-address/rtl/apps/Lua-Examples/`</span>.

4.  Drag and drop the downloaded example files into the browser window.

The upload starts as soon as you drop the files into the page.

</div>

<div id="creating-an-app-from-the-uploaded-directory" class="section">

## Creating an App from the Uploaded Directory<a href="#creating-an-app-from-the-uploaded-directory" class="headerlink" title="Link to this heading"></a>

After the files have been uploaded:

1.  Navigate to <span class="pre">`http://ip-address/rtl/`</span>.

2.  Expand <span class="guilabel">disk</span> in the left pane.

3.  Right-click the <span class="pre">`Lua-Examples`</span> directory.

4.  Select <span class="guilabel">New App</span>.

5.  Enable <span class="guilabel">Running</span> and <span class="guilabel">LSP App</span>.

6.  Click <span class="guilabel">Save</span>.

You can now expand the example application in the left pane, open any example file in the editor, and run it directly from the IDE.

</div>

<div id="using-an-external-ide" class="section">

## Using an External IDE<a href="#using-an-external-ide" class="headerlink" title="Link to this heading"></a>

The built-in web IDE is excellent for quick iteration, but an external editor or IDE may be more comfortable for larger projects. You can edit files directly on the ESP32 by mounting the device as a WebDAV drive.

For operating-system-specific instructions, see the <a href="https://fuguhub.com/FileServer.lsp" class="reference external">How to Mount a WebDAV Drive</a> guide.

The WebDAV URL is:

<div class="highlight-default notranslate">

<div class="highlight">

    https://ip-address/rtl/apps/

</div>

</div>

</div>

<div id="debugging-lua-code" class="section">


## Debugging Lua Code<a href="#debugging-lua-code" class="headerlink" title="Link to this heading"></a>

Remote debugging is supported through the Barracuda App Server <a href="https://realtimelogic.com/ba/doc/?url=auxlua.html#dbgmon" class="reference external">Lua debug module</a>. This workflow integrates with Visual Studio Code and lets you debug Lua running on the device.


One important detail is that breakpoints pause the server. Because of that, the device’s WebDAV service is also paused while the debugger is stopped. For this reason, it is better to keep your source files on the host computer and let the device access them through NetIo during a debug session.

For the NetIo background, see the <a href="https://realtimelogic.com/ba/doc/?url=xedge/readme.html#netio" class="reference external">additional Xedge documentation</a>.

</div>

<div id="debugging-workflow" class="section">

## Debugging Workflow<a href="#debugging-workflow" class="headerlink" title="Link to this heading"></a>

Follow the <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/Lua-Debug" class="reference external">How to Debug Lua Code Using Visual Studio Code</a> guide and complete the prerequisites listed there.

Then:

1.  Copy the <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/Lua-Debug/FileServer" class="reference external">File Server directory</a> into <span class="pre">`xedge-ESP32/BAS/examples/xedge/FileServer`</span> on your host machine.

2.  Start the Mako Server from <span class="pre">`xedge-ESP32/BAS/examples/xedge`</span>:

    <div class="cmd docutils container">

    mako -l::FileServer

    </div>

    If <span class="pre">`mako`</span> is not in your path:

    <div class="cmd docutils container">

    /path/2/mako -l::FileServer

    </div>

3.  When the File Server opens in the browser, click <span class="pre">`Lua-Examples`</span> and copy the full URL shown by the file server.

4.  In Xedge32, open <span class="pre">`http://ip-address/rtl/`</span>, right-click <span class="guilabel">net</span>, and paste that URL into the app dialog.

5.  Enable <span class="guilabel">Running</span> and <span class="guilabel">LSP App</span>, then click <span class="guilabel">Save</span>.

As soon as the device connects to the File Server app, you should see a message similar to:

<div class="highlight-default notranslate">

<div class="highlight">

    Creating 'Visual Studio Code' config file:   Lua-Examples/.vscode/launch.json

</div>

</div>

The generated <span class="pre">`launch.json`</span> includes the required <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/Lua-Debug#setting-up-sourcemaps-in-launchjson" class="reference external">sourceMaps attribute</a>.

</div>

<div id="starting-the-first-debug-session" class="section">

## Starting the First Debug Session<a href="#starting-the-first-debug-session" class="headerlink" title="Link to this heading"></a>

1.  Open the local directory <span class="pre">`xedge-ESP32/BAS/examples/xedge/Lua-Examples`</span> in Visual Studio Code.

2.  Open <span class="pre">`httpclient.lsp`</span> and add the following lines near the top of the file:

    <div class="highlight-lua notranslate">

    <div class="highlight">

        require"ldbgmon".connect{client=false}
        trace"Running LSP page"

    </div>

    </div>

3.  In a browser, open <span class="pre">`http://ip-address/LuaExamples/httpclient.lsp`</span>.

4.  The page will appear to hang. That is expected because the web server is now waiting for the debugger to attach.

5.  Press <span class="pre">`F5`</span> in Visual Studio Code to start debugging.

Once the debugger attaches, execution stops automatically. You can now step through the code, resume execution, and set breakpoints for later runs.

When you refresh the page again, the new breakpoint is hit. You can even step into library code such as <a href="https://realtimelogic.com/ba/doc/?url=auxlua.html#managed" class="reference external">httpm</a>, including Lua code embedded in the firmware image.

</div>

<div id="further-reading" class="section">

## Further Reading<a href="#further-reading" class="headerlink" title="Link to this heading"></a>

Before diving deeply into application development, it helps to read the following:

- <a href="https://tutorial.realtimelogic.com/" class="reference external">Online Lua tutorials</a>

- <a href="https://realtimelogic.com/xedge32-tutorials/" class="reference external">Xedge32 tutorials</a>

- <a href="https://realtimelogic.com/ba/doc/?url=Xedge.html" class="reference external">Xedge documentation</a>

- <a href="https://github.com/RealTimeLogic/LSP-Examples/tree/master/ESP32" class="reference external">ESP32 example section in LSP-Examples</a>

Additional articles that work well with Xedge32:

- <a href="https://makoserver.net/articles/How-to-Connect-to-AWS-IoT-Core-using-MQTT-amp-ALPN" class="reference external">How to connect the ESP32 to an MQTT cloud server using Mutual TLS Authentication</a>

- <a href="https://makoserver.net/articles/How-to-Build-an-Interactive-Dashboard-App" class="reference external">How to design a web user interface by using a ready to use dashboard app template</a>

- <a href="https://realtimelogic.com/articles/Rapid-Firmware-Development-with-the-Barracuda-App-Server" class="reference external">How to perform rapid IoT device client development using Lua</a>

- <a href="https://makoserver.net/articles/Secure-Remote-Access" class="reference external">How to access the web server behind a firewall without using port forwarding</a>

- <a href="https://makoserver.net/articles/Ajax-for-Beginners" class="reference external">Ajax for beginners</a>

- <a href="https://realtimelogic.com/articles/Modern-Approach-to-Embedding-a-Web-Server-in-a-Device" class="reference external">How to design a modern multiuser reactive web interface</a>

- <a href="https://realtimelogic.com/articles/Lua-FastTracks-Embedded-Web-Application-Development" class="reference external">Why use Lua for embedded web application development</a>

- <a href="https://realtimelogic.com/articles/OnPremises-IoT-Platform" class="reference external">Using the ESP32 as a foundation for your On-Premises IoT Platform</a>

</div>

</div>

</div>

</div>

<div class="rst-footer-buttons" role="navigation" aria-label="Footer">

<a href="LuaShell32.html" class="btn btn-neutral float-left" accesskey="p" rel="prev" title="LuaShell32"><span class="fa fa-arrow-circle-left" aria-hidden="true"></span> Previous</a> <a href="ADC.html" class="btn btn-neutral float-right" accesskey="n" rel="next" title="ADC API">Next <span class="fa fa-arrow-circle-right" aria-hidden="true"></span></a>

</div>

------------------------------------------------------------------------

<div role="contentinfo">

© Copyright 2026, RTL.

</div>

Built with [Sphinx](https://www.sphinx-doc.org/) using a [theme](https://github.com/readthedocs/sphinx_rtd_theme) provided by [Read the Docs](https://readthedocs.org).

</div>

</div>

</div>

</div>
