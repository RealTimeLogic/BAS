<script>
  import { onMount } from 'svelte';
  import { api } from './lib/api.js';
  import Icon from './lib/Icon.svelte';

  const devices = [
    { id: 1, name: 'Gateway Core', type: 'Edge controller', location: 'Line A', accent: 'mint' },
    { id: 7, name: 'I/O Node 07', type: 'Digital I/O', location: 'Cell 3', accent: 'neutral' },
    { id: 42, name: 'Sensor Hub', type: 'Telemetry unit', location: 'Utility room', accent: 'amber' }
  ];

  let connection = 'checking';
  let lastSync = 'Not synchronized';
  let latency = null;
  let busy = '';
  let notice = '';
  let noticeType = 'success';
  let pin = 12;
  let pinState = true;
  let inspectMode = 'live';
  let inspectOutput = 'Run a request inspection to see the server context.';
  let streamOutput = 'Waiting for stream test.';
  let checksumInput = 'fieldline-device';
  let checksumOutput = '—';
  let sidebarOpen = false;
  let activity = [
    { time: 'Now', title: 'Console initialized', detail: 'Waiting for the device API' }
  ];

  function nowLabel() {
    return new Intl.DateTimeFormat(undefined, {
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit'
    }).format(new Date());
  }

  function addActivity(title, detail) {
    activity = [{ time: nowLabel(), title, detail }, ...activity].slice(0, 5);
  }

  function showNotice(message, type = 'success') {
    notice = message;
    noticeType = type;
    window.setTimeout(() => {
      if (notice === message) notice = '';
    }, 4200);
  }

  function errorMessage(error) {
    return error?.message || 'The operation could not be completed.';
  }

  async function refreshHealth(quiet = false) {
    busy = 'health';
    const started = performance.now();
    try {
      const [health, deviceStatus] = await Promise.all([
        api.health(),
        api.deviceStatus()
      ]);
      latency = Math.max(1, Math.round(performance.now() - started));
      connection = health?.status === 'ok' ? 'online' : 'degraded';
      lastSync = `Updated ${nowLabel()}`;
      if (quiet) {
        activity = [{
          time: nowLabel(),
          title: 'Device connected',
          detail: `${deviceStatus} · ${latency} ms`
        }];
      } else {
        addActivity('Health check passed', `${deviceStatus} · ${latency} ms`);
        showNotice('Device status refreshed.');
      }
    } catch (error) {
      connection = 'offline';
      lastSync = 'Connection failed';
      if (!quiet) showNotice(errorMessage(error), 'error');
    } finally {
      busy = '';
    }
  }

  async function verifyDevice(device) {
    busy = `device-${device.id}`;
    try {
      const response = await api.device(device.id);
      addActivity(`${device.name} verified`, response);
      showNotice(`${device.name} answered successfully.`);
    } catch (error) {
      showNotice(errorMessage(error), 'error');
    } finally {
      busy = '';
    }
  }

  async function applyGpio() {
    busy = 'gpio';
    try {
      await api.setGpio(pin, pinState ? 1 : 0);
      const stateLabel = pinState ? 'HIGH' : 'LOW';
      addActivity(`GPIO ${pin} set ${stateLabel}`, 'Command accepted by the controller');
      showNotice(`GPIO ${pin} is now ${stateLabel}.`);
    } catch (error) {
      showNotice(errorMessage(error), 'error');
    } finally {
      busy = '';
    }
  }

  async function runInspection() {
    busy = 'inspect';
    try {
      document.cookie = 'session=fieldline-console; SameSite=Lax; path=/';
      inspectOutput = await api.inspect(inspectMode, 'console-web');
      addActivity('Request context inspected', `Mode: ${inspectMode}`);
    } catch (error) {
      inspectOutput = errorMessage(error);
      showNotice(inspectOutput, 'error');
    } finally {
      busy = '';
    }
  }

  async function runStream() {
    busy = 'stream';
    streamOutput = '';
    try {
      streamOutput = await api.stream((text) => {
        streamOutput = text;
      });
      addActivity('Stream completed', 'Received five response chunks');
    } catch (error) {
      streamOutput = errorMessage(error);
      showNotice(streamOutput, 'error');
    } finally {
      busy = '';
    }
  }

  async function runLatency() {
    busy = 'latency';
    const started = performance.now();
    try {
      await api.latency(250);
      latency = Math.round(performance.now() - started);
      addActivity('Worker concurrency tested', `${latency} ms elapsed`);
      showNotice('Background operation completed without blocking the server.');
    } catch (error) {
      showNotice(errorMessage(error), 'error');
    } finally {
      busy = '';
    }
  }

  async function calculateChecksum() {
    busy = 'checksum';
    try {
      const result = await api.checksum(checksumInput);
      checksumOutput = `${result.bytes} bytes · checksum ${result.checksum}`;
      addActivity('Payload processed', checksumOutput);
    } catch (error) {
      checksumOutput = errorMessage(error);
      showNotice(checksumOutput, 'error');
    } finally {
      busy = '';
    }
  }

  onMount(() => refreshHealth(true));
</script>

<svelte:head>
  <title>Fieldline Device Console</title>
</svelte:head>

<div class="app-shell">
  <aside class:open={sidebarOpen} class="sidebar">
    <div class="brand">
      <div class="brand-mark"><span></span><span></span><span></span></div>
      <div>
        <strong>Fieldline</strong>
        <small>DEVICE CONSOLE</small>
      </div>
    </div>

    <nav aria-label="Primary navigation">
      <a class="nav-item active" href="#overview" onclick={() => (sidebarOpen = false)}>
        <Icon name="overview" /> <span>Overview</span>
      </a>
      <a class="nav-item" href="#devices" onclick={() => (sidebarOpen = false)}>
        <Icon name="device" /> <span>Devices</span><b>3</b>
      </a>
      <a class="nav-item" href="#controls" onclick={() => (sidebarOpen = false)}>
        <Icon name="control" /> <span>Controls</span>
      </a>
      <a class="nav-item" href="#diagnostics" onclick={() => (sidebarOpen = false)}>
        <Icon name="pulse" /> <span>Diagnostics</span>
      </a>
    </nav>

    <div class="sidebar-foot">
      <div class="security-mark"><Icon name="shield" size={18} /></div>
      <div><strong>Local connection</strong><small>Native BAS/BWS API</small></div>
    </div>
  </aside>

  {#if sidebarOpen}
    <button class="scrim" aria-label="Close navigation" onclick={() => (sidebarOpen = false)}></button>
  {/if}

  <main>
    <header class="topbar">
      <button class="menu-button" aria-label="Open navigation" onclick={() => (sidebarOpen = true)}>
        <Icon name="menu" />
      </button>
      <div class="connection-pill {connection}">
        <span></span>
        {connection === 'online' ? 'Device online' : connection === 'checking' ? 'Connecting' : 'Device offline'}
      </div>
      <div class="topbar-actions">
        <code>GET /api/health</code>
        <button class="icon-button" aria-label="Refresh device status" onclick={() => refreshHealth()} disabled={busy === 'health'}>
          <Icon name="refresh" />
        </button>
        <div class="avatar" aria-label="Operator profile">OP</div>
      </div>
    </header>

    <div class="content">
      <section id="overview" class="hero">
        <div>
          <p class="eyebrow">OPERATIONS / EDGE CONTROLLER</p>
          <h1>Device overview</h1>
          <p class="hero-copy">Monitor the native server, manage connected nodes, and exercise the REST interface from one compact console.</p>
        </div>
        <button class="primary-button" onclick={() => refreshHealth()} disabled={busy === 'health'}>
          <Icon name="refresh" size={18} /> {busy === 'health' ? 'Refreshing…' : 'Refresh status'}
        </button>
      </section>

      <section class="metrics" aria-label="Device summary">
        <article class="metric-card">
          <div class="metric-icon mint"><Icon name="pulse" /></div>
          <div><span>API status</span><strong class:good={connection === 'online'}>{connection}</strong></div>
          <small>{lastSync}</small>
        </article>
        <article class="metric-card">
          <div class="metric-icon neutral"><Icon name="activity" /></div>
          <div><span>Round trip</span><strong>{latency === null ? '—' : `${latency} ms`}</strong></div>
          <small>Last measured response</small>
        </article>
        <article class="metric-card">
          <div class="metric-icon amber"><Icon name="device" /></div>
          <div><span>Managed nodes</span><strong>3</strong></div>
          <small>Example device inventory</small>
        </article>
        <article class="metric-card">
          <div class="metric-icon amber"><Icon name="bolt" /></div>
          <div><span>REST routes</span><strong>8</strong></div>
          <small>Fixed-capacity router</small>
        </article>
      </section>

      <div class="layout-grid">
        <section id="devices" class="panel devices-panel">
          <div class="panel-heading">
            <div><p class="eyebrow">INVENTORY</p><h2>Managed devices</h2></div>
            <span class="subtle-badge">3 nodes</span>
          </div>
          <div class="device-list">
            {#each devices as device}
              <article class="device-row">
                <div class="device-symbol {device.accent}"><Icon name="device" /></div>
                <div class="device-copy">
                  <strong>{device.name}</strong>
                  <span>{device.type} · {device.location}</span>
                </div>
                <div class="device-state"><span></span> Ready</div>
                <button class="text-button" onclick={() => verifyDevice(device)} disabled={busy === `device-${device.id}`}>
                  {busy === `device-${device.id}` ? 'Checking…' : 'Verify'} <Icon name="chevron" size={16} />
                </button>
              </article>
            {/each}
          </div>
        </section>

        <section id="controls" class="panel control-panel">
          <div class="panel-heading">
            <div><p class="eyebrow">DIRECT CONTROL</p><h2>GPIO output</h2></div>
            <div class="metric-icon mint compact"><Icon name="control" size={18} /></div>
          </div>
          <p class="panel-copy">Issue a bounded device command through the typed REST route.</p>
          <label for="pin">GPIO pin <span>0–63</span></label>
          <input id="pin" type="number" min="0" max="63" bind:value={pin} />
          <div class="switch-row">
            <div><strong>Output state</strong><span>{pinState ? 'HIGH · Enabled' : 'LOW · Disabled'}</span></div>
            <button class:enabled={pinState} class="switch" role="switch" aria-label="Toggle GPIO output state" aria-checked={pinState} onclick={() => (pinState = !pinState)}><span></span></button>
          </div>
          <button class="primary-button full" onclick={applyGpio} disabled={busy === 'gpio' || !Number.isInteger(pin) || pin < 0 || pin > 63}>
            <Icon name="bolt" size={18} /> {busy === 'gpio' ? 'Applying…' : 'Apply output'}
          </button>
          <code class="endpoint">POST /api/gpio/{pin}/set/{pinState ? 1 : 0}</code>
        </section>
      </div>

      <section id="diagnostics" class="diagnostics-grid">
        <article class="panel diagnostic-card">
          <div class="panel-heading"><div><p class="eyebrow">REQUEST CONTEXT</p><h2>Inspect request</h2></div><Icon name="terminal" /></div>
          <div class="inline-form">
            <select bind:value={inspectMode} aria-label="Inspection mode">
              <option value="live">Live mode</option>
              <option value="safe">Safe mode</option>
              <option value="maintenance">Maintenance</option>
            </select>
            <button class="secondary-button" onclick={runInspection} disabled={busy === 'inspect'}>{busy === 'inspect' ? 'Running…' : 'Run'}</button>
          </div>
          <pre>{inspectOutput}</pre>
        </article>

        <article class="panel diagnostic-card">
          <div class="panel-heading"><div><p class="eyebrow">TRANSFER</p><h2>Stream response</h2></div><Icon name="activity" /></div>
          <p class="panel-copy">Verify incremental output and response flushing.</p>
          <pre>{streamOutput}</pre>
          <button class="secondary-button" onclick={runStream} disabled={busy === 'stream'}>{busy === 'stream' ? 'Receiving…' : 'Start stream test'}</button>
        </article>

        <article class="panel diagnostic-card">
          <div class="panel-heading"><div><p class="eyebrow">PAYLOAD</p><h2>Body checksum</h2></div><Icon name="shield" /></div>
          <input aria-label="Checksum payload" bind:value={checksumInput} maxlength="256" />
          <div class="result-line"><span>Server result</span><strong>{checksumOutput}</strong></div>
          <button class="secondary-button" onclick={calculateChecksum} disabled={busy === 'checksum' || !checksumInput}>{busy === 'checksum' ? 'Sending…' : 'Send payload'}</button>
        </article>

        <article class="panel diagnostic-card latency-card">
          <div class="panel-heading"><div><p class="eyebrow">CONCURRENCY</p><h2>Worker test</h2></div><Icon name="pulse" /></div>
          <div class="latency-visual"><span style={`--latency: ${Math.min(latency || 22, 100)}%`}></span></div>
          <div class="result-line"><span>Configured delay</span><strong>250 ms</strong></div>
          <button class="secondary-button" onclick={runLatency} disabled={busy === 'latency'}>{busy === 'latency' ? 'Waiting…' : 'Run concurrency test'}</button>
        </article>
      </section>

      <section class="panel activity-panel">
        <div class="panel-heading"><div><p class="eyebrow">AUDIT TRAIL</p><h2>Recent activity</h2></div><span class="subtle-badge">Local session</span></div>
        <div class="timeline">
          {#each activity as item, index}
            <div class="timeline-item">
              <span class:latest={index === 0}></span>
              <time>{item.time}</time>
              <div><strong>{item.title}</strong><small>{item.detail}</small></div>
            </div>
          {/each}
        </div>
      </section>
    </div>
  </main>
</div>

{#if notice}
  <div class="toast {noticeType}" role="status">
    <span>{noticeType === 'success' ? '✓' : '!'}</span>{notice}
    <button aria-label="Dismiss notification" onclick={() => (notice = '')}><Icon name="close" size={16} /></button>
  </div>
{/if}
